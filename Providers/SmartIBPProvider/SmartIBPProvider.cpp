/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/4/28
 **/

#include "SmartIBPProvider.h"
#include "IBPParam.h"
#include "Debug/Debug.h"
#include "AlarmSourceManager.h"

#define MAX_PACKET_LENGTH       12      /* maximum packet length */
#define MIN_PARSE_LENGTH        2       /* minimum length of data to start parse */
#define FRAME_HEAD              0xFF    /* frame head */
#define MODULE_CMD_ZERO         0x00    /* channel zero */
#define MODULE_CMD_CALIBRATE    0x01    /* channel calibrate */

#define MODULE_IBP_CHN_1        0x00    /* IBP channel 1 */
#define MODULE_IBP_CHN_2        0x01    /* IBP channel 2 */

#define MODULE_ID               0xF9    /* moudle ID */
#define MODULE_CMD_RSP          0xFE    /* command response */

#define DATA_TYPE_SYS       0x00
#define DATA_TYPE_DIA       0x01
#define DATA_TYPE_MAP       0x02
#define DATA_TYPE_PR        0x03

#define INVALID_MEASURE_VALUE   0xFFF
#define MIN_PRESSURE_VALUE  (-50)
#define MAX_PRESSURE_VALUE  (350)

#define MAX_CACHE_WAVE_NUM  4

struct IBPChannelData
{
    IBPChannelData()
        : sys(InvData()), dia(InvData()), map(InvData()), pr(InvData()),
          sensorOff(false), checkValidDataAfterZero(true) {}
    qint16 sys;
    qint16 dia;
    qint16 map;
    qint16 pr;
    bool sensorOff;
    bool checkValidDataAfterZero;
};

struct IBPChannelWaveCache
{
    IBPChannelWaveCache()
        : index(0)
    {
        for (int i = 0; i < MAX_CACHE_WAVE_NUM; i++)
        {
            waves[i] = 0;
            flags[i] = true;
        }
    }
    short waves[MAX_CACHE_WAVE_NUM];
    bool flags[MAX_CACHE_WAVE_NUM];
    int index;
};

class SmartIBPProviderPrivate
{
public:
    explicit SmartIBPProviderPrivate(SmartIBPProvider * const q_ptr)
        : q_ptr(q_ptr)
    {
        chn1Data.sys = 0;
        chn1Data.dia = 0;
        chn1Data.map = 0;
        chn1Data.pr = 0;
        chn1Data.sensorOff = true;

        chn2Data.sys = 0;
        chn2Data.dia = 0;
        chn2Data.map = 0;
        chn2Data.pr = 0;
        chn2Data.sensorOff = true;
    }

    /**
     * @brief calcCheckSum calculate the checksum
     * @param data the data to calculate checksum
     * @param len the data len
     * @return the checksum
     * @note when the checksum is 0xFF, need to minus 1 to avoid conflcting
     * the FRAME_HEAD_BYTE
     */
    quint8 calcCheckSum(quint8 *data, int len)
    {
        quint8 sum = 0;
        for (int i = 0; i < len; i++)
        {
            sum += data[i];
        }

        return sum == FRAME_HEAD ? sum - 1 : sum;
    }

    /**
     * @brief handlePacket handle packet data
     * @param data the packet data buffer
     * @param len length of the packet data
     */
    void handlePacket(const quint8 *data, int len);

    /**
     * @brief validPressureValue check the pressure value is valid or not
     * @param chn ibp channe data
     * @param value the input pressure value
     * @return  true if valid, otherwise, false
     */
    bool validPressureValue(IBPChannelData *chn, qint16 value)
    {
        /* check the pressure value is valid or not */
        bool valid = (value >= MIN_PRESSURE_VALUE && value <= MAX_PRESSURE_VALUE);

        /* after zero all the receive zero value should be invalid */
        if (valid && chn->checkValidDataAfterZero)
        {
            if (value == 0)
            {
                valid = false;
            }
            else
            {
                chn->checkValidDataAfterZero = false;
            }
        }
        return valid;
    }

