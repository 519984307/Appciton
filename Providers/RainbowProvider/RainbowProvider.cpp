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
#define RUN_BAUD_RATE  (57600)
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
    RB_BASELINE_PI                   = 0X2B,     // baseline pi
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
    RB_CMD_REQ_PARAM_INFO            = 0x10,     // request parameter info by force
    RB_CMD_CONF_PERIOD_PARAM_OUTPUT  = 0x20,     // configure periodic parameter output
    RB_CMD_CONF_PERIOD_WAVE_OUTPUT   = 0x21,     // configure periodic waveform output
    RB_CMD_CONF_UPDATE_BAUDRATE      = 0x23,     // upgrade the baudrate
};

enum RBParamIDType
{
    RB_PARAM_OF_SPO2                 = 0X01,     // spo2 value and exceptions
    RB_PARAM_OF_PR                   = 0X02,     // pluse rate value and exceptions
    RB_PARAM_OF_PI                   = 0X03,     // pi value and exceptions
    RB_PARAM_OF_OXI_SYSTEM_EXCEPTION = 0X0C,     // plus Oximeter System Exceptions
    RB_PARAM_OF_BOARD_FAILURE        = 0X0D,     // board failure
    RB_PARAM_OF_SENSOR_PARAM_CHECK   = 0X0E,     // sensor param check
    RB_PARAM_OF_VERSION_INFO         = 0X0F,     // dsp version,base board params,mcu version and so on
    RB_PARAM_OF_BASELINE             = 0X2B,     // baseline
    RB_PARAM_MAX_ITEM,
};

enum RBWaveformSelectionBitsType
{
    AUTOSCALE_DATA                   = 0X010000,  // Autoscale data(IR_LED)
    CLIPPED_AUTOSCALE_DATA           = 0X020000,  // Clipped autoscal data
    SIGNAL_IQ_DATA                   = 0X040000,  // Signal IQ data
    SIGNAL_IQ_AUDIO_VISUAL_DATA      = 0X100000,  // Signal IQ Audio-Visual data
    ACOUSTIC_DISPLAY_DATA            = 0X400000,  // Acoustic display data
};


enum RBInitializeStep
{
    RB_INIT_BAUDRATE,
    RB_INIT_GET_BOARD_INFO,
    RB_INIT_UNLOCK_BOARD,
    RB_INIT_SENSOR_PARAM_CHECK,
    RB_INIT_SET_LINE_FREQ,
    RB_INIT_SET_AVERAGE_TIME,
    RB_INIT_SET_FAST_SAT,
    RB_INIT_SMART_TONE,
    RB_INIT_GET_SYSTEM_EXECPTION,
    RB_INIT_SET_PI,
    RB_INIT_GET_BOARD_FAILURE,
    RB_INIT_SET_SPO2,
    RB_INIT_SET_BASELINE,
    RB_INIT_SET_WAVEFORM,
    RB_INIT_COMPLETED,
};

