/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/12/14
 **/

#include "RainbowProvider.h"
#include "SPO2Param.h"
#include "Debug.h"
#include "SPO2Alarm.h"
#include <QTimer>

#define SOM  (0xA1)
#define EOM  (0xAF)
#define DEFALUT_BAUD_RATE  (9600)
#define MIN_PACKET_LEN  (5)
#define TEMP_BUFF_SIZE  (64)
#define MANU_ID    (0x23ae5d53)

enum RBRecvPacketType
{
    RB_ACK                           = 0X01,     // ack response
    RB_NCK                           = 0X02,     // nck response
    RB_PARAM                         = 0X10,     // parameter packet
    RB_WAVEFORM                      = 0X11,     // waveform packewt
    RB_ENTER_PROGRAM_MODE            = 0X20,     // resquest to enter programming mode
    RB_PROGRAM_COMPLETE              = 0X21,     // programming complete
    RB_PROGRAM_ERROR                 = 0X22,     // programming error
    RB_BOARD_INFO                    = 0X70,     // board info
    RB_PLETH_CONTROL                 = 0X90,     // pleth control
};

enum RBSendPacketType
{
    RB_CMD_REQUEST_BOARD_INFO        = 0x70,     // request board info
    RB_CMD_UNLOCK_BOARD              = 0x71,     // unlock board
    RB_CMD_CONF_AVERAGE_TIME         = 0x01,     // configure average time
    RB_CMD_CONF_SENSITIVITY_MODE     = 0x02,     // configure sensitivity mode
    RB_CMD_CONF_FASTSAT_MODE         = 0x03,     // configure fastsat mode
    RB_CMD_CONF_LINE_FREQ            = 0x04,     // configure line frequency
    RB_CMD_CONF_SMART_TONE_MODE      = 0x08,     // configure smart tone mode
    RB_CMD_CONF_WAVEFORM_MODE        = 0x09,     // configure waveform mode
    RB_CMD_CONF_PERIOD_PARAM_OUTPUT  = 0x20,     // configure periodic parameter output
    RB_CMD_CONF_PERIOD_WAVE_OUTPUT   = 0x21,     // configure periodic waveform output
};

enum RBParamIDType
{
    RB_PARAM_OF_SPO2                 = 0X01,     // spo2 value and exceptions
    RB_PARAM_OF_PR                   = 0X02,     // pluse rate value and exceptions
    RB_PARAM_OF_PI                   = 0X03,     // pi value and exceptions
    RB_PARAM_OF_OXI_SYSTEM_EXCEPTION = 0X0C,     // plus Oximeter System Exceptions
    RB_PARAM_MAX_ITEM,
};

class RainbowProviderPrivate
{
public:
    explicit RainbowProviderPrivate(RainbowProvider *p)
        : q_ptr(p)
    {
    }

    ~RainbowProviderPrivate();

    /**
     * @brief handlePacket  解析包
     * @param data  包数据
     * @param len  包的长度
     */
    void handlePacket(unsigned char *data, int len);

    /**
     * @brief handleParamInfo  处理参数信息
     * @param data 数据
     * @param id  参数id
     * @param len 参数长度
     */
    void handleParamInfo(unsigned char *data, RBParamIDType id, int len);

    /**
     * @brief handleWaveformInfo  处理波形信息
     * @param data  数据
     * @param len  数据长度
     */
    void handleWaveformInfo(unsigned char *data, int len);

    /**
     * @brief handleBoardInfo  处理面板信息
     * @param data  数据
     * @param len  数据长度
     */
    void handleBoardInfo(unsigned char *data, int len);

    /**
     * @brief sendCmd  发送指令
     * @param data  数据
     * @param len  长度
     */
    void sendCmd(const unsigned char *data, unsigned int len);

    /**
     * @brief readData  读取数据
     * @param buff  读取的数据放入buff中
     * @param len  读取的长度
     */
    void readData(unsigned char *buff, unsigned int len);

    /**
     * @brief calcChecksum  协议数据校验
     * @param data  协议数据
     * @param len  长度
     * @return  返回校验码
     */
    unsigned char calcChecksum(const unsigned char *data, int len);

