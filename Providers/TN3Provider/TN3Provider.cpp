/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/26
 **/

#include "TN3Provider.h"
#include "NIBPParam.h"
#include "Debug.h"
#include <QString>
#include "IConfig.h"
#include "crc8.h"
#include "SystemManager.h"
#include "BLMEDUpgradeParam.h"
#include "TimeDate.h"
#include "ServiceVersion.h"
#include "NIBPAlarm.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include "RawDataCollection.h"
#include "IConfig.h"

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
    "The data saved in Flash is reset to the default value.\r\n",
    "The Big gas valve is unusual.\r\n",                      // 10
    "The small gas valve is unusual.\r\n",                    // 11
    "The air pump is unusual.\r\n",                           // 12
};

static const char *nibpErrorCode[] =
{
    "Flash wrong.\r\n",                                       // 128
    "Data sample exception.\r\n",                             // 129
    "The Big gas valve is unusual for running.\r\n",          // 130
    "The small gas valve is unusual for running.\r\n",        // 131
    "The air pump is unusual for running.\r\n",               // 132
    "Calibration is unsuccessful.\r\n",                       // 133
    "The Daemon error.\r\n",                                  // 134
};

/**************************************************************************************************
 * 发送复位命令。
 *************************************************************************************************/
void TN3Provider::_sendACK(unsigned char type)
{
    unsigned char data = type & 0xFF;
    sendCmd(TN3_RESPONSE_ACK, &data, 1);
}

void TN3Provider::sendSelfTest()
{
    sendCmd(TN3_CMD_SELFTEST, NULL, 0);
}

void TN3Provider::_selfTest(unsigned char *packet, int len)
{
    int num = packet[1];
    outHex(packet, len);
    if (num > 0)
    {
        int error = 0;
        QString errorStr("");
        errorStr = "error code = ";
        for (int i = 2; i < len; i++)
        {
            if (packet[i] == 0x85 || packet[i] == 0x09)
            {
                error++;
            }
            errorStr += "0x" + QString::number(packet[1], 16) + ", ";
        }
        errorStr += "\n";

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
                errorStr += nibpSelfErrorCode[packet[i]];
                break;

            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
                errorStr += nibpErrorCode[packet[i] - 0x80];
                break;
            default:
                errorStr += "Unknown mistake.\r\n";
                break;
            }
        }

        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName("TN3 Selftest Error");
        item->setLog(errorStr);
        item->setSubSystem(ErrorLogItem::SUB_SYS_TN3);
        item->setSystemState(ErrorLogItem::SYS_STAT_SELFTEST);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

        errorLog.append(item);

        nibpParam.errorDisable();
        systemManager.setPoweronTestResult(TN3_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
    }
    else if (num == 0)
    {
        systemManager.setPoweronTestResult(TN3_MODULE_SELFTEST_RESULT, SELFTEST_SUCCESS);
    }
}

void TN3Provider::_errorWarm(unsigned char *packet, int len)
{
    outHex(packet, len);
    nibpParam.errorDisable();
    QString errorStr("");
    errorStr = "error code = ";
    errorStr += "0x" + QString::number(packet[1], 16) + ", ";
    errorStr += "\r\n";

    switch (packet[1])
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
        errorStr += nibpSelfErrorCode[packet[1]];
        break;

    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
        errorStr += nibpErrorCode[packet[1] - 128];
        break;
    default:
        errorStr += "Unknown mistake.\r\n";
        break;
    }

    ErrorLogItem *item = new CriticalFaultLogItem();
    item->setName("TN3 Error");
    item->setLog(errorStr);
    item->setSubSystem(ErrorLogItem::SUB_SYS_TN3);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
    errorLog.append(item);
}

static NIBPMeasureResultInfo getMeasureResultInfo(unsigned char *data)
{
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
    return info;
}

/**************************************************************************************************
 * 数据处理。
 *************************************************************************************************/
