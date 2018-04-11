#include "TS3Provider.h"
#include "SPO2Param.h"
#include "SPO2Alarm.h"
#include "SPO2Define.h"
#include "Debug.h"
#include <QString>
#include "SystemManager.h"
#include "BLMEDUpgradeParam.h"
#include "ServiceVersion.h"
#include <sys/time.h>
#include "RawDataCollection.h"
#include "SystemManager.h"
#include "ErrorLogItem.h"
#include "ErrorLog.h"
#include "IConfig.h"

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool TS3Provider::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_SPO2)
    {
        spo2Param.setProvider(this);
        return true;
    }
    return false;
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void TS3Provider::handlePacket(unsigned char *data, int len)
{
    BLMProvider::handlePacket(data, len);

    if ((data[0] == TS3_NOTIFY_START_UP) || (data[0] == TS3_NOTIFY_STATUS))
    {
        _sendACK(data[0]);
    }

    spo2Param.receivePackage();
    int enable = 0;
    switch(data[0])
    {
    //灵敏度0x13
    case TS3_RSP_SENSITIVITY:
        break;

        //启动帧0x40
    case TS3_NOTIFY_START_UP:
    {
        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName("TS3 Start");
        errorLog.append(item);

        spo2Param.reset();
        break;
    }
        //状态0x42
    case TS3_NOTIFY_STATUS:
        isStatus(data);
        break;
        //描记波、棒图0x5C
    case TS3_NOTIFY_WAVE:
        isResult_BAR(data);
        break;

        //SPO2值与PR值0x5F
    case TS3_NOTIFY_RESULT:
        isResultSPO2PR(data);
        break;

        //保活帧0x5B
    case TS3_NOTIFY_ALIVE:
        feed();
        break;

        //原始数据0x5D
    case TS3_NOTIFY_DATA:
        machineConfig.getNumValue("Record|SPO2", enable);
        if (enable)
        {
            rawDataCollection.pushData("BLM_TS3", data,len);
        }
        break;

        //错误警告帧0x76
    case TS3_DATA_ERROR:
    {
        _sendACK(data[0]);
//            QString errStr("");
//            if (data[1] & 0x01)
//            {
//                errStr = "LED Fault.";
//            }
//            else if (data[1] & 0x02)
//            {
//                errStr = "AD Collection Module is Fault.";
//            }

//            if (!errStr.isEmpty())
//            {
//                ErrorLogItem *item = new CriticalFaultLogItem();
//                item->setName("TS3 Error");
//                item->setLog(errStr);
//                errorLog.append(item);
//            }
        break;
    }

    default:
        break;
    }
}

/**************************************************************************************************
 * 连接中断。
 *************************************************************************************************/
void TS3Provider::disconnected(void)
{
    spo2OneShotAlarm.clear();
    spo2OneShotAlarm.setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, true);
}

/**************************************************************************************************
 * 连接恢复。
 *************************************************************************************************/
void TS3Provider::reconnected(void)
{
    spo2OneShotAlarm.setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, false);
}

/**************************************************************************************************
 * 获取版本号
 *************************************************************************************************/
void TS3Provider::sendVersion()
{
    sendCmd(TS3_CMD_GET_VERSION, NULL, 0);
}

/**************************************************************************************************
 * ACK应答
 *************************************************************************************************/
void TS3Provider::_sendACK(unsigned char type)
{
    sendCmd(TS3_RESPONSE_ACK, &type, 1);
}

/**************************************************************************************************
 * SPO2值与PR值。
 *************************************************************************************************/
