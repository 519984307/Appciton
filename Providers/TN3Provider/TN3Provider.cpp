#include "TN3Provider.h"
#include "NIBPParamService.h"
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

static const char *nibpSelfErrorCode[] = {
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

static const char *nibpErrorCode[] = {
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
    int error = 0;
    int num = packet[1];
    outHex(packet,len);
    if (num > 0)
    {
        QString errorStr("");
        errorStr = "error code = ";
        for(int i = 2; i < len; i++)
        {
            if (packet[i] == 0x85 || packet[i] == 0x09)
            {
                error++;
            }
            errorStr += QString().sprintf("0x%02x, ", packet[i]);
        }
        errorStr += "\n";

        for(int i = 2; i < len; i++)
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
        systemManager.setPoweronTestResult(TN3_MODULE_SELFTEST_RESULT,SELFTEST_FAILED);
    }
    else if (num == 0)
    {
        systemManager.setPoweronTestResult(TN3_MODULE_SELFTEST_RESULT,SELFTEST_SUCCESS);
    }
}

void TN3Provider::_errorWarm(unsigned char *packet, int len)
{
    outHex(packet,len);
    nibpParam.errorDisable();
    QString errorStr("");
    errorStr = "error code = ";
    errorStr += QString().sprintf("0x%02x, ", packet[1]);
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

/**************************************************************************************************
 * 数据处理。
 *************************************************************************************************/
void TN3Provider::handlePacket(unsigned char *data, int len)
{
    if(NULL == data ||  0 >= len)
    {
        return;
    }
    BLMProvider::handlePacket(data, len);

    if ((data[0] == TN3_NOTIFY_START_UP) || (data[0] == TN3_NOTIFY_END))
    {
        _sendACK(data[0]);

        if (data[0] == TN3_NOTIFY_START_UP)
        {
            ErrorLogItem *item = new CriticalFaultLogItem();
            item->setName("TN3 Start");
            errorLog.append(item);

            nibpParam.reset();
        }
    }

    int enable = 0;
    switch(data[0])
    {
    case TN3_NOTIFY_ALIVE:
        feed();
        break;
    case TN3_NOTIFY_DATA:
        machineConfig.getNumValue("Record|NIBP", enable);
        if (enable)
        {
            rawDataCollection.pushData("BLM_TN3", data,len);
        }
        break;
    case TN3_RSP_SELFTEST:
        _selfTest(data, len);
        break;
    case TN3_DATA_ERROR:
        _sendACK(data[0]);
        _errorWarm(data, len);
        break;
    default:
        nibpParam.unPacket(data, len);
        break;
    }
}

/**************************************************************************************************
 * 与模块的通信中断。
 *************************************************************************************************/
void TN3Provider::disconnected(void)
{
    nibpParam.connectedTimeout();
}

/**************************************************************************************************
 * 与模块的通信恢复正常。
 *************************************************************************************************/
void TN3Provider::reconnected(void)
{
    nibpOneShotAlarm.setOneShotAlarm(NIBP_ONESHOT_ALARM_COMMUNICATION_STOP, false);
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

/**************************************************************************************************
 * <15mmHg压力值周期性数据帧
 *************************************************************************************************/
short TN3Provider::lowPressure(unsigned char *packet)
{
    if (packet[0] != TN3_NOTIFY_LOW_PRESSURE)
    {
        return -1;
    }

    short pressure = (packet[2]<<8)+packet[1];

    return pressure;
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
    if(enable)
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
 * 是否为启动测量的应答。
 *************************************************************************************************/
bool TN3Provider::isStartACK(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_START_MEASURE)
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
 * 发送停止指令是否有该指令的应答。
 *************************************************************************************************/
bool TN3Provider::needStopACK(void)
{
    return true;
}

/**************************************************************************************************
 * 是否为停止测量的应答。
 *************************************************************************************************/
bool TN3Provider::isStopACK(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_STOP_MEASURE)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 * 压力数据，不是压力数据返回-1。
 *************************************************************************************************/
short TN3Provider::cuffPressure(unsigned char *packet)
{
    if (packet[0] != TN3_NOTIFY_PRESSURE)
    {
        return -1;
    }

    short pressure = (packet[2]<<8)+packet[1];

    return pressure;
}

/**************************************************************************************************
 * 测量是否结束。
 *************************************************************************************************/
bool TN3Provider::isMeasureDone(unsigned char *packet)
{
    if (packet[0] != TN3_NOTIFY_END)
    {
        return false;
    }

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
 * 是否为结果包。
 *************************************************************************************************/
bool TN3Provider::isResult(unsigned char *packet,short &sys,
                           short &dia, short &map, short &pr, NIBPOneShotType &err)
{
    if (packet[0] != TN3_RSP_GET_MEASUREMENT)
    {
        return false;
    }

    err = NIBP_ONESHOT_NONE;

    // 测量有错误，获取错误码。
    if (packet[1] != 0x00)
    {
        switch (packet[1])
        {
        case 0x02: err = NIBP_ONESHOT_ALARM_CUFF_ERROR; break;
        case 0x05: err = NIBP_ONESHOT_ALARM_SIGNAL_WEAK; break;
        case 0x06: err = NIBP_ONESHOT_ALARM_MEASURE_OVER_RANGE; break;
        case 0x08: err = NIBP_ONESHOT_ALARM_CUFF_OVER_PRESSURE; break;
        case 0x09: err = NIBP_ONESHOT_ALARM_SIGNAL_SATURATION; break;
        case 0x0A: err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT; break;
        default: break;
        }

        return true;
    }
    // 测量无错，获取测量结果。
    int t = (int)((packet[3]<<8)+packet[2]);
    if (t == 65535)
    {
        sys = InvData();
    }
    else
    {
        sys = t;
    }

    t = (int)((packet[5]<<8)+packet[4]);
    if (t == 65535)
    {
        dia = InvData();
    }
    else
    {
        dia = t;
    }

    t = (int)((packet[7]<<8)+packet[6]);
    if (t == 65535)
    {
        map = InvData();
    }
    else
    {
        map = t;
    }

    t = (int)((packet[9]<<8)+packet[8]);
    if (t == 65535)
    {
        pr = InvData();
    }
    else
    {
        pr = t;
    }

    if (sys == InvData() || dia == InvData() || map == InvData())
    {
        sys = InvData();
        dia = InvData();
        map = InvData();
    }
    return true;
}

/**************************************************************************************************
 * 进入维护模式。
 *************************************************************************************************/
void TN3Provider::service_Enter(bool enter)
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
 * 维护模式应答。
 *************************************************************************************************/
bool TN3Provider::isService_Enter(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_ENTER_SERVICE)
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
 *进入校准模式。
 *************************************************************************************************/
void TN3Provider::service_Calibrate(bool enter)
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
 *校准模式应答。
 *************************************************************************************************/
bool TN3Provider::isService_Calibrate(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_CALIBRATE)
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
 *校准点压力值输入。
 *************************************************************************************************/
void TN3Provider::service_Pressurepoint(int num, short pressure)
{
    unsigned char cmd[3];
    cmd[0] = (char)(num & 0xFF);
    cmd[1] = pressure & 0xFF;
    cmd[2] = (pressure & 0xFF00) >> 8;
    sendCmd(TN3_CMD_PRESSURE_POINT, cmd, 3);
}

/**************************************************************************************************
 *校准点压力值输入应答。
 *************************************************************************************************/
bool TN3Provider::isService_Pressurepoint(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_PRESSURE_POINT)
    {
        return false;
    }

    if (packet[1] != 0x01)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *进入压力计模式控制。
 *************************************************************************************************/
void TN3Provider::service_Manometer(bool enter)
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
 *进入压力计模式控制。
 *************************************************************************************************/
bool TN3Provider::isService_Manometer(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_MANOMETER)
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
 *进入压力操控模式。
 *************************************************************************************************/
void TN3Provider::service_Pressurecontrol(bool enter)
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
 *压力操控模式应答。
 *************************************************************************************************/
bool TN3Provider::isService_Pressurecontrol(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_PRESSURE_CONTROL)
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
 *压力控制（充气）。
 *************************************************************************************************/
void TN3Provider::service_Pressureinflate(short pressure)
{
    unsigned char cmd[2];
    cmd[0] = pressure & 0x00FF;
    cmd[1] = (pressure & 0xFF00) >> 8;
    sendCmd(TN3_CMD_PRESSURE_INFLATE, cmd, 2);
}

/**************************************************************************************************
 *压力控制（充气）应答。
 *************************************************************************************************/
bool TN3Provider::isService_Pressureinflate(unsigned char *packet)
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
void TN3Provider::service_Pressuredeflate()
{
    sendCmd(TN3_CMD_PRESSURE_DEFLATE, NULL, 0);
}

/**************************************************************************************************
 *放气控制应答。
 *************************************************************************************************/
bool TN3Provider::isService_Pressuredeflate(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_PRESSURE_DEFLATE)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *服务模式的压力值。
 *************************************************************************************************/
int TN3Provider::service_cuffPressure(unsigned char *packet)
{
    if (packet[0] != TN3_SERVICE_PRESSURE)
    {
        return -1;
    }

    return ((int)((packet[2] << 8) + packet[1]));
}

/**************************************************************************************************
 *进入校零模式。
 *************************************************************************************************/
void TN3Provider::service_CalibrateZero(bool enter)
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
    sendCmd(TN3_CMD_CALIBRATE_Zero, &cmd, 1);
}

/**************************************************************************************************
 *进入校零模式返回值。
 *************************************************************************************************/
bool TN3Provider::isService_CalibrateZero(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_CALIBRATE_Zero)
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
void TN3Provider::service_PressureZero(void)
{
    sendCmd(TN3_CMD_PRESSURE_Zero, NULL, 0);
}

/**************************************************************************************************
 *校零。
 *************************************************************************************************/
bool TN3Provider::isService_PressureZero(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_PRESSURE_Zero)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *气阀控制。
 *************************************************************************************************/
void TN3Provider::service_Valve(bool enter)
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
    sendCmd(TN3_CMD_Valve, cmd, 2);

    cmd[0] = 0x00;
    if (enter)
    {
        cmd[1] = 0x01;
    }
    else
    {
        cmd[1] = 0x00;
    }
    sendCmd(TN3_CMD_Valve, cmd, 2);
}

/**************************************************************************************************
 *气阀控制应答。
 *************************************************************************************************/
bool TN3Provider::isService_Valve(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_Valve)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *过压保护是否有效。
 *************************************************************************************************/
void TN3Provider::service_PressureProtect(bool enter)
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
    sendCmd(TN3_CMD_PRESSURE_Protect, &cmd, 1);
}

/**************************************************************************************************
 *过压保护是否有效应答。
 *************************************************************************************************/
bool TN3Provider::isService_PressureProtect(unsigned char *packet)
{
    if (packet[0] != TN3_RSP_PRESSURE_Protect)
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
