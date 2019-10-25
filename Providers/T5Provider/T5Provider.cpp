/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/5
 **/

#include "T5Provider.h"
#include "TEMPParam.h"
#include "Debug.h"
#include <QString>
#include "crc8.h"
#include "SystemManager.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include "RawDataCollector.h"
#include "IConfig.h"
#include "AlarmSourceManager.h"

#define LOW_BORDER_VALUE 0
#define HIGH_BORDER_VALUE 501
#define TEMP_INVALID_VALUE      (-1)    // 体温无效值

enum TempErrorCode
{
    ERRORCODE_DATA_RESET_DEFAULT = 1,
    ERRORCODE_FLASH_WRONG,
    ERRORCODE_SELF_CHECK_FAILED,
    ERRORCODE_CHANNEL1_AD_OVERRANGE,
    ERRORCODE_CHANNEL2_AD_OVERRANGE,
    ERRORCODE_CHANNEL1_NOT_CALIBRATION,
    ERRORCODE_CHANNEL2_NOT_CALIBRATION
};

static const char *tempErrorCode[] =
{
    "Unknown mistake.\r\n",
    "The data saved in Flash is reset to the default value.\r\n",
    "Flash wrong.\r\n",
    "AD Sampling failed.\r\n",
    "Check channel 1 of AD Sampling is overrange.\r\n",
    "Check channel 2 of AD Sampling is overrange.\r\n",
    "Temp module channel 1 not calibration\n",
    "Temp module channel 2 not calibration\n",
};

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool T5Provider::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_TEMP)
    {
        tempParam.setProvider(this);
        Provider::attachParam(param);
        return true;
    }
    return false;
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void T5Provider::handlePacket(unsigned char *data, int len)
{
    if (!isConnectedToParam)
    {
        return;
    }

    BLMProvider::handlePacket(data, len);

    switch (data[0])
    {
    // 自检状态
    case T5_RSP_SELF_STATE:
        _selfTest(data, len);
        break;

    // 获取测量状态
    case T5_RSP_PROBE_STATE:
        _probeState(data);
        break;

    // 通道校准
    case T5_RSP_CHANNEL:
        tempParam.getCalibrateData(data);
        break;

    // 启动帧。
    case T5_NOTIFY_START_UP:
    {
        _sendACK(data[0]);

        ErrorLogItem *item = new ErrorLogItem();
        item->setName("T5 Start");
        errorLog.append(item);

        tempParam.reset();
        break;
    }

    // 测量数据
    case T5_NOTIFY_DATA:
        _sendACK(data[0]);
        _limitHandle(data);     // 处理超界
        _result(data);
        break;

    // 测量超界帧
    case T5_NOTIFY_OVERRANGE:
        _sendACK(data[0]);
        break;

    // 探头脱落帧
    case T5_NOTIFY_PROBE_OFF:
        _sendACK(data[0]);
        _sensorOff(data);
        break;

    case T5_CYCLE_ALIVE:
        feed();
        break;

    case T5_CYCLE_DATA:
        rawDataCollector.collectData(RawDataCollector::TEMP_DATA, data + 1, len - 1);
        break;

    case T5_OHM_DATA:
         ohmResult(data);
         break;

    case T5_DATA_ERROR:
        _sendACK(data[0]);
        _errorWarm(data, len);
        break;


    default:
        break;
    }
}

/**************************************************************************************************
 * 与模块的通信中断。
 *************************************************************************************************/
void T5Provider::disconnected(void)
{
    _disconnected = true;
    _shotAlarm();
    tempParam.setOneShotAlarm(TEMP_ONESHOT_ALARM_COMMUNICATION_STOP, true);
}

/**************************************************************************************************
 * 与模块的通信恢复。
 *************************************************************************************************/
void T5Provider::reconnected(void)
{
    _disconnected = false;
    tempParam.setOneShotAlarm(TEMP_ONESHOT_ALARM_COMMUNICATION_STOP, false);
}