bool TS3Provider::isResultSPO2PR(unsigned char *packet)
{
//        struct timeval newTime;
//        gettimeofday(&newTime, NULL);
//        int interval = (newTime.tv_sec - _lastTime.tv_sec) * 1000 +
//                (newTime.tv_usec - _lastTime.tv_usec) / 1000;
//        debug("%d",interval);
//        _lastTime = newTime;
//        outHex(packet,6);
    if (packet[0] != TS3_NOTIFY_RESULT)
    {
        return false;
    }

    // 血氧值。
    //    if (_isValuePR)
    //        {
    //    spo2Param.setSPO2(packet[2]);
    //    }
    //    else
    //        {
    //        spo2Param.setSPO2(InvData());
    //    }
    if (packet[1] == 255)
    {
        spo2Param.setSPO2(InvData());
    }
    else
    {
        spo2Param.setSPO2(packet[1]);
    }

    // 脉率值。
    short pr = (packet[2]<<8)+packet[3];
    pr = (pr == -100) ? InvData() : pr;
    spo2Param.setPR(pr);

    return true;
}

/**************************************************************************************************
 * 描记波、棒图。
 *************************************************************************************************/
bool TS3Provider::isResult_BAR(unsigned char *packet)
{

    if (packet[0] != TS3_NOTIFY_WAVE)
    {
        return false;
    }

    for(int i = 0; i < 10; i++)
    {
        // 波形。
#if 0 //TODO: reenable in future
        if(i == 2 && dataSync.needDrop(WAVE_SPO2))
        {
            qDebug("drop one spo2 sample.");
            break;
        }
        else
#endif
        {
            spo2Param.addWaveformData(packet[i + 5]);
            _isValuePR = (packet[i + 5] == 0x80) ? false : true;
        }
    }
    // 棒图。
    spo2Param.addBarData((packet[15] == 127) ? 50 : packet[15]);

    // 脉搏音。
    spo2Param.setPulseAudio(packet[16]);

    return true;
}

/**************************************************************************************************
 * 状态。
 *************************************************************************************************/
