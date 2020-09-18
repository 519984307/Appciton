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
#include "AlarmSourceManager.h"
#include "Framework/Language/LanguageManager.h"
#include "SystemManager.h"

#define SOM  (0xA1)
#define EOM  (0xAF)
#define DEFALUT_BAUD_RATE  (9600)
#define RUN_BAUD_RATE  (57600)
#define MIN_PACKET_LEN  (5)
#define TEMP_BUFF_SIZE  (64)
#define MANU_ID_BLM     (0x23ae5d53)
#define MANU_ID_DAVID   (0x55d9b582)

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
    RB_CMD_REQUEST_BOARD_INFO          = 0x70,     // request board info
    RB_CMD_UNLOCK_BOARD                = 0x71,     // unlock board
    RB_CMD_CONF_AVERAGE_TIME           = 0x01,     // configure average time
    RB_CMD_CONF_SENSITIVITY_MODE       = 0x02,     // configure sensitivity mode
    RB_CMD_CONF_FASTSAT_MODE           = 0x03,     // configure fastsat mode
    RB_CMD_CONF_LINE_FREQ              = 0x04,     // configure line frequency
    RB_CMD_CONF_SMART_TONE_MODE        = 0x08,     // configure smart tone mode
    RB_CMD_CONF_WAVEFORM_MODE          = 0x09,     // configure waveform mode
    RB_CMD_CONF_SPHB_PRECISION_MODE    = 0X0B,     // configure SpHb precision mode
    RB_CMD_CONF_PVI_AVERAGING_MODE     = 0X0C,     // configure PVI averaging mode
    RB_CMD_CONF_SPHB_BLOOD_VESSEL_MODE = 0X0D,     // configure SpHb arterial or venous mode
    RB_CMD_CONF_SPHB_AVERAGING_MODE    = 0x0E,     // configure SpHb averaging mode
    RB_CMD_REQ_PARAM_INFO              = 0x10,     // request parameter info by force
    RB_CMD_CONF_PERIOD_PARAM_OUTPUT    = 0x20,     // configure periodic parameter output
    RB_CMD_CONF_PERIOD_WAVE_OUTPUT     = 0x21,     // configure periodic waveform output
    RB_CMD_CONF_UPDATE_BAUDRATE        = 0x23,     // upgrade the baudrate
    RB_CMD_PROGRAM_MODE_ACK            = 0x40,     // program mode ack
    RB_CMD_PROGRAM_MODE_NAK            = 0x41,     // program mode nak
};

enum RBParamIDType
{
    RB_PARAM_OF_SPO2                 = 0X01,     // spo2 value and exceptions
    RB_PARAM_OF_PR                   = 0X02,     // pluse rate value and exceptions
    RB_PARAM_OF_PI                   = 0X03,     // pi value and exceptions
    RB_PARAM_OF_SPCO                 = 0X04,     // SpCO value and exceptions
    RB_PARAM_OF_SPMET                = 0X05,     // SpMet
    RB_PARAM_OF_SPHB                 = 0X07,     // SpHb
    RB_PARAM_OF_OXI_SYSTEM_EXCEPTION = 0X0C,     // plus Oximeter System Exceptions
    RB_PARAM_OF_BOARD_FAILURE        = 0X0D,     // board failure
    RB_PARAM_OF_SENSOR_PARAM_CHECK   = 0X0E,     // sensor param check
    RB_PARAM_OF_VERSION_INFO         = 0X0F,     // dsp version,base board params,mcu version and so on
    RB_PARAM_OF_BASELINE             = 0X2B,     // baseline
    RB_PARAM_OF_SPOC                 = 0X32,     // SpOC
    RB_PARAM_OF_PVI                  = 0X33,     // Pleth Variability Index(PVI)
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
    RB_INIT_SET_SPCO,
    RB_INIT_SET_PVI_AVERAGING_MODE,
    RB_INIT_SET_PVI,
    RB_INIT_SET_SPHB_PRECISION_MODE,
    RB_INIT_SET_SPHB_ARTERIAL_VENOUS_MODE,
    RB_INIT_SET_SPHB_AVERAGING_MODE,
    RB_INIT_SET_SPHB,
    RB_INIT_SET_SPMET,
    RB_INIT_SET_SPOC,
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

enum BaudRateType
{
    BAUD_RATE_9600,
    BAUD_RATE_19200,
    BAUD_RATE_28800,
    BAUD_RATE_38400,
    BAUD_RATE_57600,
    BAUD_RATE_115200,
    BAUD_RATE_230400,
    BAUD_RATE_INVAILD
};

enum PerfusionIndexExceptions
{
    PI_LOW_CONFIDENCE = 0X0001,  // Low PI Confidence
    PI_INVAILD = 0X0004,  // Invalid PI
    PI_INVALID_SMOOTH_PI = 0X0008,  // Invalid Smooth PI
};

enum RespirationRateExceptions
{
    INVAILD_RR = 0X0004,  // Invalid respiration rate
};

enum PulseRateExceptions
{
    PR_LOW_CONFIDENCE = 0X0001,  // Low PR Confidence
    PR_INVAILD = 0X0004,  // Invalid pulse rate
};

enum Spo2Exceptions
{
    SPO2_LOW_SIGNAL_IQ = 0X0001,  // spo2 low signal IQ
    SPO2_INVAILD = 0X0004,  // Invalid functional spo2
};

enum SpCOExceptions
{
    INVAILD_SPCO = 0X0004,  // Invalid spco
};

enum PVIExceptions
{
    PVI_LOW_CONFIDENCE = 0X0001,  // Low PVI Confidence
    PVI_INVAILD = 0X0004,    // Invalid PVI
};

enum SpHbExceptions
{
    SPHB_LOW_CONFIDENCE = 0X0001,  // Low SpHb Confidence
    SPHB_LOW_PERFUSION_INDEX = 0X0002,  // Low SpHb Perfusion Index
    SPHB_INVAILD = 0X0004,      // Invalid SpHb
};

enum SpMetExceptions
{
    SPMET_LOW_CONFIDENCE = 0X0001,  // Low SpMet Confidence
    SPMET_LOW_PERFUSION_INDEX = 0X0002,  // Low SpMet Perfusion Index
    SPMET_INVAILD = 0X0004,    // Invalid SpMet
};

enum SpOCExceptions
{
    SPOC_LOW_CONFIDENCE = 0X0001,  // Low SPOC Confidence
    SPOC_LOW_PERFUSION_INDEX = 0X0002,  // Low SPOC Perfusion Index
    SPOC_INVAILD = 0X0004,     // Invalid SpOC
};

enum RBProgramingError
{
    RB_PROG_ERR_COMM_ERROR,         /* communication error */
    RB_PROG_ERR_FLASH_ERROR,        /* flash error */
    RB_PROG_ERR_INVALID_APP,        /* invalid upgrade application */
    RB_PROG_ERR_INVALID_TOOL_CONF,  /* invalid tool configuration */
};

class RainbowProviderPrivate
{
public:
    explicit RainbowProviderPrivate(RainbowProvider *p)
        : q_ptr(p)
        , lineFreq(SPO2_LINE_FREQ_50HZ)
        , averTime(SPO2_AVER_TIME_2_4SEC)
        , sensMode(SPO2_MASIMO_SENS_MAX)
        , fastSat(false)
        , enableSmartTone(false)
        , curInitializeStep(RB_INIT_BAUDRATE)
        , isInitializing(false)
        , prValue(InvData())
        , spo2Value(InvData())
        , spHbPrecision(PRECISION_NEAREST_0_1)
        , pviAveragingMode(AVERAGING_MODE_NORMAL)
        , spHbBloodVessel(BLOOD_VESSEL_ARTERIAL)
        , spHbAveragingMode(SPHB_AVERAGING_MODE_LONG)
        , provider(SPO2_RAINBOW_FACTORY_ID_DAVID)
        , isPlugin(false)
        , inProgramMode(false)
        , spo2BoardFailure(false)
        , programTimer(NULL)
        , cmdAckNum(0)
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

