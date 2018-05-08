#include "BLMNIBPProvider.h"
#include "NIBPParam.h"
#include "Debug.h"
#include <QString>

/**************************************************************************************************
 * 发送复位命令。
 *************************************************************************************************/
void BLMNIBPProvider::_sendReset(void)
{
    unsigned char cmd[8];
    ::memset(cmd, 0, sizeof(cmd));

    // 发送初始压力值。
    cmd[0] = 0x02;
    cmd[3] = ';';
    cmd[4] = ';';
    cmd[7] = 0x03;

    cmd[1] = '1';
    cmd[2] = '6';
    cmd[5] = 'D';
    cmd[6] = 'D';

    writeData(cmd, sizeof(cmd));
}

/**************************************************************************************************
 * 检查数据包的合法性。
 *************************************************************************************************/
bool BLMNIBPProvider::_isInvalid(unsigned char *buff, int len)
{
    if ((buff[len - 2] != _ETX) || (buff[len - 1] != _CR))
    {
        return false;
    }
    return true;
}

/**************************************************************************************************
 * 数据处理。
 *************************************************************************************************/
void BLMNIBPProvider::_handlePacket(unsigned char */*data*/, int /*len*/)
{

}

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool BLMNIBPProvider::attachParam(Param &param)
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
void BLMNIBPProvider::dataArrived(void)
{
    readData();
    if (ringBuff.dataSize() < _minPacketLen)
    {
        return;
    }

    unsigned char buff[64];
    int len = 0;
    while (ringBuff.dataSize() >= _minPacketLen)
    {
        if (ringBuff.at(0) == _STX)
        {
            if (ringBuff.at(1) == '9')            // 测量结束包。
            {
                len = 6;
            }
            else if (ringBuff.at(1) == 'S')       // 测量结果包。
            {
                len = 42;
            }
            else                                  // 压力包。
            {
                len = 10;
            }

            // 数据不够继续等待。
            if (ringBuff.dataSize() < len)
            {
                break;
            }

            for (int i = 0; i < len; i++)
            {
                buff[i] = ringBuff.at(0);
                ringBuff.pop(1);
            }

            // 数据包不合法。
            if (!_isInvalid(buff, len))
            {
                break;
            }

            _handlePacket(buff, len);
        }
        else
        {
            debug("BLMNIBPProvider discard data = 0x%x", (int)ringBuff.at(0));
            ringBuff.pop(1);
        }
    }
}

/**************************************************************************************************
 * 启动测量。
 *************************************************************************************************/
void BLMNIBPProvider::startMeasure(PatientType type)
{
    unsigned char cmd[8];
    ::memset(cmd, 0, sizeof(cmd));

    // 发送初始压力值。
    cmd[0] = 0x02;
    cmd[3] = ';';
    cmd[4] = ';';
    cmd[7] = 0x03;
    if (type == PATIENT_TYPE_ADULT)
    {
        // 成人，初始压150 mmHg
        cmd[1] = '2';
        cmd[2] = '2';
        cmd[5] = 'D';
        cmd[6] = 'A';
    }
    else if (type == PATIENT_TYPE_ADULT)
    {
        // 小儿，初始压140 mmHg
        cmd[1] = '2';
        cmd[2] = '1';
        cmd[5] = 'D';
        cmd[6] = '9';
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        // NEO, 初始压为100 mmHg
        cmd[1] = '1';
        cmd[2] = '9';
        cmd[5] = 'E';
        cmd[6] = '0';
    }
    writeData(cmd, sizeof(cmd));

    // 启动测量。
    cmd[1] = '0';
    cmd[2] = '1';
    cmd[5] = 'D';
    cmd[6] = '7';
    writeData(cmd, sizeof(cmd));
}

/**************************************************************************************************
 * 停止测量。
 *************************************************************************************************/
void BLMNIBPProvider::stopMeasure(void)
{
    unsigned char x = 'X';
    writeData(&x, 1);
}

/**************************************************************************************************
 * 发送启动指令是否有该指令的应答。
 *************************************************************************************************/
bool BLMNIBPProvider::needStartACK(void)
{
    return false;
}

/**************************************************************************************************
 * 是否为启动指令的应答。
 *************************************************************************************************/
