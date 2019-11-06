/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/26
 **/

#include "N5Provider.h"
#include "NIBPParam.h"
#include "Debug.h"
#include <QString>
#include "IConfig.h"
#include "SystemManager.h"
#include "TimeDate.h"
#include "NIBPAlarm.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include "RawDataCollector.h"
#include "IConfig.h"
#include "UpgradeManager.h"
#include "AlarmSourceManager.h"

static const char *nibpSelfErrorCode[] =
{
    "Unknown mistake.\r\n",                                   // 0
    "6V self-test failed.\r\n",                               // 1
    "5V self-test failed.\r\n",                               // 2
    "5Va self-test failed.\r\n",                              // 3
    "3.3Va1 self-test failed.\r\n",                           // 4
    "3.3Va2 self-test failed.\r\n",                           // 5
    "3.3V self-test failed.\r\n",                             // 6
    "15V self-test failed.\r\n",                              // 7
    "AD7739 Self-test failed.\r\n",                           // 8
    "The data saved in Flash is reset to the default value.\r\n",  // 校准数据恢复默认
    "The Big gas valve is unusual.\r\n",                      // 10
    "The small gas valve is unusual.\r\n",                    // 11
    "The air pump is unusual.\r\n",                           // 12
    "The sofaware of overpressure protect is unusual.\r\n",   // 13
    "Zero fail on start-up.\r\n",                              // 14
    "Calibration is unsuccessful.\r\n"                       // 15
};

static const char *nibpErrorCode[] =
{
    "Comparisons of pressure between master and Daemon fail to pass self-test",  // 126
    "Master-slave communication is unusual.\r\n",             // 127
    "Flash wrong.\r\n",                                       // 128
    "Data sample exception.\r\n",                             // 129
    "The Big gas valve is unusual for running.\r\n",          // 130
    "The small gas valve is unusual for running.\r\n",        // 131
    "The air pump is unusual for running.\r\n",               // 132
    "The Daemon error.\r\n"                                  // 133
};

/**************************************************************************************************
 * 发送复位命令。
 *************************************************************************************************/
void N5Provider::_sendACK(unsigned char type)
{
    unsigned char data = type & 0xFF;
    sendCmd(N5_RESPONSE_ACK, &data, 1);
}

void N5Provider::sendSelfTest()
{
    sendCmd(N5_CMD_SELFTEST, NULL, 0);
}

void N5Provider::_selfTest(unsigned char *packet, int len)
{
    int num = packet[1];
    if (num > 0)
    {
        QString errorStr("");
        errorStr = "error code = ";
        for (int i = 2; i < len; i++)
        {
            errorStr += "0x" + QString::number(packet[i], 16) + ", ";
        }
        errorStr += "\n";

        /* nibp selftest got error */
        qWarning() << Q_FUNC_INFO << getName() << "Selftest error, " << errorStr;

        for (int i = 2; i < len; i++)
        {
            switch (packet[i])
            {
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x0E:
            case 0x0F:
                errorStr += nibpSelfErrorCode[packet[i]];
                break;
            default:
                errorStr += "Unknown mistake.\r\n";
                break;
            }
        }
        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName("N5 Selftest Error");
        item->setLog(errorStr);
        item->setSubSystem(ErrorLogItem::SUB_SYS_N5);
        item->setSystemState(ErrorLogItem::SYS_STAT_SELFTEST);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

        errorLog.append(item);
        systemManager.setPoweronTestResult(N5_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
    }
    else if (num == 0)
    {
        systemManager.setPoweronTestResult(N5_MODULE_SELFTEST_RESULT, SELFTEST_SUCCESS);
    }
}

void N5Provider::_handleError(unsigned char error)
{
    int _error = 0x00;
    switch (error)
    {
    case SELF_TEST_6V_FAILED:
    case SELF_TEST_5V_FAILED:
    case SELF_TEST_5VA_FAILED:
    case SELF_TEST_3_3VA1_FAILED:
    case SELF_TEST_3_3VA2_FAILED:
    case SELF_TEST_3_3V_FAILED:
    case SELF_TEST_15V_FAILED:
    case AD7739_SELF_TEST_FAILED:
    case The_BIG_GAS_VALUE_IS_UNSUAL:
    case THE_SMALL_GAS_VALVE_IS_UNUSUAL:
    case THE_AIR_PUMP_IS_UNUSUAL:
    case THE_SOFTWARE_OF_OVERPRESSURE_PROTECT_IS_UNUSUAL:
        _error |= N5_TYPE_SELFTEST_FAIL;   // 模块自检失败
        break;
    case RESET_TO_THE_DEFAULT_VALVE:
    case CALIBRATION_IS_UNSUCCESSFUL:
        _error |= N5_TYPE_NOT_CALIBRATE;  // 模块未校准
        break;
    case ZERO_FAIL_ON_START_UP:
    case FLASH_WRONG:
        _error |= N5_TYPE_ABNORMAL;       // 模块异常
        break;
    case MASTER_AND_DAEMON_FAIL_TO_PASS_SELF_TEST:
    case MASTER_SLAVE_COMMUNICATION_IS_UNUSUAL:
    case DATA_SAMPLE_EXCEPTION:
    case THE_BIG_GAS_VALVE_IS_UNUSUAL_FOR_RUNNING:
    case THE_SMALL_GAS_VALVE_IS_UNUSUAL_FOR_RUNNING:
    case THE_AIR_PUMP_IS_UNUSUAL_FOR_RUNNING:
        _error |= N5_TYPE_ERROR;         // 模块错误
        break;
    }
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_NIBP);
    if (!alarmSource)
    {
        return;
    }

    if (_error & N5_TYPE_NOT_CALIBRATE)
    {
        alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_MODULE_NOT_CALIBRATE, true);   // 模块未校准
    }
    else if (_error & N5_TYPE_ABNORMAL)
    {
        alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_MODULE_ABNORMAL, true);   // 模块异常
    }
    else if (_error & N5_TYPE_SELFTEST_FAIL)
    {
        alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_SELTTEST_ERROR, true);   // 模块自检失败
        nibpParam.setDisableState(true);    // 设置为不可测量
        nibpParam.errorDisable();
    }
    else if (_error & N5_TYPE_ERROR)
    {
        alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_MODULE_ERROR, true);   // 模块错误
        nibpParam.setDisableState(true);
        nibpParam.errorDisable();
    }
}

