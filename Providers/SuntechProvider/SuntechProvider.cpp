/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/8
 **/

#include "SuntechProvider.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "Debug.h"
#include <QString>
#include <QTimer>

#define		HOST_START_BYTE           0x3A     // receive packet header :
#define		MODULE_START_BYTE         0x3E     // send packet header >

// RESPONSES

#define		MODULE_ACK     		      0x04      // 模块应答

#define		MODULE_RECEIVED		      0x4F      // “O”
#define		MODULE_EXECUTED		      0x4B      // "K"
#define		MODULE_ABORT		      0x41      // "A"
#define		MODULE_BUSY		          0x42      // "B"
#define		MODULE_ERROR		      0x45      // "E"
#define		MODULE_UNDIFINE	          0x4E      // "N"
#define		MODULE_UNRECOGNIZABLE     0x44      // "D"
#define		MODULE_SLEEP              0x53      // "S"
#define		MODULE_TIMEOUT            0x54      // "T"

#define     SUNTECH_RSP_CUFF_PRESSURE         0x05	    // 当前压力
#define     SUNTECH_RSP_GET_MEASUREMENT       0x18      // 测量结果


// COMMANDS

#define     SUNTECH_CMD_SEND_START              0x79
#define     SUNTECH_CMD_SET_INITIAL_INFIAL      0X17      // DATA=I0
#define     SUNTECH_CMD_START_ADU_BP            0X20      // 成人
#define     SUNTECH_CMD_START_PED_BP            0X87      // 小儿
#define     SUNTECH_CMD_START_NEO_BP            0X28      // 新生儿
#define     SUNTECH_CMD_ABORT_BP                0x01      // 停止测量
#define     SUNTECH_CMD_GET_CUFF_PRESSURE       0x05      // 获取当前压力
#define     SUNTECH_CMD_GET_BP_DATA             0x03      // 获取测量结果
#define     SUNTECH_CMD_GET_MODULE_DATA         0x00
#define     SUNTECH_CMD_GET_RETURN_STRING       0x02      // DATA=0x02 0x40
#define     SUNTECH_CMD_SET_SLEEP_MODE          0x81      // DATA=B0 B1 B2 B3
#define     SUNTECH_CMD_CONTROL_PNEUMATICS      0x0C      // DATA=B0 B1 B2
#define     SUNTECH_CMD_CALIBRATE_TRANSDUCER    0X04      // DATA=B0
#define     SUNTECH_CMD_GET_RETURN_CODE         0x79      // DATA=0x02 0x03
#define     SUNTECH_CMD_STATUS                  0x79      // DATA=0x10 0x03
#define     SUNTECH_CMD_RESET                   0x8A
#define     SUNTECH_CMD_START_NEONATE_BP        0x28      // 新生儿
#define     SUNTECH_CMD_VENOUS_STASIS           0x86      // DATA=B0 B1 B2

/**************************************************************************************************
 * 发送复位命令。
 *************************************************************************************************/
