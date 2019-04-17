/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/27
 **/

#include "MasimoProvider.h"
#include "SPO2Param.h"
#include "SPO2Alarm.h"
#include "Debug.h"
#include "NIBPParam.h"
#include "AlarmSourceManager.h"
#include "LanguageManager.h"

#define SOM             (0x02)
#define EOM             (0x03)

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool MasimoSetProvider::attachParam(Param &param)
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
void MasimoSetProvider::dataArrived(void)
{
    readData();
    if (ringBuff.dataSize() < _minPacketLen)
    {
        return;
    }

    unsigned char buff[64];
    while (ringBuff.dataSize() >= _minPacketLen)
    {
        if (ringBuff.at(0) != SOM)
        {
            // debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        if (ringBuff.dataSize() < 13)
        {
            // no enough data
            break;
        }

        if (ringBuff.at(13) != EOM)
        {
            // debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        // 将数据包读到buff中。
        for (int i = 0; i < _minPacketLen; i++)
        {
            buff[i] = ringBuff.at(0);
            ringBuff.pop(1);
        }

        unsigned char sum = _calcCheckSum(&buff[1], _minPacketLen - 3);
        if (buff[_minPacketLen - 2] == sum)
        {
            handlePacket(&buff[1], _minPacketLen - 3);
        }
        else
        {
            outHex(buff, _minPacketLen);
            debug("0x%02x", sum);
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

void MasimoSetProvider::dataArrived(unsigned char *buf, unsigned int length)
{
    _readData(buf, length);

    if (ringBuff.dataSize() < _minPacketLen)
    {
        return;
    }

    unsigned char buff[64];
    while (ringBuff.dataSize() >= _minPacketLen)
    {
        if (ringBuff.at(0) != SOM)
        {
            // debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        if (ringBuff.at(13) != EOM)
        {
            // debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        // 将数据包读到buff中。
        for (int i = 0; i < _minPacketLen; i++)
        {
            buff[i] = ringBuff.at(0);
            ringBuff.pop(1);
        }

        unsigned char sum = _calcCheckSum(&buff[1], _minPacketLen - 3);
        if (buff[_minPacketLen - 2] == sum)
        {
            handlePacket(&buff[1], _minPacketLen - 3);
        }
        else
        {
            outHex(buff, _minPacketLen);
            debug("0x%02x", sum);
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

/**************************************************************************************************
 * send to code.
 *************************************************************************************************/
void MasimoSetProvider::_sendCmd(const unsigned char *data, unsigned int len)
{
    int index = 0;
    unsigned char sendBuf[15] = {0};

    sendBuf[index++] = SOM;
    for (unsigned int i = 0; i < len; i++)
    {
        sendBuf[index++] = data[i];
    }

    sendBuf[index++] = _calcCheckSum(data, len);
    sendBuf[index++] = EOM;

    writeData(sendBuf, len + 3);
}

void MasimoSetProvider::_readData(unsigned char *buff, unsigned int len)
{
    ringBuff.push(buff, len);
}

/**************************************************************************************************
 * calculate check sum.
 *************************************************************************************************/
unsigned char MasimoSetProvider::_calcCheckSum(const unsigned char *data, int len)
{
    unsigned char sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum = sum + data[i];
    }
    sum = sum & 0xFF;
    return sum;
}

/**************************************************************************************************
 * 设置灵敏度和FastSat。
 *************************************************************************************************/
void MasimoSetProvider::setSensitivityFastSat(SensitivityMode mode, bool fastSat)
{
    unsigned char cmd[7] = {0};
    cmd[0] = 0x03;
    cmd[1] = 0x00;
    cmd[2] = 0x00;
    // bit0 for SensitivityMode, 1-normal, 0-max
    if (mode == SPO2_MASIMO_SENS_NORMAL)
    {
        cmd[2] |= 0x01;
    }

    // bit1 for Fast Sat, 1-on, 0-off.
    if (fastSat)
    {
        cmd[2] |= 0x02;
    }

    // bit2 for APOD, 1-on, 0-ignore.
    if (mode == SPO2_MASIMO_SENS_APOD)
    {
        cmd[2] |= 0x04;
    }
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = 0x00;
    cmd[6] = 0x00;

    _sendCmd(cmd, 7);
}

/**************************************************************************************************
 * 设置平均时间。
 *************************************************************************************************/
void MasimoSetProvider::setAverageTime(AverageTime mode)
{
    unsigned char cmd[7] = {0};
    cmd[0] = 0x02;
    cmd[1] = (unsigned char)mode;
    cmd[2] = 0x00;  // D2
    cmd[3] = 0x00;  // D3
    cmd[4] = 0x00;  // D4
    cmd[5] = 0x00;  // D5
    cmd[6] = 0x00;  // D6

    _sendCmd(cmd, 7);
}

/**************************************************************************************************
 * 设置SMart Tone使能选项。
 *************************************************************************************************/
void MasimoSetProvider::setSmartTone(bool enable)
{
    unsigned char cmd[7] = {0};
    cmd[0] = 0x01;
    cmd[1] = 0x18;
    cmd[2] = 0x38;
    if (enable)
    {
        cmd[3] = 0x50;
    }
    else
    {
        cmd[3] = 0x40;
    }
    cmd[4] = 0x00;
    cmd[5] = 0x00;
    cmd[6] = 0x00;

    _sendCmd(cmd, 7);
}

/**************************************************************************************************
 * 功能：构造函数
 * 参数：
 *
 *************************************************************************************************/
MasimoSetProvider::MasimoSetProvider() : Provider("MASIMO_SPO2"), SPO2ProviderIFace()
{
    disPatchInfo.packetType = DataDispatcher::PACKET_TYPE_SPO2;
    UartAttrDesc portAttr(9600, 8, 'N', 1);
    initPort(portAttr);

    _isLowPerfusionFlag = false;

    if (disPatchInfo.dispatcher)
    {
        // reset the hardware
        disPatchInfo.dispatcher->resetPacketPort(disPatchInfo.packetType);
    }
}

/**************************************************************************************************
 * 功能：析构函数
 * 参数：
 *
 *************************************************************************************************/
MasimoSetProvider::~MasimoSetProvider()
{
}

/**************************************************************************************************
 * 连接中断。
 *************************************************************************************************/
void MasimoSetProvider::disconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SPO2);
    if (alarmSource)
    {
        alarmSource->clear();
        alarmSource->setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    }
    spo2Param.setConnected(false);
}

/**************************************************************************************************
 * 连接恢复。
 *************************************************************************************************/
void MasimoSetProvider::reconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SPO2);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    }
    spo2Param.setConnected(true);
}

void MasimoSetProvider::handlePacket(unsigned char *data, int /*len*/)
{
    if (!isConnectedToParam)
    {
        return;
    }

    if (!isConnected)
    {
        spo2Param.setConnected(true);
    }

    feed();
    short temp;
    switch (data[0])
    {
    case 0x00:          // 解包获取SpO2参数值，定义状态标记。
        temp = (data[1] << 8) + data[2];
        temp = ((temp % 10) < 5) ? (temp / 10) : (temp / 10 + 1);
        if (_isLowPerfusionFlag)
        {
            spo2Param.setSPO2(InvData());
        }
        else
        {
            spo2Param.setSPO2((temp == 0) ? InvData() : temp);
        }
        break;

    case 0x01:          // 解包获取PR参数值，定义状态标记。
        temp = data[2];
        if (_isLowPerfusionFlag)
        {
            spo2Param.setPR(InvData());
        }
        else
        {
            spo2Param.setPR((temp == 0) ? InvData() : temp);
        }
        break;

    case 0x02:          // 解包获取PI参数值，定义状态标记
    {
        temp = (data[1] << 8) + data[2];
        float v = temp;

        short piValue = InvData();
        if (v > 0 && v < 20000)
        {
            v = (v / 1000 + 0.05);
            piValue = static_cast<short>(v * 10);
        }

        spo2Param.updatePIValue(piValue);     // 更新PI值。
        break;
    }

    case 0x07:          // 获取传感器探头状态
    {
        temp = (data[1] << 8) + data[2];
        // 总的探头脱落
        bool isCableOff = (temp & 0xF3F3) ? true : false;
        spo2Param.setSensorOff(isCableOff);

        // 搜索脉搏
        bool isSearching = (temp & 0x08) ? true : false;

        // 低弱冠注
        _isLowPerfusionFlag = (temp & 0x04) ? true : false;

        // Low Signal IQ。
//        bool isLowSignalIQFlag = (temp & 0x400) ? true : false;

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

        if (_isLowPerfusionFlag)
        {
            spo2Param.setSPO2(InvData());
            spo2Param.setPR(InvData());
        }
    }
    break;

    case 61:
//        temp = (data[1] << 8) + data[2];
//        spo2Param.diagnosticError(temp);
        break;

    case 62:
//        temp = (data[1] << 8) + data[2];
//        spo2Param.boardError(temp);
        break;

    case 15:
        break;
    case 16:
        break;
    case 17:
        break;
    case 18:
        break;
    case 19:
        break;

    default:
        break;
    }

    // 接收PLE波形。
    char pleWaveformValue = 0;
    pleWaveformValue = data[3];
    short wave = 100 - (unsigned char)(pleWaveformValue + 128) / 3;
    spo2Param.addWaveformData(wave);
//    spo2Param.addWaveformData(128 - pleWaveformValue);

    // 棒图，使用PLETH为原始数据。
    spo2Param.addBarData(wave * 15 / 255);

    // BEEP音。
    if ((data[4] != 0) && !(data[4] & 0x80))
    {
        spo2Param.setPulseAudio(true);
    }

    // 接收SIQ波形。
//    unsigned char siqWaveformValue = 0;
//    siqWaveformValue = data[5];
//    spo2Param.updataSiqWaveformValue((unsigned char)siqWaveformValue);
}