void N5Provider::_handleSelfTestError(unsigned char *packet, int len)
{
    int num = packet[1];
    if (num > 0)
    {
        for (int i = 2; i < len; i++)
        {
            _handleError(packet[i]);
        }
    }
}

void N5Provider::_errorWarm(unsigned char *packet, int len)
{
    outHex(packet, len);
    QString errorStr("");
    errorStr = "error code = ";
    errorStr += "0x" + QString::number(packet[1], 16) + ", ";
    errorStr += "\r\n";

    switch (packet[1])
    {
    case 0x7e:
    case 0x7f:
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
        errorStr += nibpErrorCode[packet[1] - 126];
        break;
    default:
        errorStr += "Unknown mistake.\r\n";
        break;
    }

    ErrorLogItem *item = new CriticalFaultLogItem();
    item->setName("N5 Error");
    item->setLog(errorStr);
    item->setSubSystem(ErrorLogItem::SUB_SYS_N5);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
    errorLog.append(item);
}

static NIBPMeasureResultInfo getMeasureResultInfo(unsigned char *data)
{
    int type;
    systemConfig.getNumValue("General|PatientType", type);

    NIBPMeasureResultInfo info;
    short t = static_cast<short>(data[0]);
    info.errCode = t;
    t = static_cast<short>(data[1] + (data[2] << 8));
    info.sys = t;
    t = static_cast<short>(data[3] + (data[4] << 8));
    info.dia = t;
    t = static_cast<short>(data[5] + (data[6] << 8));
    info.map = t;
    t = static_cast<short>(data[7] + (data[8] << 8));
    info.pr = t;
    if (info.errCode != 0x00)
    {
        return info;
    }
    if (type == PATIENT_TYPE_ADULT)
    {
        if (info.sys > 255 || info.sys < 40 || info.dia > 215 || info.dia < 20 || info.map > 235 || info.map < 20)
        {
            info.errCode = 0x06;
        }
    }
    else if (type == PATIENT_TYPE_PED)
    {
        if (info.sys > 200 || info.sys < 40 || info.dia > 150 || info.dia < 20 || info.map > 165 || info.map < 20)
        {
            info.errCode = 0x06;
        }
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        if (info.sys > 135 || info.sys < 40 || info.dia > 100 || info.dia < 10 || info.map > 110 || info.map < 20)
        {
            info.errCode = 0x06;
        }
    }
    return info;
}

/**************************************************************************************************
 * 数据处理。
 *************************************************************************************************/