void TN3Provider::handlePacket(unsigned char *data, int len)
{
    if (NULL == data ||  0 >= len)
    {
        return;
    }

    if (!isConnected)
    {
        nibpParam.setConnected(true);
    }
    BLMProvider::handlePacket(data, len);

    int enable = 0;
    switch (data[0])
    {
    // 启动测量
    case TN3_RSP_START_MEASURE:
        if (data[1] == 0x00)
        {
            nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_START_MEASURE, NULL, 0);
        }
        break;

    // 停止测量。
    case TN3_RSP_STOP_MEASURE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_STOP, NULL, 0);
        break;

    // 获取测量结果
    case TN3_RSP_GET_MEASUREMENT:
    {
        NIBPMeasureResultInfo info = getMeasureResultInfo(&data[1]);
        nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_GET_RESULT, reinterpret_cast<unsigned char *>(&info), sizeof(info));
        break;
    }

    // 开机自检
    case TN3_RSP_SELFTEST:
        _selfTest(data, len);
        break;

    // <15mmHg压力值周期性数据帧
    case TN3_NOTIFY_LOW_PRESSURE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_CURRENT_PRESSURE, &data[1], 2);
        break;

    // 测试压力帧
    case TN3_NOTIFY_PRESSURE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_CURRENT_PRESSURE, &data[1], 2);
        break;

    // 错误警告帧
    case TN3_DATA_ERROR:
        _sendACK(data[0]);
        _errorWarm(data, len);
        break;

    // 测量结束帧
    case TN3_NOTIFY_END:
        _sendACK(data[0]);
        nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_MEASURE_DONE, NULL, 0);
        break;

    // 启动帧
    case TN3_NOTIFY_START_UP:
    {
        _sendACK(data[0]);
        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName("TN3 Start");
        errorLog.append(item);
        nibpParam.reset();
    }
    break;

    // 保活帧
    case TN3_NOTIFY_ALIVE:
        feed();
        break;

    // 原始数据
    case TN3_NOTIFY_DATA:
        machineConfig.getNumValue("Record|NIBP", enable);
        if (enable)
        {
            rawDataCollection.pushData("BLM_TN3", data, len);
        }
        break;

    // 进入维护模式
    case TN3_RSP_ENTER_SERVICE:
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
    case TN3_RSP_CALIBRATE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_ENTER, &data[1], 1);
        break;

    // 校准点压力值反馈
    case TN3_RSP_PRESSURE_POINT:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_RSP_PRESSURE_POINT, &data[1], 1);
        break;

    // 压力计模式控制
    case TN3_RSP_MANOMETER:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_MANOMETER_ENTER, &data[1], 1);
        break;

    // 压力操控模式控制
    case TN3_RSP_PRESSURE_CONTROL:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_PRESSURECONTROL_ENTER, &data[1], 1);
        break;

    // 压力控制（充气）
    case TN3_RSP_PRESSURE_INFLATE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_PRESSURECONTROL_INFLATE, NULL, 0);
        break;

    // 放气控制
    case TN3_RSP_PRESSURE_DEFLATE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_PRESSURECONTROL_DEFLATE, NULL, 0);
        break;

    // 气阀控制
    case TN3_RSP_VALVE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_PRESSURECONTROL_VALVE, NULL, 0);
        break;

    // 进入校零模式
    case TN3_RSP_CALIBRATE_ZERO:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_ZERO_ENTER, &data[1], 1);
        break;

    // 状态改变
    case TN3_STATE_CHANGE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_STATE_CHANGE, &data[1], 1);
        break;

    // 服务模式压力帧
    case TN3_SERVICE_PRESSURE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_CURRENT_PRESSURE, &data[1], 2);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 与模块的通信中断。
 *************************************************************************************************/
void TN3Provider::disconnected(void)
{
    nibpParam.connectedFlag(false);
    nibpParam.setConnected(false);
}

/**************************************************************************************************
 * 与模块的通信恢复正常。
 *************************************************************************************************/
void TN3Provider::reconnected(void)
{
    nibpParam.connectedFlag(true);
    nibpParam.setConnected(true);
}

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool TN3Provider::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_NIBP)
    {
        nibpParam.setProvider(this);
        return true;
    }
    return false;
}

/**************************************************************************************************
 * 获取版本号。
 *************************************************************************************************/
void TN3Provider::sendVersion()
{
    sendCmd(TN3_CMD_GET_VERSION, NULL, 0);
}

/**************************************************************************************************
 * 启动测量。
 *************************************************************************************************/
void TN3Provider::startMeasure(PatientType /*type*/)
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

    sendCmd(TN3_CMD_START_MEASURE, &cmd, 1);
}

/**************************************************************************************************
 * 停止测量。
 *************************************************************************************************/
void TN3Provider::stopMeasure(void)
{
    sendCmd(TN3_CMD_STOP_MEASURE, NULL, 0);
}

void TN3Provider::setPassthroughMode(bool flag)
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
    sendCmd(TN3_NOTIFY_PASSTHROUGH_MODE, &cmd, 1);
}

// 设置预充气压力值。
void TN3Provider::setInitPressure(short pressure)
{
    unsigned char cmd[2];

    cmd[0] = pressure & 0xFF;
    cmd[1] = (pressure & 0xFF00) >> 8;

    sendCmd(TN3_CMD_INIT_PRESSURE, cmd, 2);
}

/**************************************************************************************************
 * 设置智能压力使能。
 *************************************************************************************************/
void TN3Provider::enableSmartPressure(bool enable)
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
    sendCmd(TN3_CMD_SMART_PRESSURE, &cmd, 1);
}

/**************************************************************************************************
 * 设置病人类型。
 *************************************************************************************************/
void TN3Provider::setPatientType(unsigned char type)
{
    unsigned char cmd;
    cmd = type & 0xFF;
    sendCmd(TN3_CMD_PATIENT_TYPE, &cmd, 1);
}

/**************************************************************************************************
 * 发送启动指令是否有该指令的应答。
 *************************************************************************************************/
bool TN3Provider::needStartACK(void)
{
    return true;
}