    /**
     * @brief unlockBoard  解锁rainbow模块
     * @param sn  SN序列号
     * @param flag 使能相关参数状态位
     */
    void unlockBoard(unsigned int sn, unsigned int flag);

    /**
     * @brief requestBoardInfo  请求底板信息，获取序列号
     */
    void requestBoardInfo();

    static const unsigned char minPacketLen = MIN_PACKET_LEN;
    RainbowProvider *q_ptr;
};

RainbowProvider::RainbowProvider()
    : Provider("RAINBOW_SPO2")
    , SPO2ProviderIFace()
    , d_ptr(new RainbowProviderPrivate(this))
{
    UartAttrDesc attr(DEFALUT_BAUD_RATE, 8, 'N', 1);
    initPort(attr);
    QTimer::singleShot(200, this, SLOT(onTimeOut()));
}

RainbowProvider::~RainbowProvider()
{
}

bool RainbowProvider::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_SPO2)
    {
        spo2Param.setProvider(this);
        Provider::attachParam(param);
    }
    return false;
}

void RainbowProvider::dataArrived()
{
    // 接收数据
    readData();

    // 无效数据退出处理
    if (ringBuff.dataSize() < d_ptr->minPacketLen)
    {
        return;
    }

    // 处理数据
    unsigned char buff[TEMP_BUFF_SIZE];
    while (ringBuff.dataSize() >= d_ptr->minPacketLen)
    {
        // 如果查询不到帧头，移除ringbuff缓冲区最旧的数据，下次继续查询
        if (ringBuff.at(0) != SOM)
        {
            ringBuff.pop(1);
            continue;
        }

        // 如果查询不到帧尾，移除ringbuff缓冲区最旧的数据，下次继续查询
        unsigned char len = ringBuff.at(1);
        unsigned char totalLen = 2 + len + 2;
        if (ringBuff.at(totalLen - 1) != EOM)
        {
            ringBuff.pop(1);
            continue;
        }

        // 将ringbuff中数据读取到临时缓冲区buff中,并移除ringbuff的旧数据
        for (int i = 0; i < totalLen; i++)
        {
            buff[i] = ringBuff.at(0);
            ringBuff.pop(1);
        }

        // 计算帧的校验码
        unsigned char csum = d_ptr->calcChecksum(&buff[2], len);

        // 如果求和检验码匹配，则进一步处理数据包，否则丢弃最旧数据
        if (csum == buff[totalLen - 2])
        {
            d_ptr->handlePacket(&buff[2], len);
        }
        else
        {
            outHex(buff, d_ptr->minPacketLen);
            debug("0x%02x", csum);
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

void RainbowProvider::dataArrived(unsigned char *data, unsigned char length)
{
    // 接收数据
    d_ptr->readData(data, length);

    // 无效数据退出处理
    if (ringBuff.dataSize() < d_ptr->minPacketLen)
    {
        return;
    }

    // 处理数据
    unsigned char buff[TEMP_BUFF_SIZE];
    while (ringBuff.dataSize() >= d_ptr->minPacketLen)
    {
        // 如果查询不到帧头，移除ringbuff缓冲区最旧的数据，下次继续查询
        if (ringBuff.at(0) != SOM)
        {
            ringBuff.pop(1);
            continue;
        }

        // 如果查询不到帧尾，移除ringbuff缓冲区最旧的数据，下次继续查询
        unsigned char len = ringBuff.at(1);
        unsigned char totalLen = 2 + len + 2;
        if (ringBuff.at(totalLen - 1) != EOM)
        {
            ringBuff.pop(1);
            continue;
        }

        // 将ringbuff中数据读取到临时缓冲区buff中,并移除ringbuff的旧数据
        for (int i = 0; i < totalLen; i++)
        {
            buff[i] = ringBuff.at(0);
            ringBuff.pop(1);
        }

        // 计算帧的校验码
        unsigned char csum = d_ptr->calcChecksum(&buff[2], len);

        // 如果求和检验码匹配，则进一步处理数据包，否则丢弃最旧数据
        if (csum == buff[totalLen - 2])
        {
            d_ptr->handlePacket(&buff[2], len);
        }
        else
        {
            outHex(buff, d_ptr->minPacketLen);
            debug("0x%02x", csum);
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

int RainbowProvider::getSPO2BaseLine()
{
    return 128;
}

int RainbowProvider::getSPO2MaxValue()
{
    return 128;
}


void RainbowProvider::setSensitivityFastSat(SensitivityMode mode, bool fastSat)
{
    unsigned char data[2] = {RB_CMD_CONF_SENSITIVITY_MODE, mode};
    d_ptr->sendCmd(data, 2);

    data[0] = RB_CMD_CONF_FASTSAT_MODE;
    data[1] = fastSat;
    d_ptr->sendCmd(data, 2);
}

void RainbowProvider::setAverageTime(AverageTime mode)
{
    unsigned char data[2] = {RB_CMD_CONF_AVERAGE_TIME, mode};
    d_ptr->sendCmd(data, 2);
}

void RainbowProvider::setSmartTone(bool enable)
{
    unsigned char data[2] = {RB_CMD_CONF_SMART_TONE_MODE, !!enable};
    d_ptr->sendCmd(data, 2);
}

void RainbowProvider::disconnected()
{
    spo2OneShotAlarm.clear();
    spo2OneShotAlarm.setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    spo2Param.setConnected(false);
}

void RainbowProvider::reconnected()
{
    spo2OneShotAlarm.setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    spo2Param.setConnected(true);
}

void RainbowProvider::onTimeOut()
{
    d_ptr->requestBoardInfo();
}

void RainbowProvider::setLineFrequency(RainbowLineFrequency freq)
{
    unsigned char data[2] = {RB_CMD_CONF_LINE_FREQ, freq};
    d_ptr->sendCmd(data, 2);
}












void RainbowProviderPrivate::handlePacket(unsigned char *data, int len)
{
    // 如果没有链接到参数功能，则不处理该模块数据包，直接退出
    if (q_ptr->isConnectedToParam == false)
    {
        return;
    }

    // 如果主机与该模块未连接成功，直接退出
    if (q_ptr->isConnected == false)
    {
        return;
    }

    // 发送保活帧
    q_ptr->feed();

    // 开始解析数据
    RBRecvPacketType packetType = static_cast<RBRecvPacketType>(data[0]);
    switch (packetType)
    {
    case  RB_ACK:
        break;
    case  RB_NCK:
    {
        qDebug() << "Bad Packet Sended of RainBow Provider\n";
    }
    break;
    case  RB_PARAM:
    {
        if (data[1] >= RB_PARAM_MAX_ITEM)
        {
            break;
        }
        RBParamIDType id = static_cast<RBParamIDType>(data[1]);
        handleParamInfo(&data[2], id, len - 2);
    }
    break;
    case  RB_WAVEFORM:
    {
        handleWaveformInfo(&data[1], len - 1);
    }
    break;
    case  RB_ENTER_PROGRAM_MODE:
        break;
    case  RB_PROGRAM_COMPLETE:
        break;
    case  RB_PROGRAM_ERROR:
        break;
    case  RB_BOARD_INFO:
    {
        handleBoardInfo(&data[1], len - 1);
    }
    break;
    case  RB_PLETH_CONTROL:
        break;
    default:
        break;
    }
}

void RainbowProviderPrivate::handleParamInfo(unsigned char *data, RBParamIDType id, int len)
{
    if (id >= RB_PARAM_MAX_ITEM)
    {
        return;
    }
    if (len < 6)
    {
        return;
    }
    unsigned short temp = 0;
    switch (id)
    {
    case RB_PARAM_OF_SPO2:
    {
        temp = (data[4] << 8) + data[5];
        bool valid = !!(temp & 0x0004);

        if (valid == true)
        {
            temp = (data[0] << 8) + data[1];
            temp = ((temp % 10) < 5) ? (temp / 10) : (temp / 10 + 1);
            spo2Param.setSPO2(temp);
        }
        else
        {
            spo2Param.setSPO2(InvData());
        }
    }
    break;
    case RB_PARAM_OF_PR:
    {
        temp = (data[4] << 8) + data[5];
        bool valid = !!(temp & 0x0004);
        if (valid == true)
        {
            temp = (data[0] << 8) + data[1];
            spo2Param.setPR(temp);
        }
        else
        {
            spo2Param.setPR(InvData());
        }
    }
    break;
    case RB_PARAM_OF_PI:
    {
        temp = (data[4] << 8) + data[5];
        bool valid = !!(temp & 0x0004);
        if (valid == true)
        {
            temp = (data[0] << 8) + data[1];
            float value = temp * 1.0 / 1000 + 0.05;
            spo2Param.updatePIValue(static_cast<short>(value * 10));
        }
        else
        {
            spo2Param.updatePIValue(InvData());
        }
    }
    break;
    case RB_PARAM_OF_OXI_SYSTEM_EXCEPTION:
    {
        unsigned int temp = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
        bool isCableOff = !!(temp & 0x80);
        isCableOff |= !!(temp & 0x00200000);

        bool  isSearching = !!(temp & 0x00400000);

        bool isLowPerfusionIndex = !!(temp & 0x01000000);

        if (isCableOff == true)
        {
            spo2Param.setNotify(true, trs("SPO2CheckSensor"));
            spo2Param.setValidStatus(false);
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false);
        }
        else
        {
            spo2Param.setValidStatus(true);
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, false);
            spo2Param.setSearchForPulse(isSearching);  // search pulse标志。
            if (isSearching)
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false);
            }
            else
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, isLowPerfusionIndex);
            }
        }
        if (isLowPerfusionIndex)
        {
            spo2Param.setSPO2(UnknownData());
            spo2Param.setPR(UnknownData());
        }
    }
    break;
    case RB_PARAM_MAX_ITEM:
    default:
        break;
    }
}