void N5Provider::handlePacket(unsigned char *data, int len)
{
    if (!isConnectedToParam)
    {
        return;
    }

    if (NULL == data ||  0 >= len)
    {
        return;
    }

    if (!isConnected)
    {
        nibpParam.setConnected(true);
    }
    BLMProvider::handlePacket(data, len);

    switch (data[0])
    {
    // 启动测量
    case N5_RSP_START_MEASURE:
        if (data[1] == 0x00)
        {
            nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_START_MEASURE, NULL, 0);
        }
        break;

    // 停止测量。
    case N5_RSP_STOP_MEASURE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_STOP, NULL, 0);
        rawDataCollector.collectData(RawDataCollector::NIBP_DATA, NULL, 0, true);
        break;

    // 获取测量结果
    case N5_RSP_GET_MEASUREMENT:
    {
        NIBPMeasureResultInfo info = getMeasureResultInfo(&data[1]);
        nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_GET_RESULT, reinterpret_cast<unsigned char *>(&info),
                                  sizeof(info));
        break;
    }

    // 开机自检
    case N5_RSP_SELFTEST:
        _selfTest(data, len);
        _handleSelfTestError(data, len);
        break;

    // <15mmHg压力值周期性数据帧
    case N5_NOTIFY_LOW_PRESSURE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_CURRENT_PRESSURE, &data[1], 2);
        break;

    // 测试压力帧
    case N5_NOTIFY_PRESSURE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_CURRENT_PRESSURE, &data[1], 2);
        break;

    // 错误警告帧
    case N5_DATA_ERROR:
        _sendACK(data[0]);
        _errorWarm(data, len);
        _handleError(data[1]);
        break;

    // 测量结束帧
    case N5_NOTIFY_END:
        _sendACK(data[0]);
        nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_MEASURE_DONE, NULL, 0);
        rawDataCollector.collectData(RawDataCollector::NIBP_DATA, NULL, 0, true);
        break;

    // 启动帧
    case N5_NOTIFY_START_UP:
    {
        _sendACK(data[0]);
        ErrorLogItem *item = new ErrorLogItem();
        item->setName("N5 Start");
        errorLog.append(item);
        nibpParam.reset();
    }
    break;

    // 保活帧
    case N5_NOTIFY_ALIVE:
        feed();
        break;

    // 原始数据
    case N5_NOTIFY_DATA:
        rawDataCollector.collectData(RawDataCollector::NIBP_DATA, data + 1, len - 1);
        break;

    // 进入维护模式
    case N5_RSP_ENTER_SERVICE:
        if (data[1] != 0x00)
        {
            nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_REPAIR_ENTER_FAIL, NULL, 0);
        }
        else
        {
            nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_REPAIR_ENTER_SUCCESS, NULL, 0);
        }
        break;

    // 校准模式控制
    case N5_RSP_CALIBRATE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_ENTER, &data[1], 1);
        break;

    // 校准点压力值反馈
    case N5_RSP_PRESSURE_POINT:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_RSP_PRESSURE_POINT, &data[1], 1);
        break;

    case N5_RSP_PASSTHROUGH_MODE:
        break;

    // 压力计模式控制
    case N5_RSP_MANOMETER:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_MANOMETER_ENTER, &data[1], 1);
        break;

    // 压力操控模式控制
    case N5_RSP_PRESSURE_CONTROL:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_PRESSURECONTROL_ENTER, &data[1], 1);
        break;

    // 压力控制（充气）
    case N5_RSP_PRESSURE_INFLATE:
        // 0x8A回复帧，如果等于2时候是发送充气命令立刻回复,等于1或者0时候是充到指定压力回复。
        if (data[1] == 0 || data[1] == 1)
        {
            nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_PRESSURECONTROL_INFLATE, &data[1], 1);
        }
        break;

    // 放气控制
    case N5_RSP_PRESSURE_DEFLATE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_PRESSURECONTROL_DEFLATE, NULL, 0);
        break;

    // 气泵控制
    case N5_RSP_PUMP:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_PRESSURECONTROL_PUMP, &data[1], 1);
        break;

    // 气阀控制
    case N5_RSP_VALVE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_PRESSURECONTROL_VALVE, NULL, 0);
        break;

    // 进入校零模式
    case N5_RSP_CALIBRATE_ZERO:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_ZERO_ENTER, &data[1], 1);
        break;

    // 状态改变
    case N5_STATE_CHANGE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_STATE_CHANGE, &data[1], 1);
        break;

    // 开机获取校零状态
    case N5_STATE_ZERO_SELFTEST:
        _sendACK(data[0]);
        nibpParam.handleNIBPEvent(NIBP_EVENT_ZERO_SELFTEST, &data[1], 1);
        break;

    // 服务模式压力帧
    case N5_SERVICE_PRESSURE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_CURRENT_PRESSURE, &data[1], 2);
        break;

    case N5_RSP_PRESSURE_ZERO:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_ZERO, NULL, 0);
        break;
    case N5_STATE_PRESSURE_PROTECT:
        if ((data[1] & N5_TYPE_PROTECT_MASTE_PROTECT)
            || (data[1] & N5_TYPE_PROTECT_SLAVE_PROTECT)
                || (data[1] & N5_TYPE_PROTECT_HARDWARE_PROTECT))
        {
            AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_NIBP);
            if (data[1] & N5_TYPE_PROTECT_HARDWARE_PROTECT)
            {
                if (alarmSource)
                {
                    alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_MODULE_ABNORMAL, true);
                }
            }
            if (alarmSource)
            {
                alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_MODULE_OVER_PRESSURE_PROTECT, true);
            }
            nibpParam.setDisableState(true);
            nibpParam.errorDisable();
        }
        else if (data[1] == N5_TYPE_PROTECT_NORMAL)
        {
            nibpParam.setDisableState(false);
            AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_NIBP);
            if (alarmSource)
            {
                alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_MODULE_OVER_PRESSURE_PROTECT, false);
            }
            nibpParam.handleNIBPEvent(NIBP_EVENT_CONNECTION_NORMAL, NULL, 0);                       // 恢复禁用状态
        }
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 与模块的通信中断。
 *************************************************************************************************/