    /**
     * @brief addAlarm  and the function of adding alarms
     * @param flag
     */
    void addAlarms(unsigned int flag);

    static const unsigned char minPacketLen = MIN_PACKET_LEN;

    RainbowProvider *q_ptr;

    SPO2LineFrequencyType lineFreq;

    AverageTime averTime;

    SensitivityMode sensMode;

    bool fastSat;

    bool enableSmartTone;

    RBInitializeStep curInitializeStep;

    bool isInitializing;    /* mark whether the module is in initialize state. When then module is
                             * intiliazing, all the setting command is save into the private data but
                             * not send to the module directly. The setting will be send during
                             * initilizing process.
                             */

    short prValue;

    short spo2Value;

    SpHbPrecisionMode spHbPrecision;

    AveragingMode pviAveragingMode;

    SpHbBloodVesselMode spHbBloodVessel;

    SpHbAveragingMode spHbAveragingMode;

    SPO2RainbowFactoryIDType provider;

    bool isPlugin;
    bool inProgramMode;
    bool spo2BoardFailure;   // Whether the spo2 board is faulty
    QTimer *programTimer;   /* timer to timeout every second during program */
    short cmdAckNum;    /* command ack num, some command need multi ack, because it send multi commnads */
};

RainbowProvider::RainbowProvider(const QString &name, bool isPlugin)
    : Provider(name)
    , SPO2ProviderIFace()
    , d_ptr(new RainbowProviderPrivate(this))
{
    d_ptr->averTime = spo2Param.getAverageTime();
    d_ptr->sensMode = static_cast<SensitivityMode>(spo2Param.getSensitivity());
    d_ptr->fastSat = spo2Param.getFastSat();
    d_ptr->enableSmartTone = static_cast<bool>(spo2Param.getSmartPulseTone());
    d_ptr->spHbAveragingMode = spo2Param.getSpHbAveragingMode();
    d_ptr->spHbBloodVessel = spo2Param.getSpHbBloodVessel();
    d_ptr->spHbPrecision = spo2Param.getSpHbPrecision();
    d_ptr->pviAveragingMode = spo2Param.getPviAveragingMode();
    d_ptr->isPlugin = isPlugin;

    d_ptr->programTimer = new QTimer(this);
    d_ptr->programTimer->setInterval(60 * 1000);   /* timeout in 1 minutes */
    connect(d_ptr->programTimer, SIGNAL(timeout()), this, SLOT(programPeriodTimeOut()));

    if (d_ptr->isPlugin)
    {
        plugInInfo.plugIn = PluginProvider::getPluginProvider("Plugin");
    }
    initModule();
}

RainbowProvider::~RainbowProvider()
{
}

bool RainbowProvider::attachParam(Param *param)
{
    if (param->getParamID() == PARAM_SPO2)
    {
        spo2Param.setProvider(this, d_ptr->isPlugin);
        Provider::attachParam(param);
        if (d_ptr->isPlugin)
        {
            setFirstCheck(true);
        }
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
        // 1 frame head + 1 lenblmpr byte + data length + 1 checksum + 1 frame end
        unsigned char totalLen = 2 + len + 2;

#if 0   /* TODO: check the packet length */
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
        // 1 frame head + 1 len byte + data length + 1 checksum + 1 frame end
        unsigned char totalLen = 2 + len + 2;

#if 1   /* TODO: check the packet length */
        if (totalLen > 40)
        {
            qWarning() << "packet too large, drop " << hex << ringBuff.at(0);
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
    // 返回复位成功后，需要给模块一点时间复位，才可以成功设置波特率。
    QTimer::singleShot(500, this, SLOT(changeBaudrate()));
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
    d_ptr->sensMode = mode;
    d_ptr->fastSat = fastSat;
    if (d_ptr->isInitializing)
    {
        return;
    }

    unsigned char data[2] = {RB_CMD_CONF_SENSITIVITY_MODE, mode};
    d_ptr->sendCmd(data, 2);

    data[0] = RB_CMD_CONF_FASTSAT_MODE;
    data[1] = fastSat;
    d_ptr->sendCmd(data, sizeof(data));
}

void RainbowProvider::setAverageTime(AverageTime mode)
{
    d_ptr->averTime = mode;
    if (d_ptr->isInitializing)
    {
        return;
    }

    unsigned char data[2] = {RB_CMD_CONF_AVERAGE_TIME, mode};
    d_ptr->sendCmd(data, sizeof(data));
}

void RainbowProvider::setSmartTone(bool enable)
{
    d_ptr->enableSmartTone = enable;
    if (d_ptr->isInitializing)
    {
        return;
    }

    unsigned char data[2] = {RB_CMD_CONF_SMART_TONE_MODE, !enable};
    d_ptr->sendCmd(data, 2);
}

void RainbowProvider::disconnected()
{
    AlarmOneShotIFace *alarmSource = NULL;
    if (d_ptr->isPlugin)
    {
        alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SPO2_2);
    }
    else
    {
        alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SPO2);
    }

    if (alarmSource)
    {
        alarmSource->clear();
        alarmSource->setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    }
    spo2Param.setConnected(false, d_ptr->isPlugin);
    /* clear then message on the wave widget when communication stop */
    spo2Param.setNotify(false, " ", d_ptr->isPlugin);
}

void RainbowProvider::reconnected()
{
    AlarmOneShotIFace *alarmSource = NULL;
    if (d_ptr->isPlugin)
    {
        alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SPO2_2);
    }
    else
    {
        alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SPO2);
    }
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    }
    spo2Param.setConnected(true, d_ptr->isPlugin);
}

