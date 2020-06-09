/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/26
 **/

#include "Debug.h"
#include "WitleafProvider.h"
#include "IBPParam.h"
#include "COParam.h"
#include "AlarmSourceManager.h"

#define     INVALID     0xff9d

enum  // 位操作。
{
    BIT0 = 0x01,
    BIT1 = 0x01 << 1,
    BIT2 = 0x01 << 2,
    BIT3 = 0x01 << 3,
    BIT4 = 0x01 << 4,
    BIT5 = 0x01 << 5,
    BIT6 = 0x01 << 6,
    BIT7 = 0x01 << 7,
    BIT_ALL = 0xFF,
};

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool WitleafProvider::attachParam(Param *param)
{
    const QString &name = param->getName();
    if (name == paramInfo.getParamName(PARAM_IBP))
    {
        ibpParam.setProvider(this);
        Provider::attachParam(param);
    }
    else if (name == paramInfo.getParamName(PARAM_CO))
    {
        coParam.setProvider(this);
        Provider::attachParam(param);
    }

    return true;
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void WitleafProvider::dataArrived()
{
    readData();     // 读取数据到RingBuff中

    unsigned char buff[570];

    while (ringBuff.dataSize() >= _minPacketLen)
    {
        if (ringBuff.at(0) != _STX)
        {
            // debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        int len = ringBuff.at(1);
        if ((len <= 0) || (len > 570))
        {
            ringBuff.pop(1);
            break;
        }

        if (len > ringBuff.dataSize())  // 数据还不够，继续等待。
        {
            break;
        }

        // 数据包不会超过packet长度，当出现这种情况说明发生了不可预料的错误，直接丢弃该段数据。
        if (len > static_cast<int>(sizeof(buff)))
        {
            ringBuff.pop(1);
            continue;
        }

        // 将数据包读到buff中。
        for (int i = 0; i < len; i++)
        {
            buff[i] = ringBuff.at(0);
            ringBuff.pop(1);
        }

        if (_checkPacketValid(buff, len))
        {
            handlePacket(&buff[2], len - 3);
        }
        else
        {
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

/**************************************************************************************************
 * 协议命令解析
 *************************************************************************************************/
void WitleafProvider::handlePacket(unsigned char *data, int len)
{
    if (!isConnectedToParam)
    {
        return;
    }

    if (!isConnected)
    {
        coParam.setConnected(true);
        ibpParam.setConnected(true);
    }

    // 发送保活帧, 有数据时调用清除连接计数器。
    feed();
    if (data[0] == PARAM_TYPE_IBP)
    {
        switch (data[2])
        {
        case IBP_RSP_INFO:
            break;
        case IBP_RSP_MODULE_INFO:
        {
            _status.softwaveVersion = data[3] * 100 + data[4] * 10 + data[5];
            _status.arithmeticVersion = data[6] * 100 + data[7] * 10 + data[8];
            _status.protocolVersion = data[9] * 100 + data[10] * 10 + data[11];
            break;
        }
        case IBP_RSP_SELF_CHECK:
        {
            unsigned char selfResult = data[3];
            _status.romStatus = (selfResult & BIT0) ? true : false;
            _status.ramStatus = (selfResult & BIT1) ? true : false;
            _status.cpuStatus = (selfResult & BIT2) ? true : false;
            _status.adStatus  = (selfResult & BIT3) ? true : false;
            _status.wdStatus  = (selfResult & BIT4) ? true : false;
            break;
        }
        case IBP_RSP_ZERO_INFO:
        {
            if (data[3] == IBP_CHN_1)
            {
                if (data[4] == IBP_CALIBRATION_ZERO)
                {
                    _status.ibp1ZeroCalibration.timeSecond = data[7];
                    _status.ibp1ZeroCalibration.timeMinute = data[8];
                    _status.ibp1ZeroCalibration.timeHour   = data[9];
                    _status.ibp1ZeroCalibration.timeDay    = data[10];
                    _status.ibp1ZeroCalibration.timeMouth  = data[11];
                    _status.ibp1ZeroCalibration.timeYear   = data[12];
                }
                else
                {
                    _status.ibp1Calibration.timeSecond = data[7];
                    _status.ibp1Calibration.timeMinute = data[8];
                    _status.ibp1Calibration.timeHour   = data[9];
                    _status.ibp1Calibration.timeDay    = data[10];
                    _status.ibp1Calibration.timeMouth  = data[11];
                    _status.ibp1Calibration.timeYear   = data[12];
                }
            }
            else
            {
                if (data[4] == IBP_CALIBRATION_ZERO)
                {
                    _status.ibp2ZeroCalibration.timeSecond = data[7];
                    _status.ibp2ZeroCalibration.timeMinute = data[8];
                    _status.ibp2ZeroCalibration.timeHour   = data[9];
                    _status.ibp2ZeroCalibration.timeDay    = data[10];
                    _status.ibp2ZeroCalibration.timeMouth  = data[11];
                    _status.ibp2ZeroCalibration.timeYear   = data[12];
                }
                else
                {
                    _status.ibp2Calibration.timeSecond = data[7];
                    _status.ibp2Calibration.timeMinute = data[8];
                    _status.ibp2Calibration.timeHour   = data[9];
                    _status.ibp2Calibration.timeDay    = data[10];
                    _status.ibp2Calibration.timeMouth  = data[11];
                    _status.ibp2Calibration.timeYear   = data[12];
                }
            }
            break;
        }
        case IBP_RSP_WAVE_DATA:
        {
            unsigned short waveValue = data[4];
            waveValue <<= 8;
            waveValue |= data[5];
            bool invalid = false;
            if (waveValue == 0xFFFF)
            {
                invalid = true;
                waveValue = 0;
            }
            qint16 waveValueMmhg = (waveValue - 1000);
            ibpParam.addWaveformData(waveValueMmhg, invalid, IBP_CHN_1);

            waveValue = data[6];
            waveValue <<= 8;
            waveValue |= data[7];
            invalid = false;
            if (waveValue == 0xFFFF)
            {
                invalid = true;
                waveValue = 0;
            }
            waveValueMmhg = (waveValue - 1000);
            ibpParam.addWaveformData(waveValueMmhg, invalid, IBP_CHN_2);
            break;
        }
        case IBP_RSP_RESULT_DATA:
        {
            short sys = ((data[4] << 8) | data[5]) == INVALID ? InvData() : ((data[4] << 8) | data[5]) - 100;
            short dia = ((data[6] << 8) | data[7]) == INVALID ? InvData() : ((data[6] << 8) | data[7]) - 100;
            short mean = ((data[8] << 8) | data[9]) == INVALID ? InvData() : ((data[8] << 8) | data[9]) - 100;
            short pr = ((data[10] << 8) | data[11]) == INVALID ? InvData() : ((data[10] << 8) | data[11]) - 100;
            ibpParam.setRealTimeData(sys, dia, mean, pr, IBP_CHN_1);
            sys = ((data[12] << 8) | data[13]) == INVALID ? InvData() : ((data[12] << 8) | data[13]) - 100;
            dia = ((data[14] << 8) | data[15]) == INVALID ? InvData() : ((data[14] << 8) | data[15]) - 100;
            mean = ((data[16] << 8) | data[17]) == INVALID ? InvData() : ((data[16] << 8) | data[17]) - 100;
            pr = ((data[18] << 8) | data[19]) == INVALID ? InvData() : ((data[18] << 8) | data[19]) - 100;
            ibpParam.setRealTimeData(sys, dia, mean, pr, IBP_CHN_2);
            break;
        }
        case IBP_RSP_CYCLE_REPORT:
        {
            unsigned char circleReport = data[3];
            bool chn1LeadStatus = (circleReport & BIT0) ? true : false;
            bool chn2LeadStatus = (circleReport & BIT1) ? true : false;
            _status.tbLeadStatus   = (circleReport & BIT2) ? true : false;
            _status.tiLeadStatus   = (circleReport & BIT3) ? true : false;
            _status.modulePowerStatus = (circleReport & BIT4) ? true : false;
            if (_status.ibp1LeadStatus != chn1LeadStatus)
            {
                _status.ibp1LeadStatus = chn1LeadStatus;
                ibpParam.setLeadStatus(IBP_CHN_1, _status.ibp1LeadStatus);
            }

            if (_status.ibp2LeadStatus != chn2LeadStatus)
            {
                _status.ibp2LeadStatus = chn2LeadStatus;
                ibpParam.setLeadStatus(IBP_CHN_2, _status.ibp2LeadStatus);
            }
            break;
        }
        case IBP_RSP_ZERO_RESULT:
        {
            IBPCalibration calib = (IBPCalibration)((data[3] >> 7) & 0x01);
            IBPChannel IBP = (IBPChannel)((data[3] >> 6) & 0x01);
            int info = 0;
            if (calib == IBP_CALIBRATION_ZERO)
            {
                info = data[3] & 0x07;
            }
            else if (calib == IBP_CALIBRATION_SET)
            {
                info = (data[3] >> 3) & 0x07;
            }
            ibpParam.setCalibrationInfo(calib, IBP, info);
            break;
        }
        default:
        {
//            outHex(data, len);
            break;
        }
        }
    }
    else if (data[0] == PARAM_TYPE_CO)
    {
        switch (data[2])
        {
        case CO_RSP_TBTI_DATA:
        {
            unsigned short tbData = data[3];
            tbData <<= 8;
            tbData |= data[4];

            coParam.setTb(tbData);
            break;
        }
        case CO_RSP_MEASURE_RESULT:
        {
            unsigned short coData = data[3];
            coData <<= 8;
            coData |= data[4];

            unsigned short ciData = data[5];
            ciData <<= 8;
            ciData |= data[6];

            coParam.setMeasureResult(coData, ciData);
            break;
        }
        case CO_RSP_HEMODYMIC_RESULT:
        {
            break;
        }
        case CO_RSP_MEASURE_INFO:
        {
            unsigned char coMeasureInfo = data[3];
            _status.coMeasureStatus = (coMeasureInfo & BIT0) ? true : false;
            _status.coMeasureParam = (coMeasureInfo & BIT1) ? true : false;
            _status.coHemodynamicParam = (coMeasureInfo & BIT2) ? true : false;
            _status.coMeasureInterval = (coMeasureInfo & BIT3) ? true : false;
            _status.coTBLeadoff = (coMeasureInfo & BIT4) ? true : false;
            _status.coTILeadoff = (coMeasureInfo & BIT5) ? true : false;
            break;
        }
        default:
        {
            outHex(data, len);
            break;
        }
        }
    }
}

/***************************************************************************************************
 * 发送协议命令
 * 参数:
 *      len:        命令参数长度，单位:字节。若data指针为空，则len为0
 *      paramType:  用于标识不同的参数模块的数据包,IBP 为 0x07,CO 为 0x08
 *      dataType:   用于标识不同的数据包类型,包括四种类型:DC(0x01)、DR(0x02)、DA(0x03)和DD(0x04)
 *      dataId:     标识数据包的 ID,区分不同命令或数据
 *      data:       具体的格式和长度取决于数据包 ID
 *      dataSN:     不是所有帧都需要,有的数据包存在序列号,有的数据包不存在序列号
 **************************************************************************************************/
void WitleafProvider::sendCmd(unsigned char len, unsigned char paramType,
                              unsigned char dataType, unsigned char dataId,
                              const unsigned char *data, const unsigned char *dataSN)
{
    unsigned int cmdLen;
    if (NULL != dataSN)
    {
        cmdLen = len + _minPacketLen + 4;
    }
    else
    {
        cmdLen = len + _minPacketLen;
    }
    unsigned char cmdBuf[_maxPacketLen] = {0};
    if (cmdLen > _maxPacketLen)
    {
        debug("Comand too long!\n");
        return;
    }

    cmdBuf[0] = _STX;
    cmdBuf[1] = (cmdLen & 0xff);
    cmdBuf[2] = (paramType & 0xff);
    cmdBuf[3] = (dataType & 0xff);
    cmdBuf[4] = (dataId & 0xff);
    if ((NULL != data) && (0 != len))
    {
        qMemCopy(cmdBuf + 5, data, len);
    }
    if (NULL != dataSN)
    {
        qMemCopy(cmdBuf + 5 + len, dataSN, 4);
    }
    cmdBuf[cmdLen - 1] = calcCheckSum(cmdBuf, cmdLen);
    writeData(cmdBuf, cmdLen);
//    outHex(cmdBuf, cmdLen);
}

/**************************************************************************************************
 * 计算发送数据的校验和。
 *************************************************************************************************/
unsigned char WitleafProvider::calcCheckSum(const unsigned char *data, unsigned int len)
{
    char sum = 0;
    for (unsigned int i = 0; i < len - 1; i ++)
    {
        sum += data[i] & 0xff;
    }

    return sum;
}

void WitleafProvider::disconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_IBP);
    if (alarmSource)
    {
        alarmSource->clear();
        alarmSource->setOneShotAlarm(IBP_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    }
    coParam.setConnected(false);
    ibpParam.setConnected(false);
}

void WitleafProvider::reconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_IBP);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(IBP_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    }
    coParam.setConnected(true);
    ibpParam.setConnected(true);
}

/**************************************************************************************************
 * 模块信息查询
 *************************************************************************************************/
void WitleafProvider::moduleInfo()
{
    unsigned char data[1] = {0x00};
    sendCmd(1, PARAM_TYPE_CO, IBP_DATA_DC, CO_CMD_MODULE_INFO, data, NULL);
}

/**************************************************************************************************
 * 模块自检结果查询
 *************************************************************************************************/
void WitleafProvider::moduleSelfCheck()
{
    sendCmd(0, PARAM_TYPE_CO, IBP_DATA_DC, CO_CMD_SELF_CHECK, NULL, NULL);
}

/**************************************************************************************************
 * IBP 平均时间设置
 *      字节1：0x00(IBP1), 0x01(IBP2);
 *      字节2：0x01~0x12:实际时间值,单位为秒
 *************************************************************************************************/
void WitleafProvider::setAvergTime(IBPChannel IBP, unsigned char time)
{
    unsigned char data[2] = {IBP, time};
    sendCmd(2, PARAM_TYPE_IBP, IBP_DATA_DC, IBP_CMD_SET_AVERG_TIME, data, NULL);
}

/**************************************************************************************************
 * IBP校准、校零设置
 *************************************************************************************************/
void WitleafProvider::setZero(IBPChannel IBP, IBPCalibration calibration, unsigned short pressure)
{
    unsigned char data[4] = {IBP, calibration,
                             (unsigned char)((pressure >> 8) & 0xff),
                             (unsigned char)(pressure & 0xff)};
    sendCmd(4, PARAM_TYPE_IBP, IBP_DATA_DC, IBP_CMD_SET_ZERO, data, NULL);
}

/**************************************************************************************************
 * IBP滤波设置
 *************************************************************************************************/
void WitleafProvider::setFilter(IBPChannel IBP, IBPFilterMode filter)
{
    unsigned char data[2] = {IBP, filter};
    sendCmd(2, PARAM_TYPE_IBP, IBP_DATA_DC, IBP_CMD_SET_FILTER, data, NULL);
}

/**************************************************************************************************
 * IBP表明设置
 *************************************************************************************************/
void WitleafProvider::setIndicate(IBPLabel pressurenameIBP1, IBPLabel pressurenameIBP2,
                                  IBPMeasueType auxiliarysetIBP1, IBPMeasueType auxiliarysetIBP2)
{
    unsigned char indicate = 0;
    indicate = (pressurenameIBP1 & 0x07) | ((pressurenameIBP2 & 0x07) << 3) |
               ((auxiliarysetIBP1 & 0x01) << 6) | ((auxiliarysetIBP2 & 0x01) << 7);
    unsigned char data[3] = {0x00, indicate, 0x00};
    sendCmd(3, PARAM_TYPE_IBP, IBP_DATA_DC, IBP_CMD_SET_INDICATE, data, NULL);
}

/**************************************************************************************************
 * CO 测量控制
 *************************************************************************************************/
void WitleafProvider::measureCtrl(COMeasureCtrl instctl)
{
    unsigned char data[1] = {instctl};
    sendCmd(1, PARAM_TYPE_CO, IBP_DATA_DC, CO_CMD_INST_CTL, data, NULL);
}

/**************************************************************************************************
 * CO 测量时间间隔设置
 *************************************************************************************************/
void WitleafProvider::setMeasureInterval(COMeasureInterval interval)
{
    unsigned char data[1] = {interval};
    sendCmd(1, PARAM_TYPE_CO, IBP_DATA_DC, CO_CMD_SET_INTERVAL, data, NULL);
}

/**************************************************************************************************
 * Ti 输入模式设置
 *      模块支持的水温设定范围为 0.0-27.0 摄氏度,下发范围为0~270.(放大十倍后下发数据)
 *************************************************************************************************/
void WitleafProvider::setTiSource(COTiSource inputmode, unsigned short watertemp)
{
    unsigned char data[3] = {inputmode, (unsigned char)((watertemp >> 8) & 0xff), (unsigned char)(watertemp & 0xff)};
    sendCmd(3, PARAM_TYPE_CO, IBP_DATA_DC, CO_CMD_SET_INPUT_MODULE, data, NULL);
}

/**************************************************************************************************
 * 注射液体积设定
 *      模块上电后默认为设定注射液体积是 10ml,模块支持的注射液体积为 1-200ml.
 *************************************************************************************************/
void WitleafProvider::setInjectionVolume(unsigned char volume)
{
    unsigned char data[1] = {volume};
    sendCmd(1, PARAM_TYPE_CO, IBP_DATA_DC, CO_CMD_SET_VOLUME, data, NULL);
}

/**************************************************************************************************
 * 漂浮导管系数设定
 *
 *************************************************************************************************/
void WitleafProvider::setCatheterCoeff(unsigned short ratio)
{
    unsigned char data[2] = {(unsigned char)(ratio >> 8 & 0xff), (unsigned char)(ratio & 0xff)};
    sendCmd(2, PARAM_TYPE_CO, IBP_DATA_DC, CO_CMD_SET_DUCT_RATIO, data, NULL);
}

/**************************************************************************************************
 * IBP 校零/校准时间设定
 *************************************************************************************************/
void WitleafProvider::setTimeZero(IBPChannel IBP, IBPCalibration calibration,
                                  unsigned char second, unsigned char minute,
                                  unsigned char hour, unsigned char day,
                                  unsigned char month, unsigned char year)
{
    unsigned char onechar = (second & 0x3f) | ((calibration & 0x01) << 6) | ((IBP & 0x01) << 7);
    unsigned char data[6] = {onechar, minute, hour, day, month, year};
    sendCmd(6, PARAM_TYPE_IBP, IBP_DATA_DC, IBP_CMD_SET_TIME_ZERO, data, NULL);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
WitleafProvider::WitleafProvider(): Provider("WITLEAF_IBP"), IBPProviderIFace(),
    _status(WitleafProviderStatus())
{
    UartAttrDesc portAttr(115200, 8, 'O', 1);
    initPort(portAttr);

    // init ibp lead status
    _status.ibp1LeadStatus = true;
    _status.ibp2LeadStatus = true;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
WitleafProvider::~WitleafProvider()
{
}

/***************************************************************************************************
 * 协议数据校验
 * 返回:
 *   true - 校验成功; false - 校验失败。
 **************************************************************************************************/
bool WitleafProvider::_checkPacketValid(const unsigned char *data, unsigned int len)
{
    unsigned char sum = 0;
    for (unsigned int i = 0; i < len - 1; i ++)
    {
        sum += data[i] & 0xff;
    }
    if (sum != data[len - 1])
    {
        return false;
    }
    return true;
}