void N5Provider::disconnected(void)
{
    nibpParam.connectedFlag(false);
    nibpParam.setConnected(false);
}

/**************************************************************************************************
 * 与模块的通信恢复正常。
 *************************************************************************************************/
void N5Provider::reconnected(void)
{
    nibpParam.connectedFlag(true);
    nibpParam.setConnected(true);
}

void N5Provider::sendDisconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_NIBP);
    if (alarmSource && !alarmSource->isAlarmed(NIBP_ONESHOT_ALARM_COMMUNICATION_STOP))
    {
        alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_SEND_COMMUNICATION_STOP, true);
    }
}

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool N5Provider::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_NIBP)
    {
        nibpParam.setProvider(this);
        Provider::attachParam(param);
        return true;
    }
    return false;
}

/**************************************************************************************************
 * 获取版本号。
 *************************************************************************************************/
void N5Provider::sendVersion()
{
    sendCmd(N5_CMD_GET_VERSION, NULL, 0);
}

/**************************************************************************************************
 * 启动测量。
 *************************************************************************************************/
void N5Provider::startMeasure(PatientType /*type*/)
{
    unsigned char cmd;
    if (nibpParam.getMeasurMode() == NIBP_MODE_AUTO)
    {
        cmd = 0x01;
    }
    else if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
    {
        if (nibpParam.isSTATFirst())
        {
            cmd = 0x02;
            nibpParam.setSTATFirst(false);
        }
        else
        {
            cmd = 0x03;
        }
    }
    else
    {
        cmd = 0x00;
    }

    sendCmd(N5_CMD_START_MEASURE, &cmd, 1);
}

/**************************************************************************************************
 * 停止测量。
 *************************************************************************************************/
void N5Provider::stopMeasure(void)
{
    sendCmd(N5_CMD_STOP_MEASURE, NULL, 0);
}

void N5Provider::setPassthroughMode(bool flag)
{
    unsigned char cmd;
    if (flag)
    {
        cmd = 0x01;
    }
    else
    {
        cmd = 0x00;
    }
    sendCmd(N5_NOTIFY_PASSTHROUGH_MODE, &cmd, 1);
}

// 设置预充气压力值。
void N5Provider::setInitPressure(short pressure)
{
    unsigned char cmd[2];

    cmd[0] = pressure & 0xFF;
    cmd[1] = (pressure & 0xFF00) >> 8;

    sendCmd(N5_CMD_INIT_PRESSURE, cmd, 2);
}

/**************************************************************************************************
 * 设置智能压力使能。
 *************************************************************************************************/