void RainbowProvider::setSpHbPrecisionMode(SpHbPrecisionMode mode)
{
    d_ptr->spHbPrecision = mode;
    if (d_ptr->isInitializing)
    {
        return;
    }

    unsigned char data[2] = {RB_CMD_CONF_SPHB_PRECISION_MODE, mode};
    d_ptr->sendCmd(data, sizeof(data));
}

void RainbowProvider::setPVIAveragingMode(AveragingMode mode)
{
    d_ptr->pviAveragingMode = mode;
    if (d_ptr->isInitializing)
    {
        return;
    }

    unsigned char data[2] = {RB_CMD_CONF_PVI_AVERAGING_MODE, mode};
    d_ptr->sendCmd(data, sizeof(data));
}

void RainbowProvider::setSpHbBloodVesselMode(SpHbBloodVesselMode mode)
{
    d_ptr->spHbBloodVessel = mode;
    if (d_ptr->isInitializing)
    {
        return;
    }
    unsigned char data[2] = {RB_CMD_CONF_SPHB_BLOOD_VESSEL_MODE, mode};
    d_ptr->sendCmd(data, sizeof(data));
}

void RainbowProvider::setSphbAveragingMode(SpHbAveragingMode mode)
{
    d_ptr->spHbAveragingMode = mode;
    if (d_ptr->isInitializing)
    {
        return;
    }
    unsigned char data[2] = {RB_CMD_CONF_SPHB_AVERAGING_MODE, mode};
    d_ptr->sendCmd(data, sizeof(data));
}

