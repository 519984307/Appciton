#include "SuntechProvider.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "Debug.h"
#include <QString>
#include <QTimer>

#define ROH             (0x3E)  // receive packet header
#define SOH             (0x3A)  // send packet header

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
        if (ringBuff.at(0) != ROH)
        {
            //debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
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
        if (len > (int)sizeof(buff))
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

        outHex(buff, len);

        if (buff[len-1] == _calcCheckSum(buff,len-1))
        {
            nibpParam.unPacket(&buff[1], len - 2);
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
    unsigned char cmd;
    if (type == PATIENT_TYPE_ADULT)	 //如果是成人模式
    {
      cmd = 0x20;
    }
    else if(type == PATIENT_TYPE_PED) //如果是少儿模式
    {
      cmd = 0x87;
    }
    else if(type == PATIENT_TYPE_NEO) //如果是新生儿模式
    {
      cmd = 0x28;
    }

    _sendCmd(&cmd, 1);

//    _timer->start();
    _getCuffPressure();
}

/**************************************************************************************************
 * 停止测量。
 *************************************************************************************************/
void SuntechProvider::stopMeasure(void)
{
//    _timer->stop();

    unsigned char cmd[3] = {0};

    cmd[0] = 0x79;
    cmd[1] = 0x01;
    cmd[2] = 0x00;

    _sendCmd(cmd,3);
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
 * 是否为启动指令的应答。
 *************************************************************************************************/
bool SuntechProvider::isStartACK(unsigned char *packet)
{
    if (packet[0] != 0x04)
    {
        return false;
    }

    if (packet[1] != 0x4F)
    {
        return false;
    }

    _NIBPStart = true;

    return true;
}

/**************************************************************************************************
 * 发送停止指令是否有该指令的应答。
 *************************************************************************************************/
bool SuntechProvider::needStopACK(void)
{
    return true;
}

/**************************************************************************************************
 * 是否为停止指令的应答。
 *************************************************************************************************/
bool SuntechProvider::isStopACK(unsigned char *packet)
{
    if (packet[0] != 0x04)
    {
        return false;
    }

    if (packet[1] != 0x41)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 * 压力数据，不是压力数据返回-1。
 *************************************************************************************************/
short SuntechProvider::cuffPressure(unsigned char *packet)
{
    outHex(packet,3);
    if (packet[0] != 0x05)
    {
        return -1;
    }

    short pressure = (packet[2]<<8)+packet[1];

    return pressure;
}

/**************************************************************************************************
 * 测量是否结束。
 *************************************************************************************************/
bool SuntechProvider::isMeasureDone(unsigned char *packet)
{
    if (packet[0] != 0x04)
    {
        return false;
    }

    if (packet[1] != 0x4B)
    {
        return false;
    }

    // received first data packet acknowledges the START_BP command
    if (_NIBPStart)
    {
        _NIBPStart = false;
        return true;
    }

    return false;
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
    dbg;
    unsigned char cmd[3] = {0};

    cmd[0] = 0x79;
    cmd[1] = 0x03;
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
        case 0x01: err = NIBP_ONESHOT_ALARM_SIGNAL_WEAK; break;
        case 0x02: err = NIBP_ONESHOT_ALARM_EXCESSIVE_MOVING; break;
        case 0x03: err = NIBP_ONESHOT_ALARM_MEASURE_OVER_RANGE; break;
        case 0x04: err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT; break;
        case 0x85: err = NIBP_ONESHOT_ALARM_PNEUMATIC_BLOCKAGE; break;
        case 0x87: err = NIBP_ONESHOT_ALARM_CUFF_ERROR; break;
        case 0x88: err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT; break;
        case 0x89: err = NIBP_ONESHOT_ALARM_CUFF_OVER_PRESSURE; break;
        case 0x90: err = NIBP_ONESHOT_ALARM_HARDWARE_ERROR; break;
        case 0x91: err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT; break;
        case 0x97: err = NIBP_ONESHOT_ALARM_TRANSDUCER_OVER_RANGE; break;
        case 0x99: err = NIBP_ONESHOT_ALARM_EEPROM_FAILURE; break;
        default: break;
        }
        return true;
    }

    sys = (int)((packet[2]<<8)+packet[1]);
    dia = (int)((packet[4]<<8)+packet[3]);
    map = (int)((packet[18]<<8)+packet[17]);
    pr = (int)((packet[16]<<8)+packet[15]);

    return true;
}
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SuntechProvider::SuntechProvider() : Provider("SUNTECH_NIBP"), NIBPProviderIFace()
{
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

    cmd[0] = 0x79;
    cmd[1] = 0x05;
    cmd[2] = 0x00;

    _sendCmd(cmd, 3);
}

void SuntechProvider::_sendCmd(const unsigned char *data, unsigned int len)
{
    int index = 0;
    unsigned char sendBuf[10] = {0};

    sendBuf[index++] = SOH;
    for (unsigned int i = 0; i < len; i++)
    {
        sendBuf[index++] = data[i];
    }

    sendBuf[index++] = _calcCheckSum(sendBuf,len + 1);

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