void T5Provider::sendDisconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_TEMP);
    if (alarmSource && !alarmSource->isAlarmed(TEMP_ONESHOT_ALARM_COMMUNICATION_STOP))
    {
        alarmSource->setOneShotAlarm(TEMP_ONESHOT_ALARM_SEND_COMMUNICATION_STOP, true);
    }
}

/**************************************************************************************************
 * ACK应答。
 *************************************************************************************************/
void T5Provider::_sendACK(unsigned char type)
{
    unsigned char data = type & 0xFF;
    sendCmd(T5_RESPONSE_ACK, &data, 1);
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void T5Provider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    sendCmd(cmdId, data, len);
}

/**************************************************************************************************
 * 获取版本号
 *************************************************************************************************/
void T5Provider::sendVersion()
{
    sendCmd(T5_CMD_GET_VERSION, NULL, 0);
}

/**************************************************************************************************
 * 发送自检指令。
 *************************************************************************************************/
void T5Provider::sendTEMPSelfTest()
{
    sendCmd(T5_CMD_SELF_STATE, NULL, 0);
}

/**************************************************************************************************
 * 获取测量状态
 *************************************************************************************************/
void T5Provider::sendProbeState()
{
    sendCmd(T5_CMD_PROBE_STATE, NULL, 0);
}

void T5Provider::sendCalibrateData(int channel, int value)
{
    unsigned char cmd[2];

    cmd[0] = channel & 0xFF;
    cmd[1] = value & 0xFF;

    sendCmd(T5_CMD_CHANNEL, cmd, 2);
}

void T5Provider::enterCalibrateState()
{
    unsigned char cmd[1];
    cmd[0] = 0x01;
    sendCmd(T5_CMD_CALIBRATE_STATE, cmd, 1);
}

void T5Provider::exitCalibrateState()
{
    unsigned char cmd[1];
    cmd[0] = 0x00;
    sendCmd(T5_CMD_CALIBRATE_STATE, cmd, 1);
}

/**************************************************************************************************
 * 接收自检状态。
 *************************************************************************************************/
void T5Provider::_selfTest(unsigned char *packet, int len)
{
    int num = packet[1];
    if (num > 0)
    {
        QString errorStr("");
        errorStr = "error code = ";
        for (int i = 1; i < len; i++)
        {
            errorStr += QString("0x%1, ").arg(QString::number(packet[i], 16));
        }
        errorStr += "\r\n";

        for (int i = 2; i < len; i++)
        {
            switch (packet[i])
            {
            case ERRORCODE_CHANNEL1_NOT_CALIBRATION:
                tempParam.setOneShotAlarm(TEMP_ONESHOT_ALARM_NOT_CALIBRATION_1, true);
                errorStr += tempErrorCode[packet[i]];
                break;
            case ERRORCODE_CHANNEL2_NOT_CALIBRATION:
                tempParam.setOneShotAlarm(TEMP_ONESHOT_ALARM_NOT_CALIBRATION_2, true);
                errorStr += tempErrorCode[packet[i]];
                break;
            case ERRORCODE_DATA_RESET_DEFAULT:
                tempParam.setOneShotAlarm(TEMP_ONESHOT_ALARM_MODULE_ABNORMAL, true);
                errorStr += tempErrorCode[packet[i]];
                break;
            default:
                errorStr += tempErrorCode[0];
                break;
            }
        }

        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName("T5 Selftest Error");
        item->setLog(errorStr);
        item->setSubSystem(ErrorLogItem::SUB_SYS_T5);
        item->setSystemState(ErrorLogItem::SYS_STAT_SELFTEST);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        errorLog.append(item);

        _shotAlarm();
    }
    else if (num == 0)
    {
        systemManager.setPoweronTestResult(T5_MODULE_SELFTEST_RESULT, SELFTEST_SUCCESS);
    }
}

