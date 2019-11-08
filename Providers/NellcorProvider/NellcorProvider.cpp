/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/18
 **/

#include "NellcorProvider.h"
#include "SPO2Param.h"
#include "SPO2Alarm.h"
#include "Debug.h"
#include "NIBPParam.h"
#include "AlarmSourceManager.h"
#include "Framework/Language/LanguageManager.h"

enum OxismartReport
{
    KEY = 0,
    LEN,
    SAT_DISP_TYPE,
    SPO2_VALUE,
    RATE_DISP_TYPE,
    RATE_BPM_VALUE_TOP_EIGHT,
    STATUS_AND_ALARM_WORD_TOP_EIGHT,
    STATUS_AND_ALARM_WORD_DOWN_EIGHT,
    OXISMART_REPORT_NR
};

enum NellcorPacketType
{
    NELLCOR_HEAD = 0X55,
    NELLCOR_STX = 0X02,
    NELLCOR_ETX = 0X03,
    NELLCOR_VER_INFO = 'V',
    NELLCOR_SAT_RATE_INFO = '!',  // 血氧、脉率值及其状态信息、模块复位信息  关掉
    NELLCOR__SPO2PR_VAL = 'j',  // 血氧、脉率值及其状态信息 更实时
    NELLCOR_ALARM_LIMIT = 'h',
    NELLCOR_SAT_SEC = 'u',  // 数据越界（高低限）容忍时间
    NELLCOR_SAT_SEC_REPORT = 'U',  // 数据越界（高低限）容忍时间报告
    NELLCOR_WAVE_BLIP = '~',
    NELLCOR_RESET_INFO = 'A',  // 系统复位信息
    NELLCOR_HOST_SENSOR_KEY_INFO = 'Q',  // 主机传感器密钥信息
    NELLCOR_IR_PER_VALUE = '%',  // IR百分比调制报告
    NELLCOR_WAVE_UPDATE_INFO = 'W',  // 波形更新率信息
    NELLCOR_WAVE_SELECT_INFO = 'w',  // 波形选择信息
    NELLCOR_C_CLOCK_INFO = 'Z',  // C_CLOCK相关信息
    NELLCOR_ECG_TRIGER_INFO = '^',  // ECG触发器信息
};

enum Spo2ValueLimit
{
    SPO2_VALUE_NORMAL = 0,
    SPO2_VALUE_HIGH,
    SPO2_VALUE_LOW
};

enum PRValueLimit
{
    PR_VALUE_NORMAL = 0,
    PR_VALUE_HIGH,
    PR_VALUE_LOW
};