void RainbowProviderPrivate::handleWaveformInfo(unsigned char *data, int len)
{
    if (data == NULL)
    {
        return;
    }
    if (len < 8)
    {
        return;
    }

    int16_t waveData = data[0] << 8;
    waveData |= data[1];
    spo2Param.addWaveformData(waveData);

    if (data[5] & 0x80)
    {
        spo2Param.setPulseAudio(true);
    }
}

void RainbowProviderPrivate::handleBoardInfo(unsigned char *data, int len)
{
    if (data == NULL)
    {
        return;
    }
    if (len < 8)
    {
        return;
    }
    unsigned int sn = data[0] << 24;
    sn |= data[1] << 16;
    sn |= data[2] << 8;
    sn |= data[3];
    unsigned int falg = data[4] << 24;
    falg |= data[5] << 16;
    falg |= data[6] << 8;
    falg |= data[7];

    unlockBoard(sn, falg);
}

void RainbowProviderPrivate::sendCmd(const unsigned char *data, unsigned int len)
{
    int index = 0;
    unsigned char buff[TEMP_BUFF_SIZE] = {0};
    if (len + 4 > TEMP_BUFF_SIZE)
    {
        return;
    }
    buff[index++] = SOM;
    buff[index++] = len;
    for (unsigned int i = 0; i < len; i++)
    {
        buff[index++] = data[i];
    }
    buff[index++] = calcChecksum(data, len);
    buff[index++] = EOM;
    q_ptr->writeData(buff, len + 4);
}

void RainbowProviderPrivate::readData(unsigned char *buff, unsigned int len)
{
    q_ptr->ringBuff.push(buff, len);
}

unsigned char RainbowProviderPrivate::calcChecksum(const unsigned char *data, int len)
{
    unsigned char sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum = sum + data[i];
    }
    sum = sum & 0xFF;
    return sum;
}

void RainbowProviderPrivate::unlockBoard(unsigned int sn, unsigned int flag)
{
    unsigned char data[9] = {0};
    unsigned int unlockKey = MANU_ID ^ sn;
    data[0] = RB_CMD_UNLOCK_BOARD;
    data[1] = (unlockKey >> 24) & 0xff;
    data[2] = (unlockKey >> 16) & 0xff;
    data[3] = (unlockKey >>  8) & 0xff;
    data[4] = unlockKey & 0xff;
    data[5] = (flag >> 24) & 0xff;
    data[6] = (flag >> 16) & 0xff;
    data[7] = (flag >>  8) & 0xff;
    data[8] = flag & 0xff;

    sendCmd(data, sizeof(data));
}

void RainbowProviderPrivate::requestBoardInfo()
{
    unsigned char data[1] = {RB_CMD_REQUEST_BOARD_INFO};

    sendCmd(data, sizeof(data));
}