bool TS3Provider::isStatus(unsigned char *packet)
{
//    bool isValid = false;
//    // 探头连接状态
//    if (packet[1] & 0x01)
//    {
//        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
//        spo2Param.setSensorOff(true);
//        isValid |= true;
//    }
//    else
//    {
//        spo2Param.setSensorOff(false);
//        //探头故障
//        if (packet[1] & 0x40)
//        {
//            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
//            isValid |= true;
//        }
//        else
//        {
//            // 手指状态
//            if (packet[1] & 0x02)
//            {
//                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
//                isValid |= true;
//            }
//            else
//            {
//                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, false);
//            }
//        }
//    }

//    if (packet[1] & 0x01 || packet[1] & 0x02 || packet[1] & 0x40)
//    {
//        if (packet[1] & 0x02)
//        {
//            spo2Param.setNotify(true,trs("SPO2CheckSensor"));
//        }
//        else
//        {
//            spo2Param.setNotify(false);
//        }

//        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SEARCHING_TOO_LONG, false);
//    }
//    else
//    {
//        // 脉搏搜索
//        if (packet[1] & 0x04)
//        {
//            spo2Param.setSearchForPulse(true);
//            isValid |= true;
//        }
//        else
//        {
//            spo2Param.setSearchForPulse(false);
//        }
//        //搜索时间过长
//        if (packet[1] & 0x20)
//        {
//            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SEARCHING_TOO_LONG, true);
////            isValid |= true;
//        }
//        else
//        {
//            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SEARCHING_TOO_LONG, false);
//        }
//    }

//    //SPO2突然降低
//    if (packet[1] & 0x08)
//    {
//    }
//    else
//    {
//    }

//    //信号饱和
//    if (packet[1] & 0x10)
//    {
//    }
//    else
//    {
//    }

//    spo2Param.setValidStatus(!isValid);
//    return true;

    bool isValid = false;
    bool isError = false;
    switch(packet[1])
    {
    case 0x00:
        if (packet[2] == 0x01)
        {
            _isCableOff = false;
        }
        else
        {
            _isCableOff = true;
        }
        break;

    case 0x01:
        if (packet[2] == 0x01)
        {
            _isFingerOff = false;
        }
        else
        {
            _isFingerOff = true;
        }
        break;

    case 0x02:
        if (packet[2] == 0x00)
        {
            _gainError = TS3_GAIN_NORMAL;
        }
        else
        {
            if (packet[2] == 0x01)
            {
                _gainError = TS3_GAIN_SATURATION;
            }
            else if (packet[2] == 0x02)
            {
                _gainError = TS3_GAIN_WEAK;
            }
        }
        break;

    case 0x03:
        if (packet[2] == 0x00)
        {
            _ledFault = false;
        }
        else
        {
            _ledFault = true;
        }
        break;

    case 0x04:
        isError = true;
        if (packet[2] == 0x00)
        {
            _logicStatus = TS3_LOGIC_INIT;
        }
        else if (packet[2] == 0x01)
        {
            _logicStatus = TS3_LOGIC_SEARCHING;
        }
        else if (packet[2] == 0x02)
        {
            _logicStatus = TS3_LOGIC_SEARCH_TOO_LONG;
        }
        else if (packet[2] == 0x03)
        {
            _logicStatus = TS3_LOGIC_NORMAL;
        }
        break;

    default:
        break;
    }

    if (isError)
    {
        _isCableOff = false;
        _isFingerOff = false;
        _gainError = TS3_GAIN_NORMAL;
        _ledFault = false;

        spo2Param.setSensorOff(false);

        // 初始化
        if (_logicStatus == TS3_LOGIC_INIT)
        {
            spo2Param.setNotify(true,trs("SPO2Initializing"));
        }
        // 脉搏搜索
        else if (_logicStatus == TS3_LOGIC_SEARCHING)
        {
            spo2Param.setSearchForPulse(true);
        }
        //搜索时间过长
        else if (_logicStatus == TS3_LOGIC_SEARCH_TOO_LONG)
        {
            spo2Param.setNotify(true,trs("SPO2PulseSearch"));
        }
        else
        {
            spo2Param.setNotify(false);
        }
    }
    else
    {
        if (_isCableOff)
        {
            spo2Param.setSensorOff(true);
        }
        else
        {
            spo2Param.setSensorOff(false);
        }
        if (_isCableOff || _isFingerOff || _ledFault)
        {
            spo2Param.setNotify(true,trs("SPO2CheckSensor"));
            if (_ledFault)
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LED_FAULT, true);
            }
            else
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
            }
            isValid |= true;
        }
        else
        {
            spo2Param.setNotify(false);
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LED_FAULT, false);
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, false);
            if (_gainError == TS3_GAIN_SATURATION)
            {
                isValid |= true;
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_SATURATION, true);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_WEAK, false);
            }
            else if (_gainError == TS3_GAIN_WEAK)
            {
                isValid |= true;
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_WEAK, true);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_SATURATION, false);
            }
            else
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_SATURATION, false);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_WEAK, false);
            }
        }
    }

    spo2Param.setValidStatus(!isValid);
    return true;
}

/**************************************************************************************************
 * 设置灵敏度。
 *************************************************************************************************/
void TS3Provider::setSensitive(SPO2Sensitive sen)
{
    unsigned char data = sen;
    sendCmd(TS3_CMD_SENSITIVITY, &data, 1);
}

/**************************************************************************************************
 * 查询状态。
 *************************************************************************************************/
void TS3Provider::sendStatus(void)
{
    sendCmd(TS3_CMD_STATUS, NULL, 0);
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void TS3Provider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    sendCmd(cmdId, data, len);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TS3Provider::TS3Provider() : BLMProvider("BLM_TS3"), SPO2ProviderIFace()
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    initPort(portAttr);

    _isCableOff = false;
    _isFingerOff = false;
    _gainError = TS3_GAIN_NC;
    _ledFault = false;
    _logicStatus = TS3_LOGIC_NC;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TS3Provider::~TS3Provider()
{

}