    /**
     * @brief addWaveHelper add wave data helper function, use to lower the wave fresh rate
     * @param wave the wave value
     * @param invalid  the wave flag
     * @param chn the channel
     * @note
     * The wave data sample rate is 100HZ, lower it to 25HZ
     */
    void addWaveHelper(short wave, bool invalid, IBPChannel chn)
    {
        static IBPChannelWaveCache caches[IBP_CHN_NR];

        IBPChannelWaveCache &cache = caches[chn];

        cache.waves[cache.index] = wave;
        cache.flags[cache.index] = invalid;
        cache.index++;
        if (cache.index >= MAX_CACHE_WAVE_NUM)
        {
            for (int i = 0; i < cache.index; i++)
            {
                ibpParam.addWaveformData(cache.waves[i], cache.flags[i], chn);
            }
            cache.index = 0;
        }
    }

    /**
     * @brief setChannelType set the channel data
     * @param chn the channel data struct
     * @param type the data type
     * @param val the chanel value
     */
    void setChannelData(IBPChannelData *chn, quint8 type, qint16 val)
    {
        switch (type) {
        case DATA_TYPE_SYS:
            chn->sys = validPressureValue(chn, val) ? val : InvData();
            break;
        case DATA_TYPE_DIA:
            chn->dia = validPressureValue(chn, val) ? val : InvData();
            break;
        case DATA_TYPE_MAP:
            chn->map = validPressureValue(chn, val) ? val : InvData();
            break;
        case DATA_TYPE_PR:
            chn->pr = val;
            break;
        default:
            break;
        }
    }

    /**
     * @brief sendCmd send command to the module
     * @param cmdByte the command byte
     * @param the channel number
     */
    void sendCmd(quint8 cmdByte, quint8 chn)
    {
        unsigned char buf[6];
        buf[0] = FRAME_HEAD;
        buf[1] = 0x04;
        buf[2] = MODULE_ID;
        buf[3] = cmdByte;
        buf[4] = chn;
        buf[5] = calcCheckSum(&buf[1], buf[1]);
        q_ptr->writeData(buf, sizeof(buf));
    }

    SmartIBPProvider * const q_ptr;
    IBPChannelData chn1Data;        /* data of channel 1 */
    IBPChannelData chn2Data;        /* data of channel 2 */
};

SmartIBPProvider::SmartIBPProvider(const QString /*&port*/)
    :Provider("SMART_IBP"), pimpl(new SmartIBPProviderPrivate(this))
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    initPort(portAttr);
}

SmartIBPProvider::~SmartIBPProvider()
{
}

bool SmartIBPProvider::attachParam(Param *param)
{
    if (param->getName() == paramInfo.getParamName(PARAM_IBP))
    {
        ibpParam.setProvider(this);
        Provider::attachParam(param);
        return true;
    }

    return false;
}

void SmartIBPProvider::sendVersion()
{
    /* no supported */
}

void SmartIBPProvider::disconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_IBP);
    if (alarmSource)
    {
        alarmSource->clear();
        alarmSource->setOneShotAlarm(IBP_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    }
    if (isConnectedToParam)
    {
        ibpParam.setConnected(false);
    }
}

void SmartIBPProvider::reconnected()
{
    if (isConnectedToParam)
    {
        ibpParam.setConnected(true);
    }
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_IBP);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(IBP_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    }
}

void SmartIBPProvider::setZero(IBPChannel IBP, IBPCalibration calibration, unsigned short value)
{
    Q_UNUSED(value)

    if (calibration == IBP_CALIBRATION_ZERO)
    {
        pimpl->sendCmd(MODULE_CMD_ZERO, IBP);
    }
    else if (calibration == IBP_CALIBRATION_SET)
    {
        pimpl->sendCmd(MODULE_CMD_CALIBRATE, IBP);
    }
}