void N5Provider::enableSmartPressure(bool enable)
{
    unsigned char cmd;
    if (enable)
    {
        cmd = 0x01;
    }
    else
    {
        cmd = 0x00;
    }
    sendCmd(N5_CMD_SMART_PRESSURE, &cmd, 1);
}

/**************************************************************************************************
 * 设置病人类型。
 *************************************************************************************************/
void N5Provider::setPatientType(unsigned char type)
{
    unsigned char cmd;
    cmd = type & 0xFF;
    sendCmd(N5_CMD_PATIENT_TYPE, &cmd, 1);
}

/**************************************************************************************************
 * 发送启动指令是否有该指令的应答。
 *************************************************************************************************/
bool N5Provider::needStartACK(void)
{
    return true;
}

/**************************************************************************************************
 * 发送停止指令是否有该指令的应答。
 *************************************************************************************************/
bool N5Provider::needStopACK(void)
{
    return true;
}

void N5Provider::enableRawDataSend(bool onOff)
{
    unsigned char cmd = onOff;
    sendCmd(N5_CMD_RAW_DATA, &cmd, 1);
}

/**************************************************************************************************
 * 是否为错误数据包。
 *************************************************************************************************/
NIBPOneShotType N5Provider::isMeasureError(unsigned char *)
{
    return NIBP_ONESHOT_NONE;
}

/**************************************************************************************************
 * 获取结果请求。
 *************************************************************************************************/
void N5Provider::getResult(void)
{
    sendCmd(N5_CMD_GET_MEASUREMENT, NULL, 0);
}

/**************************************************************************************************
 * 进入维护模式。
 *************************************************************************************************/
void N5Provider::serviceEnter(bool enter)
{
    unsigned char cmd;
    if (enter)
    {
        cmd = 0x01;
    }
    else
    {
        cmd = 0x00;
    }
    sendCmd(N5_CMD_ENTER_SERVICE, &cmd, 1);
}

/**************************************************************************************************
 *进入校准模式。
 *************************************************************************************************/
void N5Provider::serviceCalibrate(bool enter)
{
    unsigned char cmd;
    if (enter)
    {
        cmd = 0x01;
    }
    else
    {
        cmd = 0x00;
    }
    sendCmd(N5_CMD_CALIBRATE, &cmd, 1);
}

/**************************************************************************************************
 *校准点压力值输入。
 *************************************************************************************************/
void N5Provider::servicePressurepoint(const unsigned char *data, unsigned int len)
{
    unsigned char cmd[3] = {0};
    int pressure = data[0] | (data[1] << 8);
    if (pressure)
    {
        cmd[0] = 0x01;
    }
    else
    {
        cmd[0] = 0x00;
    }
    cmd[1] = pressure & 0xFF;
    cmd[2] = (pressure >> 8) & 0xFF;
    len  = 3;
    sendCmd(N5_CMD_PRESSURE_POINT, cmd, len);
}

/**************************************************************************************************
 *进入压力计模式控制。
 *************************************************************************************************/
void N5Provider::serviceManometer(bool enter)
{
    unsigned char cmd;
    if (enter)
    {
        cmd = 0x01;
    }
    else
    {
        cmd = 0x00;
    }
    sendCmd(N5_CMD_MANOMETER, &cmd, 1);
}

/**************************************************************************************************
 *进入压力操控模式。
 *************************************************************************************************/
void N5Provider::servicePressurecontrol(bool enter)
{
    unsigned char cmd;
    if (enter)
    {
        cmd = 0x01;
    }
    else
    {
        cmd = 0x00;
    }
    sendCmd(N5_CMD_PRESSURE_CONTROL, &cmd, 1);
}

/**************************************************************************************************
 *压力控制（充气）。
 *************************************************************************************************/
void N5Provider::servicePressureinflate(short pressure)
{
    unsigned char cmd[2];
    cmd[0] = pressure & 0x00FF;
    cmd[1] = (pressure & 0xFF00) >> 8;
    sendCmd(N5_CMD_PRESSURE_INFLATE, cmd, 2);
}

/**************************************************************************************************
 *压力控制（充气）应答。
 *************************************************************************************************/