void RainbowProvider::initModule()
{
    d_ptr->provider = SPO2_RAINBOW_FACTORY_ID_DAVID;
    d_ptr->curInitializeStep = RB_INIT_BAUDRATE;
    d_ptr->isInitializing = true;
    plugInInfo.pluginType = PluginProvider::PLUGIN_TYPE_SPO2;
    disPatchInfo.packetType = DataDispatcher::PACKET_TYPE_SPO2;

    if (d_ptr->isPlugin)
    {
        return;
    }

    UartAttrDesc attr(DEFALUT_BAUD_RATE, 8, 'N', 1);
    initPort(attr);


    if (disPatchInfo.dispatcher)
    {
        // reset the hardware
        disPatchInfo.dispatcher->resetPacketPort(disPatchInfo.packetType);
    }
    else
    {
        QTimer::singleShot(200, this, SLOT(changeBaudrate()));
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

void RainbowProvider::programFinished()
{
    d_ptr->programTimer->stop();
    stopCheckConnect(false);
    initModule();
}

void RainbowProvider::programPeriodTimeOut()
{
    programFinished();
}

void RainbowProvider::setLineFrequency(SPO2LineFrequencyType freq)
{
    d_ptr->lineFreq = freq;
    if (d_ptr->isInitializing)
    {
        return;
    }

    unsigned char data[2] = {RB_CMD_CONF_LINE_FREQ, freq};
    d_ptr->sendCmd(data, 2);
}

void RainbowProvider::setProgramResponse(bool ack)
{
    unsigned char data = ack ? RB_CMD_PROGRAM_MODE_ACK : RB_CMD_PROGRAM_MODE_NAK;
    d_ptr->sendCmd(&data, 1);
    d_ptr->inProgramMode = ack;
    d_ptr->programTimer->start();
    stopCheckConnect(true);
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
        spo2Param.setConnected(true, isPlugin);
    }

    // 发送保活帧
    q_ptr->feed();

    // 开始解析数据
    RBRecvPacketType packetType = static_cast<RBRecvPacketType>(data[0]);
    switch (packetType)
    {
    case  RB_ACK:
    {
        handleACK();
    }
    break;
    case  RB_NCK:
    {
        if (curInitializeStep == RB_INIT_GET_BOARD_INFO)
        {
            if (provider == SPO2_RAINBOW_FACTORY_ID_DAVID)
            {
                // 解锁david板子失败后再次解锁
                provider = SPO2_RAINBOW_FACTORY_ID_BLM;
                QTimer::singleShot(50, q_ptr, SLOT(requestBoardInfo()));
            }
            else if (provider == SPO2_RAINBOW_FACTORY_ID_BLM)
            {
                provider = SPO2_RAINBOW_FACTORY_ID_NR;
                qdebug("rainbow unlock failed");
            }
        }
        else  if (curInitializeStep == RB_INIT_BAUDRATE)
        {
            /* we get an nack while initialize the baudrate, maybe the module is already
             * unlocked and work in 57600 baudrate, just skip unlock step */
            curInitializeStep = RB_INIT_UNLOCK_BOARD;
            handleACK();
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "NAK: " << data[1];
        }
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
        spo2Param.showRainbowProgramMessage(SPO2_RAINBOW_PROG_MSG_REQUEST_ENTER_PROGRAM_MODE, isPlugin);
        break;
    case  RB_PROGRAM_COMPLETE:
        spo2Param.showRainbowProgramMessage(SPO2_RAINBOW_PROG_MSG_PROGRAM_COMPLETE, isPlugin);
        /* handle program finished issue after 2 second */
        QTimer::singleShot(2000, q_ptr, SLOT(programFinished()));
        break;
    case  RB_PROGRAM_ERROR:
    {
        switch (static_cast<RBProgramingError>(data[1]))
        {
        case RB_PROG_ERR_COMM_ERROR:
            spo2Param.showRainbowProgramMessage(SPO2_RAINBOW_PROG_MSG_COMMUNICATION_ERROR, isPlugin);
            break;
        case RB_PROG_ERR_FLASH_ERROR:
            spo2Param.showRainbowProgramMessage(SPO2_RAINBOW_PROG_MSG_FLASH_ERROR, isPlugin);
            break;
        case RB_PROG_ERR_INVALID_APP:
            spo2Param.showRainbowProgramMessage(SPO2_RAINBOW_PROG_MSG_INVALID_UPGRADE_APPLICATION, isPlugin);
            break;
        case RB_PROG_ERR_INVALID_TOOL_CONF:
            spo2Param.showRainbowProgramMessage(SPO2_RAINBOW_PROG_MSG_INVALID_TOOL_CONFIGURATION, isPlugin);
            break;
        default:
            break;
        }
        /* handle program finished issue after 2 second */
        QTimer::singleShot(2000, q_ptr, SLOT(programFinished()));
    }
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
    // the minimum size of the data field is 4
    if (len < 4)
    {
        return;
    }

    unsigned short temp = 0;
    switch (id)
    {
    case RB_PARAM_OF_SPO2:
    {
        temp = (data[4] << 8) + data[5];

        // SPO2 Low Signal IQ
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SPO2_LOW_SIGNAL_IQ, (temp & SPO2_LOW_SIGNAL_IQ), isPlugin);

        bool valid = !(temp & SPO2_INVAILD);
        if (valid == true && !spo2BoardFailure)
        {
            temp = (data[0] << 8) + data[1];
            temp = ((temp % 10) < 5) ? (temp / 10) : (temp / 10 + 1);
            spo2Value = temp;
        }
        else
        {
            spo2Value = InvData();
        }
    }
    break;
    case RB_PARAM_OF_PR:
    {
        if (!isPlugin)
        {
            temp = (data[4] << 8) + data[5];
            // SPO2 Low PR Confidence
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PR_CONFIDENCE, (temp & PR_LOW_CONFIDENCE), isPlugin);

            bool valid = !(temp & PR_INVAILD);
            if (valid == true && !spo2BoardFailure)
            {
                temp = (data[0] << 8) + data[1];
                prValue = temp;
            }
            else
            {
                prValue = InvData();
            }
        }
    }
    break;
    case RB_PARAM_OF_PI:
    {
        if (!isPlugin)
        {
            temp = (data[4] << 8) + data[5];
            // SPO2 Low PI Confidence
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PI_CONFIDENCE, (temp & PI_LOW_CONFIDENCE), isPlugin);
            // Invalid smooth pi
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INVALID_SMOOTH_PI, (temp & PI_INVALID_SMOOTH_PI), isPlugin);

            bool valid = !(temp & PI_INVAILD);
            if (valid == true && !spo2BoardFailure)
            {
                temp = (data[0] << 8) + data[1];
                float value = temp * 1.0 / 1000 + 0.05;
                spo2Param.setPI(static_cast<short>(value * 100));
            }
            else
            {
                spo2Param.setPI(InvData());
            }
        }
    }
    break;
    case RB_PARAM_OF_SPCO:
    {
        temp = (data[4] << 8) + data[5];
        bool valid = !(temp & INVAILD_SPCO);
        if (valid && !spo2BoardFailure)
        {
            temp = (data[0] << 8) + data[1];
            float value = (temp % 10) > 5 ? (temp / 10 + 1) : (temp / 10);
            spo2Param.setSpCO(value);
        }
        else
        {
            spo2Param.setSpCO(InvData());
        }
    }
    break;
    case RB_PARAM_OF_PVI:
    {
        temp = (data[4] << 8) + data[5];
        // SPO2 Low PVI Confidence
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PVI_CONFIDENCE, (temp & PVI_LOW_CONFIDENCE), isPlugin);

        bool valid = !(temp & PVI_INVAILD);
        if (valid && !spo2BoardFailure)
        {
            temp = (data[0] << 8) + data[1];
            spo2Param.setPVI(temp);
        }
        else
        {
            spo2Param.setPVI(InvData());
        }
    }
    break;
    case RB_PARAM_OF_SPHB:
    {
        temp = (data[4] << 8) + data[5];
        // Low SPHB Confidence
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_SPHB_CONFIDENCE, (temp & SPHB_LOW_CONFIDENCE), isPlugin);
        // Low SPHB Perfusion Index
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_SPHB_PERFUSION_INDEX,
                                  (temp & SPHB_LOW_PERFUSION_INDEX), isPlugin);

        bool valid = !(temp & SPHB_INVAILD);
        if (valid && !spo2BoardFailure)
        {
            if (spo2Param.getSpHbUnit() == SPHB_UNIT_G_DL)
            {
                /* keep one decimal */
                temp = (data[0] << 8) + data[1];
                spo2Param.setSpHb((temp + 5) / 10);
            }
            else
            {
                /* Use the alt SpHb value, mmol/L. However, the param only accept g/dL value,
                 * so we convert the unit here, 1 g/dL = 0.62 mmol/L */
                temp = (data[6] << 8) + data[7];
                float altValue = temp / 0.62;
                spo2Param.setSpHb((altValue + 5) / 10);
            }
        }
        else
        {
            spo2Param.setSpHb(InvData());
        }
    }
    break;
    case RB_PARAM_OF_SPMET:
    {
        temp = (data[4] << 8) + data[5];
        // Low SpMet Confidence
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_SPMET_CONFIDENCE, (temp & SPMET_LOW_CONFIDENCE), isPlugin);
        // Low SpMet Perfusion Index
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_SPMET_PERFUSION_INDEX,
                                  (temp & SPMET_LOW_PERFUSION_INDEX), isPlugin);

        bool valid = !(temp & SPMET_INVAILD);
        if (valid && !spo2BoardFailure)
        {
            temp = (data[0] << 8) + data[1];
            float value = temp * 1.0 / 10 + 0.5;
            spo2Param.setSpMet(value * 10);
        }
        else
        {
            spo2Param.setSpMet(InvData());
        }
    }
    break;
    case RB_PARAM_OF_SPOC:
    {
        temp = (data[4] << 8) + data[5];
        // Low SPOC Confidence
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_SPOC_CONFIDENCE, (temp & SPOC_LOW_CONFIDENCE), isPlugin);
        // Low SPOC Perfusion Index
        spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_SPOC_PERFUSION_INDEX,
                                  (temp & SPOC_LOW_PERFUSION_INDEX), isPlugin);
        bool valid = !(temp & SPOC_INVAILD);
        if (valid && !spo2BoardFailure)
        {
            temp = (data[0] << 8) + data[1];
            float value = temp * 1.0 / 10 + 0.5;
            spo2Param.setSpOC(value);
        }
        else
        {
            spo2Param.setSpOC(InvData());
        }
    }
    break;
    case RB_PARAM_OF_OXI_SYSTEM_EXCEPTION:
    {
        unsigned int temp = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];

        bool isCableOff = !!(temp & RB_NO_CABLE_CONNECTED);  // no cable connected

        isCableOff |= !!(temp & RB_NOSENSOR_CONNECTED);  // no sensor connected

        isCableOff |= !!(temp & RB_SENSOR_OFF_PATIENT);  // sensor off patient

        bool  isSearching = !!(temp & RB_PULSE_SEARCH);  // pulse search

        bool isLowPerfusionIndex = !!(temp & RB_LOW_PERFUSION_INDEX);  // low perfusion index

        addAlarms(temp);

        if (isPlugin)
        {
            spo2Param.setNotify(isCableOff, trs("SPO22CheckSensor"), isPlugin);
        }
        else
        {
            spo2Param.setNotify(isCableOff, trs("SPO2CheckSensor"), isPlugin);
        }

        if (isPlugin)
        {
            if (isCableOff == true)
            {
                spo2Param.setValidStatus(false, isPlugin);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true, true);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false, true);
            }
            else
            {
                spo2Param.setValidStatus(true, isPlugin);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, false, true);
                spo2Param.setSearchForPulse(isSearching, isPlugin);  // search pulse标志。
                if (isSearching)
                {
                    spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false, true);
                }
                else
                {
                    spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, isLowPerfusionIndex, true);
                }
            }
        }
        else
        {
            if (isCableOff == true)
            {
                spo2Param.setValidStatus(false, isPlugin);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false);
            }
            else
            {
                spo2Param.setValidStatus(true, isPlugin);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, false);
                spo2Param.setSearchForPulse(isSearching, isPlugin);  // search pulse标志。
                if (isSearching)
                {
                    spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false);
                }
                else
                {
                    spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, isLowPerfusionIndex);
                }
            }
        }
        // 最后更新spo2值和pr值。避免趋势界面的值跳动。
        spo2Param.setPerfusionStatus(isLowPerfusionIndex, isPlugin);
        if (!isPlugin)
        {
            spo2Param.setSPO2(spo2Value);
            spo2Param.setPR(prValue);
        }
        else
        {
            spo2Param.setPluginSPO2(spo2Value);
        }
    }
    break;
    case RB_PARAM_OF_VERSION_INFO:
    {
        unsigned short dspVersion = (data[0] << 8) | data[1];
        unsigned short mcuVersion = (data[6] << 8) | data[7];
//        unsigned short protocolVersion = (data[14] << 8) | data[15];
        QString dspVer = QString("DSP: V%1.%2.%3.%4").arg((dspVersion & 0xF000) >> 12)
                                                .arg((dspVersion & 0x0F00) >> 8)
                                                .arg((dspVersion & 0x00F0) >> 4)
                                                .arg(dspVersion & 0x000F);
        QString mcuVer = QString("MCU: V%1.%2.%3.%4").arg((mcuVersion & 0xF000) >> 12)
                                                .arg((mcuVersion & 0x0F00) >> 8)
                                                .arg((mcuVersion & 0x00F0) >> 4)
                                                .arg(mcuVersion & 0x000F);
        q_ptr->versionInfo = dspVer + "\n" + mcuVer;
    }
        break;
    case RB_PARAM_MAX_ITEM:
        break;
    case RB_PARAM_OF_BOARD_FAILURE:
    {
        unsigned short tmp = (data[0] << 8) | data[1];
        if (tmp > 0)
        {
            spo2BoardFailure = true;
            qWarning("Rainbow Board failure, reseting...");
            if (q_ptr->disPatchInfo.dispatcher)
            {
                q_ptr->disPatchInfo.dispatcher->resetPacketPort(q_ptr->disPatchInfo.packetType);
                isInitializing = true;
                curInitializeStep = RB_INIT_BAUDRATE;
            }
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_BOARD_FAILURE, true);
        }
        else
        {
            spo2BoardFailure = false;
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_BOARD_FAILURE, false);
        }
    }
    break;
    case RB_PARAM_OF_SENSOR_PARAM_CHECK:
    {
        unsigned short sensorType = (data[0] << 8) | data[1];
        unsigned short sensorFamilyMember = (data[2] << 8) | data[3];
        if (sensorFamilyMember == 8 || sensorType == 0)
        {
            // remove the debug hint
//            qDebug() << "No Sensor Connected!";

            // 传感器探头突然脱落时，会一直进来这里,添加报警(区分插件盒模块)
            if (isPlugin)
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true, true);
            }
            else
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
            }
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
    // SpO2 board failure, clear waveform data.
    if (spo2BoardFailure)
    {
        spo2Param.clearWaveformData();
        return;
    }

    short waveData = (data[0] << 8) | data[1];
    waveData = (waveData + 32768) / 256;    // change to a positive value, in range of [0, 255)
    waveData = 256 - waveData;      // upside down


    if (data[2] & SPO2_IQ_FLAG_BIT)  // get beep pulse audio status
    {
        unsigned char signalIQ = data[2];
        spo2Param.addWaveformData(waveData, signalIQ, isPlugin);
        // add wavedata twice for rounding SPO2 Waveform Sample  62.5 * 2 = 125
        spo2Param.addWaveformData(waveData, signalIQ, isPlugin);
        spo2Param.setPulseAudio(true);
    }
    else
    {
        spo2Param.addWaveformData(waveData, 0, isPlugin);
        // add wavedata twice for rounding SPO2 Waveform Sample  62.5 * 2 = 125
        spo2Param.addWaveformData(waveData, 0, isPlugin);
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

    curInitializeStep = RB_INIT_GET_BOARD_INFO;
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
    unsigned int unlockKey = MANU_ID_BLM ^ sn;
    if (provider == SPO2_RAINBOW_FACTORY_ID_DAVID)
    {
        unlockKey = MANU_ID_DAVID ^ sn;
    }
    else if (provider == SPO2_RAINBOW_FACTORY_ID_BLM)
    {
        unlockKey = MANU_ID_BLM ^ sn;
    }
    data[0] = RB_CMD_UNLOCK_BOARD;
    data[1] = (unlockKey >> 24) & 0xff;
    data[2] = (unlockKey >> 16) & 0xff;
    data[3] = (unlockKey >>  8) & 0xff;
    data[4] = unlockKey & 0xff;
    data[5] = (flag >> 24) & 0xff;
    data[6] = (flag >> 16) & 0xff;
    data[7] = (flag >>  8) & 0xff;
    data[8] = flag & 0xff;

    curInitializeStep = RB_INIT_GET_BOARD_INFO;
    sendCmd(data, sizeof(data));
}