enum VolumeWarnPriority
{
    VOL_WAR_PRI_NOR = 0,
    VOL_WAR_PRI_LOW,
    VOL_WAR_PRI_MID,
    VOL_WAR_PRI_HIGH
};

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool NellcorSetProvider::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_SPO2)
    {
        spo2Param.setProvider(this);
        Provider::attachParam(param);
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void NellcorSetProvider::dataArrived(void)
{
    readData();
    if (ringBuff.dataSize() < _minPacketLen)
    {
        return;
    }

    unsigned char buff[64] = {0};
    while (ringBuff.dataSize() >= _minPacketLen)
    {
        if (ringBuff.at(0) != NELLCOR_HEAD)
        {
            // debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        if (ringBuff.at(1) != NELLCOR_STX)
        {
            ringBuff.pop(1);
            continue;
        }

        if (ringBuff.at(ringBuff.at(2) + 4) != NELLCOR_ETX)
        {
            // debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        int length = ringBuff.at(2) + 5;
        // 将数据包读到buff中。
        for (int i = 0; (i < static_cast<int>(sizeof(buff)) && i < length); i++)
        {
            buff[i] = ringBuff.at(0);
            ringBuff.pop(1);
        }

        unsigned char sum = _calcCheckSum(&buff[3], buff[2]);
        if (buff[3 + buff[2]] == sum)
        {
            handlePacket(&buff[3], buff[2]);
        }
        else
        {
            outHex(buff, buff[2] + 5);
            debug("0x%02x", sum);
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

void NellcorSetProvider::setSensitive(SPO2Sensitive sens)
{
    unsigned char spo2Sens = 0;
    switch (sens)
    {
    case SPO2_SENS_LOW:
        spo2Sens = 4;
        break;
    case SPO2_SENS_MED:
        spo2Sens = 2;
        break;
    case SPO2_SENS_HIGH:
        spo2Sens = 1;
        break;
    case SPO2_SENS_NR:
        spo2Sens = 4;
        break;
    }
    unsigned char data[3] = {'W', 0x01, 0x00};
    data[2] = spo2Sens;
    _sendCmd(data, 3);
}

/**************************************************************************************************
 * send to code.
 *************************************************************************************************/
void NellcorSetProvider::_sendCmd(const unsigned char *data, unsigned int len)
{
    int index = 0;
    unsigned int lenTemp = len;
    unsigned char sendBuf[32] = {0};

    if (lenTemp > sizeof(sendBuf) - 4)
    {
        lenTemp = sizeof(sendBuf) - 4;
    }

    sendBuf[index++] = NELLCOR_HEAD;
    sendBuf[index++] = NELLCOR_STX;
    sendBuf[index++] = lenTemp;

    for (unsigned int i = 0; i < lenTemp; i++)
    {
        sendBuf[index++] = data[i];
    }

    sendBuf[index++] = _calcCheckSum(data, lenTemp);
    sendBuf[index++] = NELLCOR_ETX;

    writeData(sendBuf, lenTemp + 4);
}

/*======================================================================*/
/*  Description: CRC Lookup Table                                       */
/*======================================================================*/
static unsigned char crcTable[256] =
{
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95,  1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93,  3, 128, 222, 60, 98,
    190, 224,  2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89,  7,
    219, 133, 103, 57, 186, 228,  6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135,  4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91,  5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73,  8, 86, 180, 234, 105, 55, 213, 139,
    87,  9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

/**************************************************************************************************
 * calculate check sum.
 *************************************************************************************************/
unsigned char NellcorSetProvider::_calcCheckSum(const unsigned char *data, int len)
{
    unsigned char crc = 0;
    for (unsigned char i = 0;  i < len;  i++)
    {
        crc = crcTable[(unsigned char)(crc ^ data[i])];
    }
    return crc;
}

void NellcorSetProvider::sendVersion()
{
    unsigned char data[2] = {NELLCOR_VER_INFO, 0x00};
    _sendCmd(data, 2);
}

void NellcorSetProvider::setWarnLimitValue(char spo2Low, char spo2High, short prLow, short prHigh)
{
    unsigned char data[8] = {0};
    data[0] = 'h';
    data[1] = 0x06;
    data[2] = spo2Low;
    data[3] = spo2High;
    data[4] = prLow >> 8;
    data[5] = prLow & 0xff;
    data[6] = prHigh >> 8;
    data[7] = prHigh & 0xff;

    _sendCmd(data, 8);
}

void NellcorSetProvider::getWarnLimitValue()
{
    unsigned char data[2] = {NELLCOR_ALARM_LIMIT, 0x00};
    _sendCmd(data, 2);
}


/**************************************************************************************************
 * 功能：构造函数
 * 参数：
 *
 *************************************************************************************************/
NellcorSetProvider::NellcorSetProvider() : Provider("NELLCOR_SPO2"), SPO2ProviderIFace(),
    _spo2ValueLimitStatus(0),
    _prValueLimitStatus(0),
    _isInterfere(false),
    _isLowPerfusionFlag(false),
    _volumeWarnPriority(0),
    _spo2Low(0),
    _spo2High(0),
    _prLow(0),
    _prHigh(0),
    _hostSensorKeyOne(0),
    _hostSensorKeyTwo(0),
    _hostSensorKeyThree(0),
    _hostSensorKeyFour(0),
    _versionInfo(""),
    _isreset(false),
    _satSeconds(0),
    _irPerModulValue(0),
    _waveUpdateType(WAVE_UPDATE_0),
    _waveSelectType(0),
    _isEnableCClock(false),
    _isEnableSensorEventRecord(false),
    _sensorEventStatus(0),
    _sensorEventType(0),
    _sensorType(0)
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    initPort(portAttr);
}

/**************************************************************************************************
 * 功能：析构函数
 * 参数：
 *
 *************************************************************************************************/
NellcorSetProvider::~NellcorSetProvider()
{
}

/**************************************************************************************************
 * 连接中断。
 *************************************************************************************************/
void NellcorSetProvider::disconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SPO2);
    if (alarmSource)
    {
        alarmSource->clear();
        alarmSource->setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    }
}

/**************************************************************************************************
 * 连接恢复。
 *************************************************************************************************/
void NellcorSetProvider::reconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SPO2);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    }
}

bool NellcorSetProvider::isResultSPO2PR(unsigned char *packet)
{
    if (packet[0] != NELLCOR__SPO2PR_VAL)
    {
        return false;
    }

    short temp;
    // 血氧值
    temp = packet[SPO2_VALUE];
    if (temp)
    {
        spo2Param.setSPO2(temp);
    }
    else
    {
        spo2Param.setSPO2(InvData());
    }

    // 脉率值
    if (packet[RATE_DISP_TYPE] & 0x01)
    {
        temp = packet[RATE_BPM_VALUE_TOP_EIGHT] + (0x01 << 9);
    }
    else
    {
        temp = packet[RATE_BPM_VALUE_TOP_EIGHT];
    }
    if (temp)
    {
        spo2Param.setPR(temp);
    }
    else
    {
        spo2Param.setPR(InvData());
    }

    return true;
}

bool NellcorSetProvider::isResult_BAR(unsigned char *packet)
{
    if (packet[0] != NELLCOR_WAVE_BLIP)
    {
        return false;
    }

    if (packet[1] != 0x02)
    {
        return false;
    }
    short temp;
    temp = packet[3];
    if (temp == 127)
    {
        spo2Param.addWaveformData(InvData());
        spo2Param.addBarData(InvData());
    }
    else
    {
        spo2Param.addWaveformData(128 - temp);
        spo2Param.addBarData(temp / 15);
    }

    temp = packet[2];

    spo2Param.setPulseAudio((temp & 0x80));

    // 低弱冠注
    if ((temp & 0x0f) < 3)
    {
        _isLowPerfusionFlag = true;
    }
    else
    {
        _isLowPerfusionFlag = false;
    }

    return true;
}

bool NellcorSetProvider::isStatus(unsigned char *packet)
{
    if (packet[0] != NELLCOR__SPO2PR_VAL)
    {
        return false;
    }
    short temp;
    // 传感器连接状态
    temp = packet[STATUS_AND_ALARM_WORD_TOP_EIGHT];
    bool isCableOff = ((temp >> 6) & 0x03) ? (false) : (true);
    spo2Param.setSensorOff(isCableOff);

    // 传感器搜索状态
    temp = packet[STATUS_AND_ALARM_WORD_TOP_EIGHT];
    unsigned char stat = (temp >> 4) & 0x03;
    bool isSearching = (stat > 0 && stat < 3) ? (true) : (false);

    if (isCableOff)  // 存在报警则不显示searching for pulse。
    {
        spo2Param.setNotify(true, trs("SPO2CheckSensor"));
        spo2Param.setValidStatus(false);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false);
    }
    else
    {
        spo2Param.setValidStatus(true);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, false);
        spo2Param.setSearchForPulse(isSearching);  // search pulse标志。
        if (!spo2Param.isNibpSameSide() || !nibpParam.isMeasuring())
        {
            if (isSearching)
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false);
            }
            else
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, _isLowPerfusionFlag);
            }
        }
    }
    return true;
}