enum RBPulseOximeterSystemExceptions
{
    RB_NO_CABLE_CONNECTED                        = (1 << 0),  // no cable connected
    RB_CABLE_EXPIRED                             = (1 << 1),  // cable expired
    RB_INCOMPATIBLE_CABLE                        = (1 << 2),  // incompatible cable
    RB_UNRECONGNIZED_CABLE                       = (1 << 3),  // unrecongnized cable
    RB_DEFECTIVE_CABLE                           = (1 << 4),  // defective cable
    RB_CABLE_NEAR_EXPIRATION                     = (1 << 5),  // cable near expiration
    RB_RESERVED                                  = (1 << 6),  // reserved
    RB_NOSENSOR_CONNECTED                        = (1 << 7),  // no sensor connected
    RB_SENSOR_EXPIRED                            = (1 << 8),  // sensor expired
    RB_INCOMPATIBLE_SENSOR                       = (1 << 9),  // incompatible sensor
    RB_UNRECONGNIZED_SENSOR                      = (1 << 10),  // unrecongnized sensor
    RB_DEFECTIVE_SENSOR                          = (1 << 11),  // defective sensor
    RB_CHECK_CABLE_AND_SENSOR_FAULT              = (1 << 12),  // check cable and sensor fault
    RB_RESERVED_TWO                              = (1 << 13),  // reserved
    RB_SENSOR_NEAR_EXPIRATION                    = (1 << 14),  // sensor near expiration
    RB_NO_ADHESIVE_SENSOR                        = (1 << 15),  // no adhesive sensor
    RB_ADHESIVE_SENSOR_EXPIRATION                = (1 << 16),  // adhesive sensor expiraton
    RB_INCOMPATIBLE_ADHESIVE_SENSOR              = (1 << 17),  // incompatible adhesive sensor
    RB_UNRECONGNIZED_ADHESIVE_SENSOR             = (1 << 18),  // unrecongnized adhesive sensor
    RB_DEFECTIVE_ADHESIVE_SENSOR                 = (1 << 19),  // defective adhesive sensor
    RB_SENSOR_INITING                            = (1 << 20),  // sensor initing
    RB_SENSOR_OFF_PATIENT                        = (1 << 21),  // sensor off patient
    RB_PULSE_SEARCH                              = (1 << 22),  // pulse search
    RB_INTERFERENCE_DETECTED                     = (1 << 23),  // interference detected
    RB_LOW_PERFUSION_INDEX                       = (1 << 24),  // low perfusion index
    RB_DEMO_MODE                                 = (1 << 25),  // demo mode
    RB_ADHESIVE_SENSOR_NEAR_EXPIRATION           = (1 << 26),  // adhesive sensor near expiration
    RB_RESERVED_THREE                            = (1 << 27),  // reserved
    RB_CHECK_SENSOR_CONNECTION                   = (1 << 28),  // check sensor connection
    RB_SPO2_ONLY_MODE                            = (1 << 29),  // spo2 only mode
    RB_RESERVED_FOUR                             = (1 << 30),  // reserved
    RB_RESERVED_FIVE                             = (1 << 31),  // reserved
};


class RainbowProviderPrivate
{
public:
    explicit RainbowProviderPrivate(RainbowProvider *p)
        : q_ptr(p)
        , lineFreq(RB_LINE_FREQ_50HZ)
        , averTime(SPO2_AVER_TIME_2_4SEC)
        , sensMode(SPO2_MASIMO_SENS_MAX)
        , fastSat(false)
        , enableSmartTone(false)
        , curInitializeStep(RB_INIT_BAUDRATE)
        , noSensor(true)
        , isReseting(false)
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

    /**
     * @brief upgradeBaudRate upgrade the baudrate
     */
    void updateBaudRate();

    /**
     * @brief configPeriodParamOut  配置参数输出周期
     * @param paramId  参数id
     * @param periodTime  周期时间
     */
    void configPeriodParamOut(RBParamIDType paramId, unsigned int periodTime);

    /**
     * @brief configPeriodWaveformOut  配置波形输出周期
     * @param selectionBits  波形参数选择
     * @param periodTime  周期时间
     */
    void configPeriodWaveformOut(unsigned int selectionBits, unsigned char periodTime);

    /**
     * @brief handleACK  接收ack处理
     */
    void handleACK();

    /**
     * @brief requestParamStatus  请求参数状态
     */
    void requestParamStatus();

    static const unsigned char minPacketLen = MIN_PACKET_LEN;

    RainbowProvider *q_ptr;

    RainbowLineFrequency lineFreq;

    AverageTime averTime;

    SensitivityMode sensMode;

    bool fastSat;

    bool enableSmartTone;

    RBInitializeStep curInitializeStep;

    bool noSensor;

    bool isReseting;
};

