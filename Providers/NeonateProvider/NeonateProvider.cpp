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
#include "ErrorLogItem.h"
#include "ErrorLog.h"

bool NeonateProvider::attachParam(Param &param)
{
}

NeonateProvider::NeonateProvider() : BLMProvider("NEONATE_O2"), O2ProviderIFace()
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    initPort(portAttr);
}

NeonateProvider::~NeonateProvider()
{
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

void NeonateProvider::sendMotorControl(int status)
{
    unsigned char data = status;
    sendCmd(NEONATE_CMD_MOTOR_CONTROL, &data, 1);
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
        break;
    case NEONATE_RSP_PROBE_MOTOR:
        break;
    case NEONATE_RSP_CALIBRATION:
        o2Param.calibrationResult(data);
        break;
    case NEONATE_RSP_MOTOR_CONTROL:
        break;
    case NEONATE_NOTIFY_START_UP:
    {
        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName("T5 Start");
        errorLog.append(item);
        o2Param.reset();
        break;
    }
    case NEONATE_NOTIFY_PROBE_MOTOR:
        break;
    case NEONATE_CYCLE_ALIVE:
        feed();
        break;
    case NEONATE_CYCLE_O2_DATA:
        o2Param.setO2Concentration(data[1]);
        break;
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