int SmartIBPProvider::getIBPWaveformSample()
{
    return 100;
}

int SmartIBPProvider::getIBPBaseLine()
{
    return 0;
}

int SmartIBPProvider::getIBPMaxWaveform()
{
    return 3000;
}

void SmartIBPProvider::dataArrived()
{
    readData();
    quint8 buf[MAX_PACKET_LENGTH];
    while (ringBuff.dataSize() > MIN_PARSE_LENGTH)
    {
        /* find frame head */
        if (ringBuff.at(0) != FRAME_HEAD)
        {
            ringBuff.pop(1);
            continue;
        }

        quint8 length = ringBuff.at(1);
        if (length != 0x06 && length != 0x0A)
        {
            /* possible length field value of packet from module is 0x06 0r 0x0A */
            qDebug() << "Invalid Packet length:" << length;
            ringBuff.pop(1);
            continue;
        }

        if (ringBuff.dataSize() < length)
        {
            /* no enough data */
            break;
        }

        ringBuff.copy(0, buf, length + 2);
        quint8 checksum = pimpl->calcCheckSum(buf + 1, length);
        if (checksum == buf[length + 1])
        {
            /* packet is valid, pop the packet data */
            ringBuff.pop(length + 2);

            pimpl->handlePacket(buf + 2, length - 1);
            // outHex(buf, length + 2);
        }
        else
        {
            qDebug() << this->getName() << "CheckSum failed!";
            ringBuff.pop(1);
        }
    }
}

