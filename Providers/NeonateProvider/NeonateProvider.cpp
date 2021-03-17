/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/13
 **/

#include "NeonateProvider.h"
#include "O2Param.h"
#include "Framework/ErrorLog/ErrorLogItem.h"
#include "Framework/ErrorLog/ErrorLog.h"
#include "O2Alarm.h"
#include "O2Define.h"
#include "SystemManager.h"
#include "AlarmSourceManager.h"

#define MOTOR_INTERMITTENT_VIBRATE_TIME  (10*1000)   // 10s Motor intermittent vibration time

class NeonateProviderPrivate
{
public:
    NeonateProviderPrivate()
        : motorControlTimer(), motorVibrateState(false), mototControlStatus(false)
    {}

    ~NeonateProviderPrivate()
    {}

    QTimer motorControlTimer;   // Control motor timer
    bool motorVibrateState;     // Used to control the intermittent vibration function of the motor.
    bool mototControlStatus;    // Control whether the motor starts or stops the vibration function.
};

bool NeonateProvider::attachParam(Param *param)
{
    if (param->getParamID() == PARAM_O2)
    {
        o2Param.setProvider(this);
        Provider::attachParam(param);
        return true;
    }
    return false;
}

NeonateProvider::NeonateProvider() : BLMProvider("NEONATE_O2"), O2ProviderIFace()
  , d_ptr(new NeonateProviderPrivate())
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    initPort(portAttr);

    // create timer
    d_ptr->motorControlTimer.setInterval(MOTOR_INTERMITTENT_VIBRATE_TIME);    // 10s
    connect(&d_ptr->motorControlTimer, SIGNAL(timeout()), this, SLOT(_motorVibrateControl()));
}

NeonateProvider::~NeonateProvider()
{
    delete d_ptr;
}

void NeonateProvider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    sendCmd(cmdId, data, len);
}

void NeonateProvider::sendVersion()
{
    sendCmd(NEONATE_CMD_VERSION, NULL, 0);
}

void NeonateProvider::sendO2SelfTest()
{
    sendCmd(NEONATE_CMD_SELF_STATUS, NULL, 0);
}

void NeonateProvider::sendProbeState()
{
    sendCmd(NEONATE_CMD_PROBE_MOTOR, NULL, 0);
}

void NeonateProvider::sendCalibration(int concentration)
{
    unsigned char data = concentration;
    sendCmd(NEONATE_CMD_CALIBRATION, &data, 1);
}

void NeonateProvider::sendMotorControl(bool status)
{
    if (d_ptr->mototControlStatus == status)
    {
        return;
    }
    if (status)
    {
        d_ptr->motorControlTimer.start();
    }
    else
    {
        d_ptr->motorControlTimer.stop();
    }
    unsigned char data = status;
    sendCmd(NEONATE_CMD_MOTOR_CONTROL, &data, 1);
    d_ptr->mototControlStatus = status;
    d_ptr->motorVibrateState = status;
}

void NeonateProvider::sendVibrationIntensity(int intensity)
{
    unsigned char data = intensity;
    sendCmd(NEONATE_CMD_VIBRATION_INTENSITY, &data, 1);
}

void NeonateProvider::sendACK(unsigned char type)
{
    unsigned char data = type;
    sendCmd(NEONATE_RESPONSE_ACK, &data, 1);
}

void NeonateProvider::handlePacket(unsigned char *data, int len)
{
    if (!isConnectedToParam)
    {
        return;
    }
    BLMProvider::handlePacket(data, len);

    sendACK(data[0]);
    switch (data[0])
    {
    case NEONATE_RSP_VERSION:
        break;
    case NEONATE_RSP_SELF_STATUS:
        _selfTest(data, len);
        break;
    case NEONATE_RSP_PROBE_MOTOR:
    {
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_O2);
        if (alarmSource)
        {
            alarmSource->setOneShotAlarm(O2_ONESHOT_ALARM_MOTOR_NOT_IN_POSITION, !data[1]);
            alarmSource->setOneShotAlarm(O2_ONESHOT_ALARM_SENSOR_OFF, !data[2]);
        }
        break;
    }
    case NEONATE_RSP_CALIBRATION:
        o2Param.calibrationResult(data);
        break;
    case NEONATE_RSP_MOTOR_CONTROL:
        break;
    case NEONATE_NOTIFY_START_UP:
    {
        ErrorLogItem *item = new ErrorLogItem();
        item->setName("T5 Start");
        errorLog.append(item);
        o2Param.reset();
        break;
    }
    case NEONATE_NOTIFY_PROBE_MOTOR:
    {
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_O2);
        if (alarmSource)
        {
            alarmSource->setOneShotAlarm(O2_ONESHOT_ALARM_MOTOR_NOT_IN_POSITION, !data[1]);
            alarmSource->setOneShotAlarm(O2_ONESHOT_ALARM_SENSOR_OFF, !data[2]);
        }
        break;
    }
    case NEONATE_CYCLE_ALIVE:
        feed();
        break;
    case NEONATE_CYCLE_O2_DATA:
    {
        int16_t o2 = data[1] | (data[2] << 8);
        if (o2 < 0)
        {
            o2Param.setO2Concentration(InvData());
        }
        else
        {
            o2Param.setO2Concentration(o2);
        }
        break;
    }
    case NEONATE_CYCLE_AD_DATA:
        break;
    case NEONATE_DATA_ERROR:
        break;
    default:
        break;
    }
}

void NeonateProvider::disconnected()
{
}

void NeonateProvider::reconnected()
{
}

void NeonateProvider::sendDisconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_O2);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(O2_ONESHOT_ALARM_SEND_COMMUNICATION_STOP, true);
    }
}

void NeonateProvider::_motorVibrateControl()
{
    // Control the motor to start vibrating or stop vibrating for 10s.
    unsigned char data = (!d_ptr->motorVibrateState);
    sendCmd(NEONATE_CMD_MOTOR_CONTROL, &data, 1);
    d_ptr->motorVibrateState = (!d_ptr->motorVibrateState);
}

void NeonateProvider::_selfTest(unsigned char *packet, int len)
{
    int num = packet[1];
    if (num > 0)
    {
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_O2);
        if (alarmSource == NULL)
        {
            return;
        }
        for (int i = 2; i < len; i++)
        {
            switch (packet[i])
            {
            case 0x01:
                alarmSource->setOneShotAlarm(O2_ONESHOT_ALARM_CALIBRATE_RESET, true);
                break;
            case 0x02:
                alarmSource->setOneShotAlarm(O2_ONESHOT_ALARM_NOT_CALIBRATE, true);
                break;
            default:
                break;
            }
        }
    }
    else if (num == 0)
    {
        systemManager.setPoweronTestResult(O2_MODULE_SELFTEST_RESULT, SELFTEST_SUCCESS);
    }
}
