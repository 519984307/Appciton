/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/5
 **/

#include "S5Provider.h"
#include "SPO2Param.h"
#include "SPO2Alarm.h"
#include "SPO2Define.h"
#include "Debug.h"
#include <QString>
#include "SystemManager.h"
#include "ServiceVersion.h"
#include <sys/time.h>
#include "RawDataCollector.h"
#include "SystemManager.h"
#include "ErrorLogItem.h"
#include "ErrorLog.h"
#include "IConfig.h"
#include "AlarmSourceManager.h"
#include "LanguageManager.h"

#define PROBE_IN_OUT            0x0001
#define FINGER_IN_OUT           0x0002
#define SIGNAL_SATURATION       0x0004
#define SIGNAL_WEAK             0x0008
#define LED_FAULT               0x0010
#define ALGORITHM_MASK          0x0060
#define PULSE_SEARCHING         1
#define PULSE_SEARCH_TOO_LONG   2
#define ALG_NORMAL              3

enum S5StatusType
{
    S5_STATUS_PROBE,                // 探头插入状态
    S5_STATUS_FINGER,               // 手指插入状态
    S5_STATUS_FLASH_GAIN,           // 调光调增益错误
    S5_STATUS_LED,                  // LED fault
    S5_STATUS_ALGORITHM,            // 算法状态
    S5_STATUS_NR,
};


/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool S5Provider::attachParam(Param &param)
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
void S5Provider::handlePacket(unsigned char *data, int len)
{
    if (!isConnectedToParam)
    {
        return;
    }

    BLMProvider::handlePacket(data, len);

    if (!isConnected)
    {
        spo2Param.setConnected(true);
    }

    if ((data[0] == S5_NOTIFY_START_UP) || (data[0] == S5_NOTIFY_STATUS))
    {
        _sendACK(data[0]);
    }

    spo2Param.receivePackage();
    switch (data[0])
    {
    // 灵敏度0x13
    case S5_RSP_SENSITIVITY:
        break;

    // 启动帧0x40
    case S5_NOTIFY_START_UP:
    {
        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName("S5 Start");
        errorLog.append(item);

        spo2Param.reset();
        break;
    }
    // 状态0x42
    case S5_NOTIFY_STATUS:
        isStatus(data);
        break;
    // 描记波、棒图0x5C
    case S5_NOTIFY_WAVE:
        isResult_BAR(data);
        break;

    // SPO2值与PR值0x5F
    case S5_NOTIFY_RESULT:
        isResultSPO2PR(data);
        break;

    // 保活帧0x5B
    case S5_NOTIFY_ALIVE:
        feed();
        break;

    // 原始数据0x5D
    case S5_NOTIFY_DATA:
        rawDataCollector.collectData(RawDataCollector::SPO2_DATA, data, len);
        break;

    // 错误警告帧0x76
    case S5_DATA_ERROR:
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
//                item->setName("S5 Error");
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
void S5Provider::disconnected(void)
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
void S5Provider::reconnected(void)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SPO2);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    }
    spo2Param.setConnected(true);
}

/**************************************************************************************************
 * 获取版本号
 *************************************************************************************************/
void S5Provider::sendVersion()
{
    sendCmd(S5_CMD_GET_VERSION, NULL, 0);
}

/**************************************************************************************************
 * ACK应答
 *************************************************************************************************/
void S5Provider::_sendACK(unsigned char type)
{
    sendCmd(S5_RESPONSE_ACK, &type, 1);
}

/**************************************************************************************************
 * SPO2值与PR值。
 *************************************************************************************************/
bool S5Provider::isResultSPO2PR(unsigned char *packet)
{
//        struct timeval newTime;
//        gettimeofday(&newTime, NULL);
//        int interval = (newTime.tv_sec - _lastTime.tv_sec) * 1000 +
//                (newTime.tv_usec - _lastTime.tv_usec) / 1000;
//        debug("%d",interval);
//        _lastTime = newTime;
//        outHex(packet,6);
    if (packet[0] != S5_NOTIFY_RESULT)
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
    short pr = (packet[2] << 8) + packet[3];
    pr = (pr == -100) ? InvData() : pr;
    spo2Param.setPR(pr);

    return true;
}