void RainbowProviderPrivate::requestBoardInfo()
{
    unsigned char data[1] = {RB_CMD_REQUEST_BOARD_INFO};

    sendCmd(data, sizeof(data));
}

void RainbowProviderPrivate::updateBaudRate()
{
    unsigned char data[2] = {RB_CMD_CONF_UPDATE_BAUDRATE, BAUD_RATE_57600};    // upgrade the baudrate to 57600
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

/*
 * before sending the parameters setting at inialization, we have to
 * deasset the @isInitializeing flag temporarily
 */
#define SEND_INITSETTING_START()   {isInitializing = false;}
#define SEND_INITSETTING_END()  {isInitializing = true;}

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
                QTimer::singleShot(100, q_ptr, SLOT(requestBoardInfo()));
                qDebug() << "Rainbow baudrate has changed, update packet port baudrate.";
            }
            else if (q_ptr->plugInInfo.plugIn)
            {
                // data is transmited directly through the uart port
                q_ptr->plugInInfo.plugIn->updateUartBaud(RUN_BAUD_RATE);
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
            // 解锁成功后，执行下一步
            curInitializeStep = RB_INIT_UNLOCK_BOARD;
        case RB_INIT_UNLOCK_BOARD:
            // get here after board unlock
            configPeriodParamOut(RB_PARAM_OF_SENSOR_PARAM_CHECK, 500);  // 每500ms检查一次传感器参数
            curInitializeStep = RB_INIT_SENSOR_PARAM_CHECK;
            break;
        case RB_INIT_SENSOR_PARAM_CHECK:
            // get here after the param check command success
            SEND_INITSETTING_START();
            q_ptr->setLineFrequency(lineFreq);
            SEND_INITSETTING_END();
            curInitializeStep = RB_INIT_SET_LINE_FREQ;
            break;
        case RB_INIT_SET_LINE_FREQ:
            // get here after the line frequency
            SEND_INITSETTING_START();
            q_ptr->setAverageTime(averTime);
            SEND_INITSETTING_END();
            curInitializeStep = RB_INIT_SET_AVERAGE_TIME;
            break;
        case RB_INIT_SET_AVERAGE_TIME:
            // get here after the line frequency
            cmdAckNum = 0;
            SEND_INITSETTING_START();
            q_ptr->setSensitivityFastSat(sensMode, fastSat);
            SEND_INITSETTING_END();
            curInitializeStep = RB_INIT_SET_FAST_SAT;
            break;
        case RB_INIT_SET_FAST_SAT:
            cmdAckNum++;
            /* need two ack for @sendSensitivityFastSat api */
            if (cmdAckNum == 2)
            {
                // get here after the fast sat
                SEND_INITSETTING_START();
                q_ptr->setSmartTone(enableSmartTone);
                SEND_INITSETTING_END();
                curInitializeStep = RB_INIT_SMART_TONE;
            }
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
            // 每16ms输出一次波形
            configPeriodWaveformOut(CLIPPED_AUTOSCALE_DATA | SIGNAL_IQ_AUDIO_VISUAL_DATA, 16);
            if (provider == SPO2_RAINBOW_FACTORY_ID_BLM
                    || isPlugin
                    || !systemManager.isSupport(CONFIG_SPO2_HIGH_CONFIGURE))
            {
                // BLM rainbow didn't support senior parameters
                // plugin module didn't support senior parameters
                // system is configured not support senior parameters
                curInitializeStep = RB_INIT_COMPLETED;
                isInitializing = false;
            }
            else
            {
                /*
                 * when got here, not BLM rainbow module and not plugin and
                 * system support senior parameters
                 */
                curInitializeStep = RB_INIT_SET_SPCO;
            }
            break;
        case RB_INIT_SET_SPCO:
            configPeriodParamOut(RB_PARAM_OF_SPCO, 170);        // 每170ms输出一次SPCO
            curInitializeStep = RB_INIT_SET_PVI_AVERAGING_MODE;
            break;
        case RB_INIT_SET_PVI_AVERAGING_MODE:
            SEND_INITSETTING_START();
            q_ptr->setPVIAveragingMode(pviAveragingMode);
            SEND_INITSETTING_END();
            curInitializeStep = RB_INIT_SET_PVI;
            break;
        case RB_INIT_SET_PVI:
            configPeriodParamOut(RB_PARAM_OF_PVI, 150);         // 每150ms输出一次PVi
            curInitializeStep = RB_INIT_SET_SPHB_PRECISION_MODE;
            break;
        case RB_INIT_SET_SPHB_PRECISION_MODE:
            SEND_INITSETTING_START();
            q_ptr->setSpHbPrecisionMode(spHbPrecision);
            SEND_INITSETTING_END();
            curInitializeStep = RB_INIT_SET_SPHB_ARTERIAL_VENOUS_MODE;
            break;
        case RB_INIT_SET_SPHB_ARTERIAL_VENOUS_MODE:
            SEND_INITSETTING_START();
            q_ptr->setSpHbBloodVesselMode(spHbBloodVessel);
            SEND_INITSETTING_END();
            curInitializeStep = RB_INIT_SET_SPHB_AVERAGING_MODE;
            break;
        case RB_INIT_SET_SPHB_AVERAGING_MODE:
            SEND_INITSETTING_START();
            q_ptr->setSphbAveragingMode(spHbAveragingMode);
            SEND_INITSETTING_END();
            curInitializeStep = RB_INIT_SET_SPHB;
            break;
        case RB_INIT_SET_SPHB:
            configPeriodParamOut(RB_PARAM_OF_SPHB, 130);         // 每130ms输出一次SpHb
            curInitializeStep = RB_INIT_SET_SPMET;
            break;
        case RB_INIT_SET_SPMET:
            configPeriodParamOut(RB_PARAM_OF_SPMET, 140);       // 每140ms输出一次SpMet
            curInitializeStep = RB_INIT_SET_SPOC;
            break;
        case RB_INIT_SET_SPOC:
            configPeriodParamOut(RB_PARAM_OF_SPOC, 160);       // 每160ms输出一次SpOC
            curInitializeStep = RB_INIT_COMPLETED;
            break;
        case RB_INIT_COMPLETED:
            isInitializing = false;
            break;
        default:
            break;
        }
    }
    else  // 当初始化完成时，请求参数状态（里面含有版本信息）
    {
        isInitializing = false;
        requestParamStatus();
    }
}