void NellcorSetProvider::setSatSeconds(Spo2SatSecondsType type)
{
    unsigned char data[3] = {NELLCOR_SAT_SEC, 0x01, type};
    _sendCmd(data, sizeof(data));
}

void NellcorSetProvider::getSatSeconds(unsigned char *pack)
{
    if (pack[0] != NELLCOR_SAT_SEC_REPORT)
    {
        return;
    }
    unsigned char count = pack[1];
    if (count != 1)
    {
        return;
    }
    _satSeconds = pack[2];
}

void NellcorSetProvider::sendResetInfo()
{
    unsigned char data[2] = {NELLCOR_RESET_INFO, 0x00};
    _sendCmd(data, sizeof(data));
}


void NellcorSetProvider::resetNecllcorModule()
{
    unsigned char data[3] = {NELLCOR_RESET_INFO, 0x01, 0x0F};
    _sendCmd(data, sizeof(data));
}

void NellcorSetProvider::sendHostSensorKeyInfo()
{
    unsigned char data[2] = {NELLCOR_HOST_SENSOR_KEY_INFO, 0x00};
    _sendCmd(data, sizeof(data));
}

void NellcorSetProvider::setHostSensorKeyInfo(const unsigned char *keyInfo)
{
    if (!keyInfo)
    {
        return;
    }
    if (!&keyInfo[3])
    {
        return;
    }
    unsigned char data[6] = {NELLCOR_HOST_SENSOR_KEY_INFO, 0x04};
    memcpy(&data[2], keyInfo, 0x04);
    _sendCmd(data, sizeof(data));
}


void NellcorSetProvider::sendWaveUpdateInfo()
{
    unsigned char data[2] = {NELLCOR_WAVE_UPDATE_INFO, 0x00};
    _sendCmd(data, sizeof(data));
}