bool N5Provider::isServicePressureinflate(unsigned char *packet)
{
    if (packet[0] != N5_RSP_PRESSURE_INFLATE)
    {
        return false;
    }

    if (packet[1] != 0x00)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *放气控制。
 *************************************************************************************************/
void N5Provider::servicePressuredeflate()
{
    sendCmd(N5_CMD_PRESSURE_DEFLATE, NULL, 0);
}

/**************************************************************************************************
 *服务模式的压力值。
 *************************************************************************************************/
int N5Provider::serviceCuffPressure(unsigned char *packet)
{
    if (packet[0] != N5_SERVICE_PRESSURE)
    {
        return -1;
    }

    return (static_cast<int>((packet[2] << 8) + packet[1]));
}

/**************************************************************************************************
 *进入校零模式。
 *************************************************************************************************/
void N5Provider::serviceCalibrateZero(bool enter)
{
    unsigned char cmd;
    if (enter)
    {
        cmd = 0x01;
    }
    else
    {
        cmd = 0x00;
    }
    sendCmd(N5_CMD_CALIBRATE_ZERO, &cmd, 1);
}

/**************************************************************************************************
 *进入校零模式返回值。
 *************************************************************************************************/
bool N5Provider::isServiceCalibrateZero(unsigned char *packet)
{
    if (packet[0] != N5_RSP_CALIBRATE_ZERO)
    {
        return false;
    }

    if (packet[1] != 0x00)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *校零。
 *************************************************************************************************/
void N5Provider::servicePressureZero(void)
{
    sendCmd(N5_CMD_PRESSURE_ZERO, NULL, 0);
}

/**************************************************************************************************
 *校零。
 *************************************************************************************************/
bool N5Provider::isServicePressureZero(unsigned char *packet)
{
    if (packet[0] != N5_RSP_PRESSURE_ZERO)
    {
        return false;
    }

    return true;
}

void N5Provider::servicePump(bool enter, unsigned char pump)
{
    unsigned char cmd[2];
    cmd[0] = enter;
    cmd[1] = pump;
    sendCmd(N5_CMD_PUMP, cmd, 2);
}

/**************************************************************************************************
 *气阀控制。
 *************************************************************************************************/
void N5Provider::serviceValve(bool enter)
{
    unsigned char cmd[2];
    cmd[0] = 0x01;
    if (enter)
    {
        cmd[1] = 0x01;
    }
    else
    {
        cmd[1] = 0x00;
    }
    sendCmd(N5_CMD_VALVE, cmd, 2);

    cmd[0] = 0x00;
    if (enter)
    {
        cmd[1] = 0x01;
    }
    else
    {
        cmd[1] = 0x00;
    }
    sendCmd(N5_CMD_VALVE, cmd, 2);
}

/**************************************************************************************************
 *气阀控制应答。
 *************************************************************************************************/
bool N5Provider::isServiceValve(unsigned char *packet)
{
    if (packet[0] != N5_RSP_VALVE)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *过压保护是否有效。
 *************************************************************************************************/
void N5Provider::servicePressureProtect(bool enter)
{
    unsigned char cmd;
    if (enter)
    {
        cmd = 0x00;
    }
    else
    {
        cmd = 0x01;
    }
    sendCmd(N5_CMD_PRESSURE_PROTECT, &cmd, 1);
}

/**************************************************************************************************
 *过压保护是否有效应答。
 *************************************************************************************************/
bool N5Provider::isServicePressureProtect(unsigned char *packet)
{
    if (packet[0] != N5_RSP_PRESSURE_PROTECT)
    {
        return false;
    }

    if (packet[1] != 0x00)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void N5Provider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    sendCmd(cmdId, data, len);
}

unsigned char N5Provider::convertErrcode(unsigned char code)
{
    unsigned char err;
    switch (code)
    {
    case 0x02:
        err = NIBP_ONESHOT_ALARM_CUFF_ERROR;
        break;
    case 0x05:
        err = NIBP_ONESHOT_ALARM_SIGNAL_WEAK;
        break;
    case 0x06:
        err = NIBP_ONESHOT_ALARM_MEASURE_OVER_RANGE;
        break;
    case 0x08:
        err = NIBP_ONESHOT_ALARM_CUFF_OVER_PRESSURE;
        break;
    case 0x09:
        err = NIBP_ONESHOT_ALARM_SIGNAL_SATURATION;
        break;
    case 0x0A:
        err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
        break;
    default:
        err = NIBP_ONESHOT_NONE;
        break;
    }
    return err;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
N5Provider::N5Provider() : BLMProvider("BLM_N5"), NIBPProviderIFace()
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    if (!initPort(portAttr))
    {
        systemManager.setPoweronTestResult(N5_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
    }

    setDisconnectThreshold(5);
    _hardWareProtect = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
N5Provider::~N5Provider()
{
}