void SuntechProvider::_sendReset(void)
{
    unsigned char cmd[1] = {0};
    cmd[0] = 0x8A;
    _sendCmd(cmd, 1);
}

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool SuntechProvider::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_NIBP)
    {
        nibpParam.setProvider(this);
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void SuntechProvider::dataArrived(void)
{
    readData();
    if (ringBuff.dataSize() < _minPacketLen)
    {
        return;
    }

    unsigned char buff[64];
    while (ringBuff.dataSize() >= _minPacketLen)
    {
        if (ringBuff.at(0) != MODULE_START_BYTE)
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

        if (len > ringBuff.dataSize()) // 数据还不够，继续等待。
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

        if (buff[len - 1] == _calcCheckSum(buff, len - 1))
        {
            _handlePacket(&buff[1], len - 2);
        }
        else
        {
            outHex(buff, len);
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

/**************************************************************************************************
 * 启动测量。
 *************************************************************************************************/
void SuntechProvider::startMeasure(PatientType type)
{
    unsigned char cmd = 0;
    if (type == PATIENT_TYPE_ADULT)	 //如果是成人模式
    {
        cmd = SUNTECH_CMD_START_ADU_BP;
    }
    else if (type == PATIENT_TYPE_PED) //如果是少儿模式
    {
        cmd = SUNTECH_CMD_START_PED_BP;
    }
    else if (type == PATIENT_TYPE_NEO) //如果是新生儿模式
    {
        cmd = SUNTECH_CMD_START_NEO_BP;
    }

    _sendCmd(&cmd, 1);

//    _timer->start();
    _getCuffPressure();
    _NIBPStart = true;
    _flagStartCmdSend = 1;
}

/**************************************************************************************************
 * 停止测量。
 *************************************************************************************************/
void SuntechProvider::stopMeasure(void)
{
//    _timer->stop();

    unsigned char cmd[3] = {0};

    cmd[0] = SUNTECH_CMD_SEND_START;
    cmd[1] = SUNTECH_CMD_ABORT_BP;
    cmd[2] = 0x00;

    _sendCmd(cmd, 3);

    _flagStartCmdSend = 1;
}

/**************************************************************************************************
 * 设置初始化压力。
 *************************************************************************************************/
void SuntechProvider::setInitPressure(short pressure)
{
    unsigned char cmd[3] = {0};

    cmd[0] = 0x17;
    cmd[1] = pressure & 0xFF;
    cmd[2] = (pressure & 0xFF00) >> 8;

    _sendCmd(cmd, 3);
}

/**************************************************************************************************
 * 发送启动指令是否有该指令的应答。
 *************************************************************************************************/
bool SuntechProvider::needStartACK(void)
{
    return true;
}

/**************************************************************************************************
 * 发送停止指令是否有该指令的应答。
 *************************************************************************************************/
bool SuntechProvider::needStopACK(void)
{
    return true;
}

void SuntechProvider::servicePressurepoint(const unsigned char *data, unsigned int len)
{
    unsigned char cmd[2] = {0};
    cmd[0] = 0x04;
    int pressure = data[0] | (data[1] << 8);
    if (pressure)
    {
        cmd[1] = 0x01;
    }
    else
    {
        cmd[1] = 0x00;
    }
    len = 2;
    _sendCmd(cmd, len);
}

/**************************************************************************************************
 * 是否为错误数据包。
 *************************************************************************************************/
NIBPOneShotType SuntechProvider::isMeasureError(unsigned char *)
{
    return NIBP_ONESHOT_NONE;
}

/**************************************************************************************************
 * 获取结果请求。
 *************************************************************************************************/
void SuntechProvider::getResult(void)
{
    unsigned char cmd[3] = {0};

    cmd[0] = SUNTECH_CMD_SEND_START;
    cmd[1] = SUNTECH_CMD_GET_BP_DATA;
    cmd[2] = 0x00;

    _sendCmd(cmd, 3);
}

/**************************************************************************************************
 * 是否为结果包。
 *************************************************************************************************/
bool SuntechProvider::isResult(unsigned char *packet,
                               short &sys, short &dia, short &map, short &pr, NIBPOneShotType &err)
{
//    _timer->stop();

    if (packet[0] != 0x18)
    {
        return false;
    }

    err = NIBP_ONESHOT_NONE;

    // 测量有错误，获取错误码。
    if (packet[19] != 0x00)
    {
        switch (packet[19])
        {
        case 0x01:
            err = NIBP_ONESHOT_ALARM_SIGNAL_WEAK;
            break;
        case 0x02:
            err = NIBP_ONESHOT_ALARM_EXCESSIVE_MOVING;
            break;
        case 0x03:
            err = NIBP_ONESHOT_ALARM_MEASURE_OVER_RANGE;
            break;
        case 0x04:
            err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
            break;
        case 0x85:
            err = NIBP_ONESHOT_ALARM_PNEUMATIC_BLOCKAGE;
            break;
        case 0x87:
            err = NIBP_ONESHOT_ALARM_CUFF_ERROR;
            break;
        case 0x88:
            err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
            break;
        case 0x89:
            err = NIBP_ONESHOT_ALARM_CUFF_OVER_PRESSURE;
            break;
        case 0x90:
            err = NIBP_ONESHOT_ALARM_HARDWARE_ERROR;
            break;
        case 0x91:
            err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
            break;
        case 0x97:
            err = NIBP_ONESHOT_ALARM_TRANSDUCER_OVER_RANGE;
            break;
        case 0x99:
            err = NIBP_ONESHOT_ALARM_EEPROM_FAILURE;
            break;
        default:
            break;
        }
        return true;
    }

    sys = static_cast<int>((packet[2] << 8) + packet[1]);
    dia = static_cast<int>((packet[4] << 8) + packet[3]);
    map = static_cast<int>((packet[18] << 8) + packet[17]);
    pr = static_cast<int>((packet[16] << 8) + packet[15]);

    return true;
}
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SuntechProvider::SuntechProvider() : Provider("SUNTECH_NIBP"), NIBPProviderIFace()
{
    _flagStartCmdSend = -1;

    _NIBPStart = false;

    UartAttrDesc portAttr(9600, 8, 'N', 1);
    initPort(portAttr);
    setDisconnectThreshold(1);

    _timer = new QTimer();
    _timer->setInterval(200);
    _timer->start();
    connect(_timer, SIGNAL(timeout()), this, SLOT(_getCuffPressure()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SuntechProvider::~SuntechProvider()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

/**************************************************************************************************
 * get cuff pressure。
 *************************************************************************************************/
void SuntechProvider::_getCuffPressure()
{
    unsigned char cmd[3] = {0};

    cmd[0] = SUNTECH_CMD_SEND_START;
    cmd[1] = SUNTECH_CMD_GET_CUFF_PRESSURE;
    cmd[2] = 0x00;

    _sendCmd(cmd, 3);
}

static NIBPMeasureResultInfo getMeasureResultInfo(unsigned char *data)
{
    NIBPMeasureResultInfo info;
    short t = static_cast<short>(data[0] + (data[1] << 8));
    info.sys = t;
    t = static_cast<short>(data[2] + (data[3] << 8));
    info.dia = t;
    t = static_cast<short>(data[16] + (data[17] << 8));
    info.map = t;
    t = static_cast<short>(data[14] + (data[15] << 8));
    info.pr = t;
    t = static_cast<short>(data[18]);
    info.errCode = t;
    return info;
}

/**************************************************************************************************
 * 数据处理。
 *************************************************************************************************/
void SuntechProvider::_handlePacket(unsigned char *data, int len)
{
    switch (data[0])
    {
    case MODULE_ACK:
        // "O"
        if (data[1] == MODULE_RECEIVED)
        {
            if (_flagStartCmdSend == 1)
            {
                // 启动测量
                if (_NIBPStart)
                {
                    nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_START_MEASURE, NULL, 0);
                }
                _flagStartCmdSend = 2;
            }
        }

        // "A"
        else if (data[1] == MODULE_ABORT)
        {
            if (_flagStartCmdSend == 1)
            {
                if (_NIBPStart)
                {
                    _NIBPStart = false;
                }
                _flagStartCmdSend = 2;
            }
        }

        // "AK"  "OK"
        else if (data[1] == MODULE_EXECUTED)
        {
            if (_flagStartCmdSend == 2)
            {
                _flagStartCmdSend = 0;
                // 测量完成
                if (_NIBPStart)
                {
                    nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_MEASURE_DONE, NULL, 0);
                    _NIBPStart = false;
                }
                // 测量停止
                else
                {
                    nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_STOP, NULL, 0);
                }
            }
        }
        break;

    // 当前压力
    case SUNTECH_RSP_CUFF_PRESSURE:
        nibpParam.handleNIBPEvent(NIBP_EVENT_CURRENT_PRESSURE, &data[1], 2);
        break;
    // 测量结果
    case SUNTECH_RSP_GET_MEASUREMENT:
    {
        NIBPMeasureResultInfo info = getMeasureResultInfo(&data[1]);
        nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_GET_RESULT, reinterpret_cast<unsigned char *>(&info), sizeof(info));
        break;
    }

    default:
        break;
    }
}

void SuntechProvider::_sendCmd(const unsigned char *data, unsigned int len)
{
    int index = 0;
    unsigned char sendBuf[10] = {0};

    sendBuf[index++] = HOST_START_BYTE;
    for (unsigned int i = 0; i < len; i++)
    {
        sendBuf[index++] = data[i];
    }

    sendBuf[index++] = _calcCheckSum(sendBuf, len + 1);

    writeData(sendBuf, len + 2);
}

/**************************************************************************************************
 * calculate check sum.
 *************************************************************************************************/
unsigned char SuntechProvider::_calcCheckSum(const unsigned char *data, int len)
{
    unsigned char sum = 0x00;
    for (int i = 0; i < len; i++)
    {
        sum = sum + data[i];
    }
    sum = 0x00 - sum;
    return sum;
}