void NellcorSetProvider::sendWaveSelectInfo()
{
    unsigned char data[2] = {NELLCOR_WAVE_SELECT_INFO, 0x00};
    _sendCmd(data, sizeof(data));
}

void NellcorSetProvider::setWaveUpdateInfo(unsigned char infoIndex)
{
    unsigned char data[3] = {NELLCOR_WAVE_UPDATE_INFO, 0x01, 0x00};
    switch (infoIndex)
    {
    case WAVE_UPDATE_0:
        break;
    case WAVE_UPDATE_76:
        data[2] = 0x01;
        break;
    case WAVE_UPDATE_38:
        data[2] = 0x02;
        break;
    case WAVE_UPDATE_25:
        data[2] = 0x03;
        break;
    case WAVE_UPDATE_19:
        data[2] = 0x04;
        break;
    case WAVE_UPDATE_NR:
        break;
    }
    if (infoIndex > WAVE_UPDATE_0 && infoIndex < WAVE_UPDATE_NR)
    {
        _sendCmd(data, sizeof(data));
    }
}

void NellcorSetProvider::sendCClockInfo()
{
    unsigned char data[2] = {NELLCOR_C_CLOCK_INFO, 0x00};
    _sendCmd(data, sizeof(data));
}

void NellcorSetProvider::sendTriggerInfo()
{
    unsigned char data[2] = {NELLCOR_ECG_TRIGER_INFO, 0x00};
    _sendCmd(data, sizeof(data));
}

void NellcorSetProvider::disabledSatRateStatus()
{
    unsigned char data[3] = {NELLCOR_SAT_RATE_INFO, 0x01, 0x0f};
    _sendCmd(data, sizeof(data));
}

void NellcorSetProvider::setCClockInfo(bool isEnabled)
{
    unsigned char data[3] = {NELLCOR_C_CLOCK_INFO, 0x01, 0x00};
    if (isEnabled)
    {
        data[2] = 0xf0;
    }
    else
    {
        data[2] = 0x0f;
    }
    _sendCmd(data, sizeof(data));
}

void NellcorSetProvider::setWaveSelectInfo(unsigned char index)
{
    unsigned char data[3] = {NELLCOR_WAVE_SELECT_INFO, 0x01, 0x00};
    data[2] = index;
    _sendCmd(data, sizeof(data));
}


void NellcorSetProvider::_analyzeHostSeneorKeyInfo(unsigned char *pack)
{
    if (pack[0] != NELLCOR_HOST_SENSOR_KEY_INFO)
    {
        return;
    }

    unsigned char count = pack[1];
    if (count != 4)
    {
        return;
    }
    _hostSensorKeyOne = pack[2];
    _hostSensorKeyTwo = pack[3];
    _hostSensorKeyThree = pack[4];
    _hostSensorKeyFour = pack[5];
}

void NellcorSetProvider::_analyzeVersionInfo(unsigned char *pack)
{
    if (pack[0] != NELLCOR_VER_INFO)
    {
        return;
    }

    unsigned char count = pack[1];
    if (count > 64)
    {
        return;
    }
    _versionInfo.clear();
    for (unsigned char i = 0; i < count; i++)
    {
        _versionInfo.append(pack[i + 2]);
    }
}


void NellcorSetProvider::_analyzeIrPerModulValue(unsigned char *pack)
{
    if (pack[0] != NELLCOR_IR_PER_VALUE)
    {
        return;
    }

    unsigned char count = pack[1];
    if (count != 1)
    {
        return;
    }
    _irPerModulValue = pack[2];
}

void NellcorSetProvider::_analyzeWaveUpdateInfo(unsigned char *pack)
{
    if (pack[0] != NELLCOR_WAVE_UPDATE_INFO)
    {
        return;
    }

    unsigned char count = pack[1];
    if (count != 1)
    {
        return;
    }
    _waveUpdateType = static_cast<WaveUpdateType>(pack[2]);
}

void NellcorSetProvider::_analyzewaveSelectInfo(unsigned char *pack)
{
    if (pack[0] != NELLCOR_WAVE_SELECT_INFO)
    {
        return;
    }

    unsigned char count = pack[1];
    if (count != 1)
    {
        return;
    }
    _waveSelectType = pack[2];
}

void NellcorSetProvider::_analyzeCClockInfo(unsigned char *pack)
{
    if (pack[0] != NELLCOR_C_CLOCK_INFO)
    {
        return;
    }

    unsigned char count = pack[1];
    if (count != 1)
    {
        return;
    }
    if (pack[2] == 0xf0)
    {
        _isEnableCClock = true;
    }
    else if (pack[2] == 0x0f)
    {
        _isEnableCClock = false;
    }
}