void T5Provider::_errorWarm(unsigned char *packet, int len)
{
    QString errorStr("");
    errorStr = "error code = ";
    for (int i = 1; i < len; i++)
    {
        errorStr += QString("0x%1, ").arg(QString::number(packet[i]));
    }
    errorStr += "\r\n";

    for (int i = 1; i < len; i++)
    {
        switch (packet[i])
        {
        case ERRORCODE_FLASH_WRONG:
            tempParam.setOneShotAlarm(TEMP_ONESHOT_ALARM_MODULE_ABNORMAL, true);
            break;
        case ERRORCODE_SELF_CHECK_FAILED:
            systemManager.setPoweronTestResult(T5_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
            tempParam.setErrorDisable();
            tempParam.setWidgetErrorShow(true);
            tempParam.setOneShotAlarm(TEMP_ONESHOT_ALARM_MODULE_DISABLE, true);
            errorStr += tempErrorCode[packet[i]];
            break;
        default:
            errorStr += tempErrorCode[0];
            break;
        }
    }

    ErrorLogItem *item = new CriticalFaultLogItem();
    item->setName("T5 Error");
    item->setLog(errorStr);
    item->setSubSystem(ErrorLogItem::SUB_SYS_T5);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
    errorLog.append(item);

    _shotAlarm();
}

/**************************************************************************************************
 * 接收测量状态。
 *************************************************************************************************/
void T5Provider::_probeState(unsigned char *packet)
{
    if (packet[1] & 0x01)
    {
        _overRang1 = true;
    }
    else
    {
        _overRang1 = false;
    }
    if (packet[1] & 0x10)
    {
        _overRang1 = false;
    }
    else
    {
        _sensorOff1 = false;
    }

    if (packet[2] & 0x01)
    {
        _overRang2 = true;
    }
    else
    {
        _overRang2 = false;
    }
    if (packet[2] & 0x10)
    {
        _overRang2 = false;
    }
    else
    {
        _sensorOff2 = false;
    }

    _shotAlarm();
}

/**************************************************************************************************
 * 体温值。
 *************************************************************************************************/
void T5Provider::_result(unsigned char *packet)
{
    _temp1 = InvData();
    _temp2 = InvData();
    _tempd = InvData();
    short temp = InvData();
    if (!_overRang1)
    {
        temp = static_cast<short>((packet[2] << 8) +packet[1]);
        if (temp == TEMP_INVALID_VALUE)
        {
            temp = InvData();
        }
        else if (!_hasBeenConnected1)
        {
            _hasBeenConnected1 = true;
        }
        _temp1 = borderValueChange(LOW_BORDER_VALUE, HIGH_BORDER_VALUE, temp);
    }

    if (!_overRang2)
    {
        temp = static_cast<short>((packet[4] << 8) + packet[3]);
        if (temp == TEMP_INVALID_VALUE)
        {
            temp = InvData();
        }
        else if (!_hasBeenConnected2)
        {
            _hasBeenConnected2 = true;
        }
        _temp2 = borderValueChange(LOW_BORDER_VALUE, HIGH_BORDER_VALUE, temp);
    }

    if (_temp1 >= 0 && _temp1 <= 500 && _temp2 >= 0 && _temp2 <= 500)
    {
        _tempd = abs(_temp1 - _temp2);
    }
    else
    {
        _tempd = InvData();
    }

    tempParam.setTEMP(_temp1, _temp2, _tempd);
}

void T5Provider::ohmResult(unsigned char *packet)
{
    ohm1 = InvData();
    ohm2 = InvData();
    if (!(0xFF == packet[2] && 0xFF == packet[1]))
    {
        ohm1 = static_cast<int>((packet[2] << 8) + packet[1]);
    }
    if (!(0xFF == packet[4] && 0xFF == packet[3]))
    {
        ohm2 = static_cast<int>((packet[4] << 8) + packet[3]);
    }
    tempParam.setOhm(ohm1, ohm2);
}

/**************************************************************************************************
 * 探头脱落。
 *************************************************************************************************/
void T5Provider::_sensorOff(unsigned char *packet)
{
    if (!_hasBeenConnected1 && !(packet[1] & 0x01))
    {
        _hasBeenConnected1 = true;
    }

    if (!_hasBeenConnected2 && !(packet[1] & 0x02))
    {
        _hasBeenConnected2 = true;
    }

    if (packet[1] == 0x00)
    {
        _sensorOff1 = false;
        _sensorOff2 = false;
    }
    else if (packet[1] == 0x01)
    {
        if (_hasBeenConnected1)
        {
            _sensorOff1 = true;
        }
        else
        {
            _sensorOff1 = false;
        }
        _sensorOff2 = false;

        _overRang1 = false;
    }
    else if (packet[1] == 0x02)
    {
        _sensorOff1 = false;
        if (_hasBeenConnected2)
        {
            _sensorOff2 = true;
        }
        else
        {
            _sensorOff2 = false;
        }
        _overRang2 = false;
    }
    else if (packet[1] == 0x03)
    {
        if (_hasBeenConnected1)
        {
            _sensorOff1 = true;
        }
        else
        {
            _sensorOff1 = false;
        }

        if (_hasBeenConnected2)
        {
            _sensorOff2 = true;
        }
        else
        {
            _sensorOff2 = false;
        }

        _overRang1 = false;
        _overRang2 = false;
    }
    _shotAlarm();
}

/**************************************************************************************************
 * 报警显示。
 *************************************************************************************************/
void T5Provider::_shotAlarm()
{
    // 自检失败
    if (_disconnected)
    {
        tempParam.setTEMP(InvData(), InvData(), InvData());
        return;
    }

    // 探头全部脱落报警，取消探头全部超界，
    if (_sensorOff1 && _sensorOff2)
    {
        _temp1 = InvData();
        _temp2 = InvData();
        _tempd = InvData();

        // 取消探头全部超界
        tempParam.setOneShotAlarm(TEMP_OVER_RANGR_ALL, false);
        tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_1, false);
        tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_2, false);
        tempParam.setOneShotAlarm(TEMP_OVER_RANGR_1, false);
        tempParam.setOneShotAlarm(TEMP_OVER_RANGR_2, false);

        // 探头全部脱落报警
        if (_hasBeenConnected1 && _hasBeenConnected2)
        {
            tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_ALL, true);
        }
    }
    else
    {
        // 取消探头全部脱落
        tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_ALL, false);

        // 探头未脱落，探头全部超界
        if (!_sensorOff1 && !_sensorOff2)
        {
            // 取消探头1、探头2脱落报警
            tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_1, false);
            tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_2, false);

            if (_overRang1 && _overRang2)
            {
                tempParam.setOneShotAlarm(TEMP_OVER_RANGR_ALL, true);
                // 取消探头1、探头2超界
                tempParam.setOneShotAlarm(TEMP_OVER_RANGR_1, false);
                tempParam.setOneShotAlarm(TEMP_OVER_RANGR_2, false);
            }
            else
            {
                tempParam.setOneShotAlarm(TEMP_OVER_RANGR_ALL, false);
                if (_overRang1)
                {
                    tempParam.setOneShotAlarm(TEMP_OVER_RANGR_1, true);
                }
                else
                {
                    tempParam.setOneShotAlarm(TEMP_OVER_RANGR_1, false);
                }

                if (_overRang2)
                {
                    tempParam.setOneShotAlarm(TEMP_OVER_RANGR_2, true);
                }
                else
                {
                    tempParam.setOneShotAlarm(TEMP_OVER_RANGR_2, false);
                }
            }
        }
        // 单个探头脱落
        else
        {
            // 取消探头全部超界
            tempParam.setOneShotAlarm(TEMP_OVER_RANGR_ALL, false);
            // 探头1脱落报警，取消探头全部脱落，取消探头全部超界，取消探头1超界
            if (_sensorOff1)
            {
                _temp1 = InvData();
                // 取消探头1超界
                tempParam.setOneShotAlarm(TEMP_OVER_RANGR_1, false);
                // 探头1脱落报警
                if (_hasBeenConnected1)
                {
                    tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_1, true);
                }
            }
            // 取消探头全部脱落，取消探头1脱落报警，取消探头1脱落报警，判断探头1是否超界
            else
            {
                // 取消探头1脱落报警
                tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_1, false);

                // 判断探头1是否超界
                if (_overRang1)
                {
                    _temp1 = InvData();
                    // 探头1超界报警
                    tempParam.setOneShotAlarm(TEMP_OVER_RANGR_1, true);
                }
                else
                {
                    // 取消探头1超界报警
                    tempParam.setOneShotAlarm(TEMP_OVER_RANGR_1, false);
                }
            }

            // 探头2脱落报警，取消探头全部超界，取消探头2超界
            if (_sensorOff2)
            {
                // 取消探头2超界
                tempParam.setOneShotAlarm(TEMP_OVER_RANGR_2, false);
                // 探头2脱落报警
                if (_hasBeenConnected2)
                {
                    tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_2, true);
                }
            }
            // 取消探头全部脱落，取消探头2脱落报警，取消探头2脱落报警，判断探头2是否超界
            else
            {
                // 取消探头2脱落报警
                tempParam.setOneShotAlarm(TEMP_SENSOR_OFF_2, false);

                // 判断探头2是否超界
                if (_overRang2)
                {
                    _temp2 = InvData();
                    // 探头2超界报警
                    tempParam.setOneShotAlarm(TEMP_OVER_RANGR_2, true);
                }
                else
                {
                    // 取消探头2超界报警
                    tempParam.setOneShotAlarm(TEMP_OVER_RANGR_2, false);
                }
            }
        }
    }
    if (_temp1 == InvData() || _temp2 == InvData())
    {
        _tempd = InvData();
    }
    tempParam.setTEMP(_temp1, _temp2, _tempd);
}