void RainbowProviderPrivate::requestParamStatus()
{
    unsigned char data[2] = {RB_CMD_REQ_PARAM_INFO, RB_PARAM_OF_VERSION_INFO};
    sendCmd(data, sizeof(data));
}

void RainbowProviderPrivate::addAlarms(unsigned int flag)
{
    if (isPlugin)
    {
        if (flag & RB_DEFECTIVE_CABLE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_CABLE, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_CABLE, false, true);
        }

        if (flag & RB_CABLE_EXPIRED)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_EXPIRED, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_EXPIRED, false, true);
        }

        if (flag & RB_INCOMPATIBLE_CABLE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_CABLE, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_CABLE, false, true);
        }

        if (flag & RB_UNRECONGNIZED_CABLE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_CABLE, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_CABLE, false, true);
        }

        if (flag & RB_CABLE_NEAR_EXPIRATION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_NEAR_EXPIRATION, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_NEAR_EXPIRATION, false, true);
        }

        if (flag & RB_SENSOR_EXPIRED)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_EXPIRED, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_EXPIRED, false, true);
        }

        if (flag & RB_INCOMPATIBLE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_SENSOR, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_SENSOR, false, true);
        }

        if (flag & RB_UNRECONGNIZED_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_SENSOR, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_SENSOR, false, true);
        }

        if (flag & RB_DEFECTIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_SENSOR, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_SENSOR, false, true);
        }

        if (flag & RB_CHECK_CABLE_AND_SENSOR_FAULT)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_CABLE_AND_SENSOR_FAULT, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_CABLE_AND_SENSOR_FAULT, false, true);
        }

        if (flag & RB_SENSOR_NEAR_EXPIRATION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_NEAR_EXPIRATION, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_NEAR_EXPIRATION, false, true);
        }

        if (flag & RB_NO_ADHESIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_NO_ADHESIVE_SENSOR, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_NO_ADHESIVE_SENSOR, false, true);
        }

        if (flag & RB_ADHESIVE_SENSOR_EXPIRATION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_EXPIRATION, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_EXPIRATION, false, true);
        }

        if (flag & RB_INCOMPATIBLE_ADHESIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_ADHESIVE_SENSOR, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_ADHESIVE_SENSOR, false, true);
        }

        if (flag & RB_UNRECONGNIZED_ADHESIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_ADHESIVE_SENSOR, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_ADHESIVE_SENSOR, false, true);
        }

        if (flag & RB_DEFECTIVE_ADHESIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_ADHESIVE_SENSOR, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_ADHESIVE_SENSOR, false, true);
        }

        if (flag & RB_SENSOR_INITING)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_INITING, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_INITING, false, true);
        }

        if (flag & RB_INTERFERENCE_DETECTED)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INTERFERENCE_DETECTED, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INTERFERENCE_DETECTED, false, true);
        }

        if (flag & RB_DEMO_MODE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEMO_MODE, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEMO_MODE, false, true);
        }

        if (flag & RB_ADHESIVE_SENSOR_NEAR_EXPIRATION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_NEAR_EXPIRATION, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_NEAR_EXPIRATION, false, true);
        }

        if (flag & RB_CHECK_SENSOR_CONNECTION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR_CONNECTION, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR_CONNECTION, false, true);
        }

        if (flag & RB_SPO2_ONLY_MODE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SPO2_ONLY_MODE, true, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SPO2_ONLY_MODE, false, true);
        }
    }
    else
    {
        if (flag & RB_DEFECTIVE_CABLE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_CABLE, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_CABLE, false);
        }

        if (flag & RB_CABLE_EXPIRED)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_EXPIRED, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_EXPIRED, false);
        }

        if (flag & RB_INCOMPATIBLE_CABLE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_CABLE, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_CABLE, false);
        }

        if (flag & RB_UNRECONGNIZED_CABLE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_CABLE, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_CABLE, false);
        }

        if (flag & RB_CABLE_NEAR_EXPIRATION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_NEAR_EXPIRATION, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CABLE_NEAR_EXPIRATION, false);
        }

        if (flag & RB_SENSOR_EXPIRED)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_EXPIRED, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_EXPIRED, false);
        }

        if (flag & RB_INCOMPATIBLE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_SENSOR, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_SENSOR, false);
        }

        if (flag & RB_UNRECONGNIZED_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_SENSOR, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_SENSOR, false);
        }

        if (flag & RB_DEFECTIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_SENSOR, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_SENSOR, false);
        }

        if (flag & RB_CHECK_CABLE_AND_SENSOR_FAULT)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_CABLE_AND_SENSOR_FAULT, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_CABLE_AND_SENSOR_FAULT, false);
        }

        if (flag & RB_SENSOR_NEAR_EXPIRATION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_NEAR_EXPIRATION, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_NEAR_EXPIRATION, false);
        }

        if (flag & RB_NO_ADHESIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_NO_ADHESIVE_SENSOR, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_NO_ADHESIVE_SENSOR, false);
        }

        if (flag & RB_ADHESIVE_SENSOR_EXPIRATION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_EXPIRATION, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_EXPIRATION, false);
        }

        if (flag & RB_INCOMPATIBLE_ADHESIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_ADHESIVE_SENSOR, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INCOMPATIBLE_ADHESIVE_SENSOR, false);
        }

        if (flag & RB_UNRECONGNIZED_ADHESIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_ADHESIVE_SENSOR, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_UNRECONGNIZED_ADHESIVE_SENSOR, false);
        }

        if (flag & RB_DEFECTIVE_ADHESIVE_SENSOR)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_ADHESIVE_SENSOR, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEFECTIVE_ADHESIVE_SENSOR, false);
        }

        if (flag & RB_SENSOR_INITING)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_INITING, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SENSOR_INITING, false);
        }

        if (flag & RB_INTERFERENCE_DETECTED)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INTERFERENCE_DETECTED, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_INTERFERENCE_DETECTED, false);
        }

        if (flag & RB_DEMO_MODE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEMO_MODE, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_DEMO_MODE, false);
        }

        if (flag & RB_ADHESIVE_SENSOR_NEAR_EXPIRATION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_NEAR_EXPIRATION, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_NEAR_EXPIRATION, false);
        }

        if (flag & RB_CHECK_SENSOR_CONNECTION)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR_CONNECTION, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR_CONNECTION, false);
        }

        if (flag & RB_SPO2_ONLY_MODE)
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SPO2_ONLY_MODE, true);
        }
        else
        {
            spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_SPO2_ONLY_MODE, false);
        }
    }
}