RainbowProvider::RainbowProvider()
    : Provider("RAINBOW_SPO2")
    , SPO2ProviderIFace()
    , d_ptr(new RainbowProviderPrivate(this))
{
    disPatchInfo.packetType = DataDispatcher::PACKET_TYPE_SPO2;
    UartAttrDesc attr(DEFALUT_BAUD_RATE, 8, 'N', 1);
    initPort(attr);

    if (disPatchInfo.dispatcher)
    {
        // reset the hardware
        disPatchInfo.dispatcher->resetPacketPort(disPatchInfo.packetType);
        d_ptr->isReseting = true;
    }
    else
    {
        QTimer::singleShot(200, this, SLOT(changeBaudrate()));
    }
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
        return true;
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
    while (ringBuff.dataSize() >= d_ptr->minPacketLen)
    {
        // 如果查询不到帧头，移除ringbuff缓冲区最旧的数据，下次继续查询
        if (ringBuff.at(0) != SOM)
        {
            ringBuff.pop(1);
            continue;
        }


        // 如果查询不到帧尾，移除ringbuff缓冲区最旧的数据，下次继续查询
        unsigned char len = ringBuff.at(1);     // data field length
        unsigned char totalLen = 2 + len + 2;   // 1 frame head + 1 lenblmpr byte + data length + 1 checksum + 1 frame end

        if (ringBuff.dataSize() < totalLen)
        {
            // no enough data
            break;
        }

        if (ringBuff.at(totalLen - 1) != EOM)
        {
            ringBuff.pop(1);
            continue;
        }

        unsigned char buff[TEMP_BUFF_SIZE] = {0};
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

void RainbowProvider::dataArrived(unsigned char *data, unsigned int length)
{
    // 接收数据
    d_ptr->readData(data, length);

    if (d_ptr->isReseting)
    {
        while (ringBuff.dataSize())
        {
            ringBuff.pop(1);
        }
    }

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
        unsigned char len = ringBuff.at(1);     // data field length
        unsigned char totalLen = 2 + len + 2;   // 1 frame head + 1 len byte + data length + 1 checksum + 1 frame end

#if 0   // TODO: check the packet length
        if (totalLen > 40)
        {
            qDebug() << "packet too large";
            ringBuff.pop(1);
            continue;
        }
#endif

        if (ringBuff.dataSize() < totalLen)
        {
            // no enough data
            break;
        }

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

void RainbowProvider::dispatchPortHasReset()
{
    d_ptr->isReseting = false;
    QTimer::singleShot(0, this, SLOT(changeBaudrate()));
}

int RainbowProvider::getSPO2BaseLine()
{
    return 0;
}

int RainbowProvider::getSPO2MaxValue()
{
    return 255;
}

void RainbowProvider::sendVersion()
{
    return;
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
    if (d_ptr->curInitializeStep != RB_INIT_COMPLETED)
    {
        d_ptr->handleACK();
    }
}

void RainbowProvider::requestBoardInfo()
{
    d_ptr->requestBoardInfo();
}

void RainbowProvider::changeBaudrate()
{
    d_ptr->updateBaudRate();
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
        spo2Param.setConnected(true);
    }

    // 发送保活帧
    q_ptr->feed();

    // 开始解析数据
    RBRecvPacketType packetType = static_cast<RBRecvPacketType>(data[0]);
    switch (packetType)
    {
    case  RB_ACK:
    {
        qDebug() << Q_FUNC_INFO << "ACK" << curInitializeStep;
        handleACK();
    }
    break;
    case  RB_NCK:
    {
        qDebug() << Q_FUNC_INFO << "Receive NAK type " << data[1];
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
        bool valid = !(temp & 0x0004);

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
        bool valid = !(temp & 0x0004);
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
        bool valid = !(temp & 0x0004);
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

        bool isCableOff = !!(temp & RB_NOSENSOR_CONNECTED);  // no sensor connected

        isCableOff |= !!(temp & RB_SENSOR_OFF_PATIENT);  // sensor off patient

        bool  isSearching = !!(temp & RB_PULSE_SEARCH);  // pulse search

        bool isLowPerfusionIndex = !!(temp & RB_LOW_PERFUSION_INDEX);  // low perfusion index

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
    case RB_PARAM_OF_VERSION_INFO:
        break;
    case RB_PARAM_MAX_ITEM:
        break;
    case RB_PARAM_OF_BOARD_FAILURE:
    {
        unsigned short tmp = (data[0] << 8) | data[1];
        if (tmp > 0)
        {
            qWarning("Rainbow Board failure, reseting...");
            if (q_ptr->disPatchInfo.dispatcher)
            {
                q_ptr->disPatchInfo.dispatcher->resetPacketPort(q_ptr->disPatchInfo.packetType);
                isReseting = true;
                curInitializeStep = RB_INIT_BAUDRATE;
            }
        }
    }
    break;
    case RB_PARAM_OF_SENSOR_PARAM_CHECK:
    {
        unsigned short sensorType = (data[0] << 8) | data[1];
        unsigned short sensorFamilyMember = (data[2] << 8) | data[3];
        if (sensorFamilyMember == 8 || sensorType == 0)
        {
            noSensor = true;
        }
        else
        {
            noSensor = false;
        }
    }
    break;
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

    if (len < 3)
    {
        return;
    }

    short waveData = (data[0] << 8) | data[1];
    waveData = (waveData + 32768) / 256;    // change to a positive value, in range of [0, 255)
    waveData = 256 - waveData;      // upside down
    spo2Param.addWaveformData(waveData);
    spo2Param.addWaveformData(waveData);  // add wavedata twice for rounding SPO2 Waveform Sample  62.5 * 2 = 125

    if (data[2] & 0x80)
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
    curInitializeStep = RB_INIT_UNLOCK_BOARD;
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

void RainbowProviderPrivate::updateBaudRate()
{
    unsigned char data[2] = {RB_CMD_CONF_UPDATE_BAUDRATE, 0x04};    // upgrade the baudrate to 57600
    sendCmd(data, sizeof(data));
}

void RainbowProviderPrivate::configPeriodParamOut(RBParamIDType paramId, unsigned int periodTime)
{
    unsigned char id = static_cast<unsigned char>(paramId);
    unsigned char data[4] = {RB_CMD_CONF_PERIOD_PARAM_OUTPUT, id};
    data[2] = periodTime >> 8;
    data[3] = periodTime & 0xff;

    sendCmd(data, sizeof(data));
}

void RainbowProviderPrivate::configPeriodWaveformOut(unsigned int selectionBits, unsigned char periodTime)
{
    unsigned char data[5] = {RB_CMD_CONF_PERIOD_WAVE_OUTPUT};
    data[1] = (selectionBits >> 16) & 0xff;
    data[2] = (selectionBits >>  8) & 0xff;
    data[3] = (selectionBits) & 0xff;
    data[4] = periodTime;

    sendCmd(data, sizeof(data));
}

void RainbowProviderPrivate::handleACK()
{
    if (curInitializeStep != RB_INIT_COMPLETED)
    {
        switch (curInitializeStep)
        {
        case RB_INIT_BAUDRATE:
        {
            // baudrate has been update, switch to new baudrate
            if (q_ptr->disPatchInfo.dispatcher)
            {
                // data is tramsmited through the dispatcher
                // tell the dispatch to change the baudrate
                q_ptr->disPatchInfo.dispatcher->setPacketPortBaudrate(q_ptr->disPatchInfo.packetType,
                        DataDispatcher::BAUDRATE_57600);
                QTimer::singleShot(50, q_ptr, SLOT(requestBoardInfo()));
            }
            else
            {
                // data is transmited directly through the uart port
                // set the port's baudrate
                UartAttrDesc attr(RUN_BAUD_RATE, 8, 'N', 1);
                q_ptr->uart->updateSetting(attr);
                curInitializeStep = RB_INIT_GET_BOARD_INFO;
                QTimer::singleShot(0, q_ptr, SLOT(requestBoardInfo()));
            }
        }
        break;
        case RB_INIT_GET_BOARD_INFO:
            // board info response is not a ack message
            break;
        case RB_INIT_UNLOCK_BOARD:
            // get here after board unlock
            configPeriodParamOut(RB_PARAM_OF_SENSOR_PARAM_CHECK, 500);
            curInitializeStep = RB_INIT_SENSOR_PARAM_CHECK;
            break;
        case RB_INIT_SENSOR_PARAM_CHECK:
            // get here after the param check command success
            q_ptr->setLineFrequency(lineFreq);
            curInitializeStep = RB_INIT_SET_LINE_FREQ;
            break;
        case RB_INIT_SET_LINE_FREQ:
            // get here after the line frequency
            q_ptr->setAverageTime(averTime);
            curInitializeStep = RB_INIT_SET_AVERAGE_TIME;
            break;
        case RB_INIT_SET_AVERAGE_TIME:
            // get here after the line frequency
            q_ptr->setSensitivityFastSat(sensMode, fastSat);
            curInitializeStep = RB_INIT_SET_FAST_SAT;
            break;
        case RB_INIT_SET_FAST_SAT:
            // get here after the fast sat
            q_ptr->setSmartTone(enableSmartTone);
            curInitializeStep = RB_INIT_SMART_TONE;
            break;
        case RB_INIT_SMART_TONE:
            // get here after the smart tone
            configPeriodParamOut(RB_PARAM_OF_OXI_SYSTEM_EXCEPTION, 100);  // 每100ms检查一次异常
            curInitializeStep = RB_INIT_GET_SYSTEM_EXECPTION;
            break;
        case RB_INIT_GET_SYSTEM_EXECPTION:
            // get here after the system execption
            configPeriodParamOut(RB_PARAM_OF_PI, 200);  // 每200ms输出一次灌注PI
            curInitializeStep = RB_INIT_SET_PI;
            break;
        case RB_INIT_SET_PI:
            // get here after the setting pi
            configPeriodParamOut(RB_PARAM_OF_BOARD_FAILURE, 180);  // 每180ms输出一次board failure
            curInitializeStep = RB_INIT_GET_BOARD_FAILURE;
            break;
        case RB_INIT_GET_BOARD_FAILURE:
            // get here after the board failure
            configPeriodParamOut(RB_PARAM_OF_SPO2, 110);  // 每110ms输出一次spo2
            curInitializeStep = RB_INIT_SET_SPO2;
            break;
        case RB_INIT_SET_SPO2:
            // get here after the spo2
            configPeriodParamOut(RB_PARAM_OF_PR, 120);  // 每120ms输出一次脉率
            curInitializeStep = RB_INIT_SET_BASELINE;
            break;
        case RB_INIT_SET_BASELINE:
            // get here after the baseline
            configPeriodParamOut(RB_PARAM_OF_BASELINE, 170);  // 每170ms输出一次Baseline PI
            curInitializeStep = RB_INIT_SET_WAVEFORM;
            break;
        case RB_INIT_SET_WAVEFORM:
            // get here after the baseline
            configPeriodWaveformOut(CLIPPED_AUTOSCALE_DATA | SIGNAL_IQ_AUDIO_VISUAL_DATA, 16);  // 每16ms输出一次波形
            curInitializeStep = RB_INIT_COMPLETED;
            break;
        default:
            break;
        }
    }
}

void RainbowProviderPrivate::requestParamStatus()
{
    unsigned char data[2] = {RB_CMD_REQ_PARAM_INFO, RB_PARAM_OF_VERSION_INFO};
    sendCmd(data, sizeof(data));
}