void T5Provider::_limitHandle(unsigned char *packet)
{
    // 判断T1超界
    short temp1 = static_cast<short>((packet[2] << 8) + packet[1]);
    if (temp1 == TEMP_INVALID_VALUE && (_sensorOff1 || !_hasBeenConnected1))
    {
        // 探头脱落且体温为-1时，撤销超界报警
        _overRang1 = false;
    }
    else if ((temp1 < LOW_BORDER_VALUE || temp1 > HIGH_BORDER_VALUE) && _overRang1 == false)
    {
        _overRang1 = true;
    }
    else if (temp1 >= LOW_BORDER_VALUE && temp1 <= HIGH_BORDER_VALUE && _overRang1 == true)
    {
        _overRang1 = false;
    }
    // 判断T2超界
    short temp2 = static_cast<short>((packet[4] << 8) + packet[3]);
    if (temp2 == TEMP_INVALID_VALUE && (_sensorOff2 || !_hasBeenConnected2))
    {
        _overRang2 = false;
    }
    else if ((temp2 < LOW_BORDER_VALUE || temp2 > HIGH_BORDER_VALUE) && _overRang2 == false)
    {
        _overRang2 = true;
    }
    else if (temp2 >= LOW_BORDER_VALUE && temp2 <= HIGH_BORDER_VALUE && _overRang2 == true)
    {
        _overRang2 = false;
    }
    _shotAlarm();
}

int T5Provider::borderValueChange(int low, int high, int temp)
{
    if (temp == low)
    {
        return temp + 1;
    }
    else if (temp == high)
    {
        return temp -1;
    }
    return temp;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
T5Provider::T5Provider() : BLMProvider("BLM_T5"), TEMPProviderIFace(),
    _disconnected(false), _overRang1(false), _overRang2(false),
    _sensorOff1(true), _sensorOff2(true), _hasBeenConnected1(false),
    _hasBeenConnected2(false), _temp1(InvData()), _temp2(InvData()),
    _tempd(InvData()), ohm1(InvData()), ohm2(InvData())
{
    disPatchInfo.packetType = DataDispatcher::PACKET_TYPE_T5;

    UartAttrDesc portAttr(115200, 8, 'N', 1);
    if (!initPort(portAttr))
    {
        systemManager.setPoweronTestResult(T5_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
    }

    setDisconnectThreshold(5);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
T5Provider::~T5Provider()
{
}