bool BLMNIBPProvider::isStartACK(unsigned char */*packet*/, int /*len*/)
{
    return true;
}

/**************************************************************************************************
 * 发送停止指令是否有该指令的应答。
 *************************************************************************************************/
bool BLMNIBPProvider::needStopACK(void)
{
    return false;
}

/**************************************************************************************************
 * 压力数据，不是压力数据返回-1。
 *************************************************************************************************/
int BLMNIBPProvider::cuffPressure(unsigned char *packet, int len)
{
    // 通过长度判断数据包。
    if (len != 10)
    {
        return -1;
    }

    return ((packet[1] - '0') * 100) + ((packet[2] - '0') * 10) + (packet[3] - '0');
}

/**************************************************************************************************
 * 测量是否结束。
 *************************************************************************************************/
bool BLMNIBPProvider::isMeasureDone(unsigned char *packet, int len)
{
    if (len != 6)
    {
        return false;
    }

    return ((packet[1] == '9') && (packet[2] == '9') && (packet[3] == '9'));
}

/**************************************************************************************************
 * 是否为错误数据包。
 *************************************************************************************************/
NIBPOneShotType BLMNIBPProvider::isMeasureError(unsigned char *, int )
{
    return NIBP_ONESHOT_NONE;
}

/**************************************************************************************************
 * 获取结果请求。
 *************************************************************************************************/
void BLMNIBPProvider::getResult(void)
{
    unsigned char cmd[8];
    ::memset(cmd, 0, sizeof(cmd));

    // 发送初始压力值。
    cmd[0] = 0x02;
    cmd[1] = '1';
    cmd[2] = '8';
    cmd[3] = ';';
    cmd[4] = ';';
    cmd[5] = 'D';
    cmd[6] = 'F';
    cmd[7] = 0x03;
    writeData(cmd, sizeof(cmd));
}

/**************************************************************************************************
 * 是否为结果包。
 *************************************************************************************************/
bool BLMNIBPProvider::isResult(unsigned char *packet, int len,
        short &sys, short &dia, short &map, short &pr, NIBPOneShotType &err)
{
    if (len != 42)
    {
        return false;
    }
    err = NIBP_ONESHOT_NONE;

   // 测量有错误，获取错误码。
    if (packet[2] == '2')
    {
        int errCode = ((packet[12] - '0') * 10) + (packet[13] - '0');
        switch (errCode)
        {
        case 1: err = NIBP_ONESHOT_ALARM_HARDWARE_ERROR; break;
        case 6: err = NIBP_ONESHOT_ALARM_CUFF_ERROR; break;
        case 7: err = NIBP_ONESHOT_ALARM_CUFF_AIR_LEAKAGE; break;
        case 8: err = NIBP_ONESHOT_ALARM_PESSURE_ERROR; break;
        case 9: err = NIBP_ONESHOT_ALARM_SIGNAL_WEAK; break;
        case 10: err = NIBP_ONESHOT_ALARM_MEASURE_OVER_RANGE; break;
        case 11: err = NIBP_ONESHOT_ALARM_EXCESSIVE_MOVING; break;
        case 12: err = NIBP_ONESHOT_ALARM_CUFF_OVER_PRESSURE; break;
        case 13: err = NIBP_ONESHOT_ALARM_SIGNAL_SATURATION; break;
        case 19: err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT; break;
        case 20: err = NIBP_ONESHOT_ALARM_CUFF_TYPE_ERROR; break;
        default: break;
        }

        return true;
    }

    // 测量无错，获取测量结果。
    sys = ((packet[16] - '0') * 100) + ((packet[17] - '0') * 10) + (packet[18] - '0');
    dia = ((packet[19] - '0') * 100) + ((packet[20] - '0') * 10) + (packet[21] - '0');
    map = ((packet[22] - '0') * 100) + ((packet[23] - '0') * 10) + (packet[24] - '0');
    pr  = ((packet[27] - '0') * 100) + ((packet[28] - '0') * 10) + (packet[29] - '0');

    return true;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
BLMNIBPProvider::BLMNIBPProvider() : Provider("BLM_NIBP"), NIBPProviderIFace()
{
    UartAttrDesc portAttr(4800, 8, 'N', 1);
    initPort(portAttr);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
BLMNIBPProvider::~BLMNIBPProvider()
{

}