/**************************************************************************************************
 * 描记波、棒图。
 *************************************************************************************************/
bool S5Provider::isResult_BAR(unsigned char *packet)
{

    if (packet[0] != S5_NOTIFY_WAVE)
    {
        return false;
    }

    for (int i = 0; i < 10; i++)
    {
        // 波形。
        spo2Param.addWaveformData(packet[i + 5]);
        _isValuePR = (packet[i + 5] == 0x80) ? false : true;
    }
    // 棒图。
    // spo2Param.addBarData((packet[15] == 127) ? 50 : packet[15]);

    // PI;
    short piValue = (packet[15] << 8) + packet[16];
    if (piValue > 200 || piValue < 1)
    {
        piValue = InvData();
    }
    spo2Param.updatePIValue(piValue);

    // 脉搏音。
    spo2Param.setPulseAudio(packet[17]);

    return true;
}

/**************************************************************************************************
 * 状态。
 *************************************************************************************************/
bool S5Provider::isStatus(unsigned char *packet)
{
    // 探头插入
    if (packet[1] & PROBE_IN_OUT)
    {
        spo2Param.setSensorOff(false);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_OFF, false);
    }
    else
    {
        spo2Param.setSensorOff(true);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_OFF, true);
    }

    // 手指插入
    if (packet[1] & FINGER_IN_OUT)
    {
        spo2Param.setNotify(false, trs("SPO2CheckSensor"));
        spo2Param.setValidStatus(true);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, false);
    }
    else
    {
        spo2Param.setNotify(true, trs("SPO2CheckSensor"));
        spo2Param.setValidStatus(false);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
    }

    // 调光调增益

    if (packet[1] & SIGNAL_SATURATION)
    {
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_SATURATION, true);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_WEAK, false);
    }
    else if (packet[1] & SIGNAL_WEAK)
    {
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_WEAK, true);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_SATURATION, false);
    }
    else
    {
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_SATURATION, false);
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SIGNAL_WEAK, false);
    }

    // LED
    if (packet[1] & LED_FAULT)
    {
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LED_FAULT, true);
    }
    else
    {
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LED_FAULT, false);
    }

    // 算法状态
    int algorithmSta = (packet[1] & ALGORITHM_MASK) >> 5;
    if (algorithmSta == PULSE_SEARCHING)
    {
        spo2Param.setSearchForPulse(true);
    }
    else if (algorithmSta == PULSE_SEARCH_TOO_LONG)
    {
        spo2Param.setNotify(true, trs("SPO2PulseSearchTooLong"));
    }
    else if ((algorithmSta == ALG_NORMAL) && !(packet[1] & FINGER_IN_OUT))
    {
        spo2Param.setNotify(false);
    }
    return true;
}

/**************************************************************************************************
 * 设置灵敏度。
 *************************************************************************************************/
void S5Provider::setSensitive(SPO2Sensitive sen)
{
    unsigned char data = sen;
    sendCmd(S5_CMD_SENSITIVITY, &data, 1);
}

/**************************************************************************************************
 * 查询状态。
 *************************************************************************************************/
void S5Provider::sendStatus(void)
{
    sendCmd(S5_CMD_STATUS, NULL, 0);
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void S5Provider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    sendCmd(cmdId, data, len);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
S5Provider::S5Provider() : BLMProvider("BLM_S5"), SPO2ProviderIFace()
{
    disPatchInfo.packetType = DataDispatcher::PACKET_TYPE_SPO2;

    UartAttrDesc portAttr(115200, 8, 'N', 1);
    initPort(portAttr);

    _isCableOff = false;
    _isFingerOff = false;
    _gainError = S5_GAIN_NC;
    _ledFault = false;
    _logicStatus = S5_LOGIC_NC;
    _isValuePR = false;

    if (disPatchInfo.dispatcher)
    {
        // reset the hardware
        disPatchInfo.dispatcher->resetPacketPort(disPatchInfo.packetType);
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
S5Provider::~S5Provider()
{
}