/**************************************************************************************************
 * 发送停止指令是否有该指令的应答。
 *************************************************************************************************/
bool TN3Provider::needStopACK(void)
{
    return true;
}

/**************************************************************************************************
 * 是否为错误数据包。
 *************************************************************************************************/
NIBPOneShotType TN3Provider::isMeasureError(unsigned char *)
{
    return NIBP_ONESHOT_NONE;
}

/**************************************************************************************************
 * 获取结果请求。
 *************************************************************************************************/
void TN3Provider::getResult(void)
{
    sendCmd(TN3_CMD_GET_MEASUREMENT, NULL, 0);
}

/**************************************************************************************************
 * 进入维护模式。
 *************************************************************************************************/
void TN3Provider::serviceEnter(bool enter)
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
    sendCmd(TN3_CMD_ENTER_SERVICE, &cmd, 1);
}

/**************************************************************************************************
 *进入校准模式。
 *************************************************************************************************/
void TN3Provider::serviceCalibrate(bool enter)
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
    sendCmd(TN3_CMD_CALIBRATE, &cmd, 1);
}

/**************************************************************************************************
 *校准点压力值输入。
 *************************************************************************************************/
void TN3Provider::servicePressurepoint(const unsigned char *data, unsigned int len)
{
    sendCmd(TN3_CMD_PRESSURE_POINT, data, len);
}

/**************************************************************************************************
 *进入压力计模式控制。
 *************************************************************************************************/
void TN3Provider::serviceManometer(bool enter)
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
    sendCmd(TN3_CMD_MANOMETER, &cmd, 1);
}

/**************************************************************************************************
 *进入压力操控模式。
 *************************************************************************************************/
void TN3Provider::servicePressurecontrol(bool enter)
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
    sendCmd(TN3_CMD_PRESSURE_CONTROL, &cmd, 1);
}

/**************************************************************************************************
 *压力控制（充气）。
 *************************************************************************************************/
void TN3Provider::servicePressureinflate(short pressure)
{
    unsigned char cmd[2];
    cmd[0] = pressure & 0x00FF;
    cmd[1] = (pressure & 0xFF00) >> 8;
    sendCmd(TN3_CMD_PRESSURE_INFLATE, cmd, 2);
}

/**************************************************************************************************
 *压力控制（充气）应答。
 *************************************************************************************************/
bool TN3Provider::isServicePressureinflate(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_PRESSURE_INFLATE)
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
void TN3Provider::servicePressuredeflate()
{
    sendCmd(TN3_CMD_PRESSURE_DEFLATE, NULL, 0);
}

/**************************************************************************************************
 *服务模式的压力值。
 *************************************************************************************************/
int TN3Provider::serviceCuffPressure(unsigned char *packet)
{
    if (packet[0] != TN3_SERVICE_PRESSURE)
    {
        return -1;
    }

    return (static_cast<int>((packet[2] << 8) + packet[1]));
}

/**************************************************************************************************
 *进入校零模式。
 *************************************************************************************************/
void TN3Provider::serviceCalibrateZero(bool enter)
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
    sendCmd(TN3_CMD_CALIBRATE_ZERO, &cmd, 1);
}

/**************************************************************************************************
 *进入校零模式返回值。
 *************************************************************************************************/
bool TN3Provider::isServiceCalibrateZero(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_CALIBRATE_ZERO)
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
void TN3Provider::servicePressureZero(void)
{
    sendCmd(TN3_CMD_PRESSURE_ZERO, NULL, 0);
}

/**************************************************************************************************
 *校零。
 *************************************************************************************************/
bool TN3Provider::isServicePressureZero(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_PRESSURE_ZERO)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *气阀控制。
 *************************************************************************************************/
void TN3Provider::serviceValve(bool enter)
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
    sendCmd(TN3_CMD_VALVE, cmd, 2);

    cmd[0] = 0x00;
    if (enter)
    {
        cmd[1] = 0x01;
    }
    else
    {
        cmd[1] = 0x00;
    }
    sendCmd(TN3_CMD_VALVE, cmd, 2);
}

/**************************************************************************************************
 *气阀控制应答。
 *************************************************************************************************/
bool TN3Provider::isServiceValve(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_VALVE)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *过压保护是否有效。
 *************************************************************************************************/
void TN3Provider::servicePressureProtect(bool enter)
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
    sendCmd(TN3_CMD_PRESSURE_PROTECT, &cmd, 1);
}

/**************************************************************************************************
 *过压保护是否有效应答。
 *************************************************************************************************/
bool TN3Provider::isServicePressureProtect(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_PRESSURE_PROTECT)
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
void TN3Provider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    sendCmd(cmdId, data, len);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TN3Provider::TN3Provider() : BLMProvider("BLM_TN3"), NIBPProviderIFace()
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    if (!initPort(portAttr))
    {
        systemManager.setPoweronTestResult(TN3_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
    }

    setDisconnectThreshold(5);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TN3Provider::~TN3Provider()
{
}