void NellcorSetProvider::_analyzeSensorEventInfo(unsigned char *pack)
{
    switch (pack[0])
    {
    case 'K':  // 传感器启用、禁用信息
        if (pack[1] != 1)
        {
            break;
        }
        if (pack[2] == 0xf0)
        {
            _isEnableSensorEventRecord = true;
        }
        else
        {
            _isEnableSensorEventRecord = false;
        }
        break;
    case 'H':  // 传感器时间spo2限值信息
        if (pack[1] != 0x02)
        {
            break;
        }
        _spo2High = pack[2];
        _spo2Low = pack[3];
        break;
    case 'Y':  // 传感器事件报告
        if (pack[1] != 0x03)
        {
            break;
        }
        _sensorEventStatus = pack[2];
        _sensorEventType = pack[3];
        _sensorType = pack[4];
        break;
    case 'T':  // 传感器事件信息与时间信息
        if (pack[1] != 0x07)
        {
            break;
        }
        QDate date(pack[2], pack[3], pack[4]);
        QTime time(pack[5], pack[6], pack[7]);
        _dataTime.setDate(date);
        _dataTime.setTime(time);
        break;
    }
}

void NellcorSetProvider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    unsigned char dat[32] = {0};
    dat[0] = cmdId;
    dat[1] = len;
    for (unsigned int i = 0; i < 30 && i < len; i++)
    {
        dat[i + 2] = data[i];
    }
    _sendCmd(dat, (len + 2));
}

void NellcorSetProvider::handlePacket(unsigned char *data, int /*len*/)
{
    if (!isConnectedToParam)
    {
        return;
    }

    feed();
    short temp;
    switch (data[0])
    {
    case NELLCOR__SPO2PR_VAL:  // oxismart 报告 包含spo2、pr、状态等相关信息

        isResultSPO2PR(data);
        isStatus(data);

        // 脉率高低限警报
        temp = data[RATE_DISP_TYPE];
        if ((temp & 0x06) == 4)
        {
            _prValueLimitStatus = PR_VALUE_HIGH;
        }
        else if ((temp & 0x06) == 2)
        {
            _prValueLimitStatus = PR_VALUE_LOW;
        }
        else
        {
            _prValueLimitStatus = PR_VALUE_NORMAL;
        }

        // 血氧高低限警报
        temp = data[SAT_DISP_TYPE];
        if ((temp & 0x06) == 4)
        {
            _spo2ValueLimitStatus = SPO2_VALUE_HIGH;
        }
        else if ((temp & 0x06) == 2)
        {
            _spo2ValueLimitStatus = SPO2_VALUE_LOW;
        }
        else
        {
            _spo2ValueLimitStatus = SPO2_VALUE_NORMAL;
        }

        // 干扰状态获取
        temp = data[STATUS_AND_ALARM_WORD_TOP_EIGHT];
        if ((temp & 0xc0) == 0)
        {
            _isInterfere = false;
        }
        else
        {
            _isInterfere = true;
        }

        // 声音报警优先级状态获取
        temp = data[STATUS_AND_ALARM_WORD_DOWN_EIGHT];
        _volumeWarnPriority = temp & 0x03;

        break;
    case NELLCOR_WAVE_BLIP:  // 添加血氧波形数据 棒图数据 设置beep音标志
        isResult_BAR(data);
        break;
    case NELLCOR_VER_INFO:  // 获取版本信息
        _analyzeVersionInfo(data);
        break;
    case NELLCOR_ALARM_LIMIT:  // 获取报警限值
        _spo2Low = data[2];
        _spo2High = data[3];
        _prLow = data[4] << 8 | data[5];
        _prHigh = data[6] << 8 | data[7];
        break;
    case NELLCOR_SAT_SEC_REPORT:  // 高低限值容忍时间报告
        getSatSeconds(data);
        break;
    case NELLCOR_IR_PER_VALUE:  // IR百分比调制值
        _analyzeIrPerModulValue(data);
        break;
    case NELLCOR_WAVE_UPDATE_INFO:  // 波形更新
        _analyzeWaveUpdateInfo(data);
        break;
    case NELLCOR_WAVE_SELECT_INFO:  // 波形选择信息
        _analyzewaveSelectInfo(data);
        break;
    default:
        _analyzeSensorEventInfo(data);
        break;
    }
}