void SmartIBPProviderPrivate::handlePacket(const quint8 *data, int len)
{
    Q_UNUSED(len)

    if (!q_ptr->isConnectedToParam)
    {
        return;
    }

    if (!q_ptr->connected())
    {
        ibpParam.setConnected(true);
    }
    // 发送保活帧, 有数据时调用清除连接计数器。
    q_ptr->feed();
    switch (data[0])
    {
    case MODULE_ID:
    {
        /* periodic data */
        bool ch1SensorOff = data[1] & 0x40;
        qint16 ch1Wave = ((data[1] & 0x1F) << 7) + (data[2] & 0x7F);;
        quint8 ch1Type = (data[3] & 0x60) >> 5;
        qint16 ch1Val = ((data[3] &0x1F) << 7) + (data[4] & 0x7F);
        if (ch1Type != DATA_TYPE_PR)
        {
            ch1Val = (ch1Val - 0x320) / 8;
        }

        if (ch1SensorOff || ibpParam.channelNeedZero(IBP_CHN_1))
        {
            chn1Data.sys = InvData();
            chn1Data.dia = InvData();
            chn1Data.map = InvData();
            chn1Data.pr = InvData();
        }
        else
        {
            setChannelData(&chn1Data, ch1Type, ch1Val);
        }

        if (chn1Data.sensorOff != ch1SensorOff)
        {
            chn1Data.sensorOff = ch1SensorOff;
            ibpParam.setLeadStatus(IBP_CHN_1, ch1SensorOff);
        }

        bool ch2SensorOff = data[5] & 0x40;
        qint16 ch2Wave = ((data[5] & 0x1F) << 7) + (data[6] & 0x7F);
        quint8 ch2Type = (data[7] & 0x60) >> 5;
        quint16 ch2Val = ((data[7] &0x1F) << 7) + (data[8] & 0x7F);
        if (ch2Type != DATA_TYPE_PR)
        {
            ch2Val = (ch2Val - 0x320) / 8;
        }
        else
        {
            /*
             * It seem that the pr value from channel 2 is always zero and
             * the real pr value is always on channel 1 event channel 1 is lead off.
             * So when the channel 2's pr is zero, use the pr from channel 1
             */
            if (ch2Val == 0)
            {
                ch2Val = ((data[3] &0x1F) << 7) + (data[4] & 0x7F);
            }
        }

        if (ch2SensorOff || ibpParam.channelNeedZero(IBP_CHN_2))
        {
            chn2Data.sys = InvData();
            chn2Data.dia = InvData();
            chn2Data.map = InvData();
            chn2Data.pr = InvData();
        }
        else
        {
            setChannelData(&chn2Data, ch2Type, ch2Val);
        }

        if (chn2Data.sensorOff != ch2SensorOff)
        {
            chn2Data.sensorOff = ch2SensorOff;
            ibpParam.setLeadStatus(IBP_CHN_2, ch2SensorOff);
        }

        bool chn1WaveInvalid  = ch1SensorOff;
        if (ch1Wave == INVALID_MEASURE_VALUE)
        {
            ch1Wave = q_ptr->getIBPBaseLine();
            chn1WaveInvalid = true;
        }
        else
        {
            /* the wave data should scale by 10, since the wave data unit is 0.1mmhg */
            ch1Wave = (ch1Wave - 0x320) * 10 / 8;
        }
        addWaveHelper(ch1Wave, chn1WaveInvalid, IBP_CHN_1);

        bool chn2WaveInvalid  = ch2SensorOff;
        if (ch2Wave == INVALID_MEASURE_VALUE)
        {
            ch2Wave = q_ptr->getIBPBaseLine();
            chn2WaveInvalid = true;
        }
        else
        {
            /* the wave data should scale by 10, since the wave data unit is 0.1mmhg */
            ch2Wave = (ch2Wave - 0x320) * 10 / 8;
        }
        addWaveHelper(ch2Wave, chn2WaveInvalid, IBP_CHN_2);

        if (ch1Type == DATA_TYPE_PR)
        {
            /* if all the pressure valus is invalid, set the pr valud to invaid also */
            if (chn1Data.sys == InvData() && chn1Data.dia == InvData() && chn1Data.map == InvData())
            {
                chn1Data.pr = InvData();
            }

            ibpParam.setRealTimeData(chn1Data.sys, chn1Data.dia, chn1Data.map,
                                     chn1Data.pr, IBP_CHN_1);
        }

        if (ch2Type == DATA_TYPE_PR)
        {
            /* if all the pressure valus is invalid, set the pr valud to invaid also */
            if (chn2Data.sys == InvData() && chn2Data.dia == InvData() && chn2Data.map == InvData())
            {
                chn2Data.pr = InvData();
            }
            ibpParam.setRealTimeData(chn2Data.sys, chn2Data.dia, chn2Data.map,
                                     chn2Data.pr, IBP_CHN_2);
        }
    }
        break;
    case MODULE_CMD_RSP:
    {
        /* comman respone */
        quint8 cmd = data[2];
        quint8 chn = data[3];
        quint8 result = data[4];
        if (result == 0)
        {
            qDebug() << Q_FUNC_INFO << "CMD" << cmd << "success!";
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "CMD" << cmd << "fail!";
        }

        if (cmd == IBP_CALIBRATION_ZERO)
        {
            ibpParam.setCalibrationInfo(IBP_CALIBRATION_ZERO,
                                     static_cast<IBPChannel>(chn),
                                     result == 0 ? IBP_ZERO_SUCCESS : IBP_ZERO_FAIL);
            if (chn == 0)
            {
                chn1Data.checkValidDataAfterZero = true;
            }
            else if (chn == 1)
            {
                chn2Data.checkValidDataAfterZero = true;
            }
        }
        else if (cmd == IBP_CALIBRATION_SET)
        {
            ibpParam.setCalibrationInfo(IBP_CALIBRATION_SET,
                                     static_cast<IBPChannel>(chn),
                                     result == 0 ? IBP_CALIBRATION_SUCCESS: IBP_CALIBRATION_FAIL);
        }
    }
        break;
    default:
        /* unknown packet id */
        qDebug() << Q_FUNC_INFO << "unknown packet type";
        break;
    }
}
