/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/8
 **/

#include "SmartCOProvider.h"
#include "COParam.h"
#include <QDebug>

#define FRAME_HEAD  0xA2
#define MIN_FRAME_LEN 4
#define MAX_FRAME_LEN   13


/* version message */
#define RECV_MSG_VERSION        0xFF
/* realtime waveform, 125 HZ, unit 0.01 celsius degree */
#define RECV_MSG_REALTIME_WAVE  0x10
/* data and status,  */
#define RECV_MSG_DATA_STATUS    0x20

/* measure relate command, depend on the specific command data */
#define CMD_MEASURE_SETTING     0x81
/* setting the injection volume, 1~10 */
#define CMD_SET_INJECTION_VOLUME    0x82
/* setting the injection temperature, manual mode only, unit 0.1 celsius degree */
#define CMD_SET_INJECTION_TEMP  0x83



enum SmartCOMeasureCmdId {
    SMART_CO_START_MEASURE = 0x10,
    SMART_CO_STOP_MEASURE = 0x11,

    SMART_CO_TEMP_AUTO = 0x20,    /* auto detect injection temperature */
    SMART_CO_TEMP_MANUAL = 0x21,  /* manual setting injection temperature */

    SMART_CO_MODULE_VERSION = 0xff,
};

/* measurement status 1 flags defination */
/* got measure result */
#define SMART_CO_STAT1_GOT_RESULT   (1 << 7)
/* floating pin is not connected */
#define SMART_CO_STAT1_FLOAT_PIPE_DISCONNECT (1 << 6)
/* injection temp sensor is disconnect */
#define SMART_CO_STAT1_TI_SENSOR_DISCONNECT (1 << 5)
/* data error */
#define SMART_CO_STAT1_DATA_ERROR   (1 << 4)
/* measure timeout */
#define SMART_CO_STAT1_MEASURE_TIMEOUT  (1<<3)

/* measurement status 2 flags defination */
#define SMART_CO_STAT2_BASELINE(d) (((d) & 0xC0) >> 6)
#define SMART_CO_BASELINE_NORMAL    0
/* baseline too much offset */
#define SMART_CO_BASELINE_TOO_MUCH_OFFSET 1
/* baseline too much noise */
#define SMART_CO_BASELINE_TOO_MUCH_NOISE 2
#define SMART_CO_STAT2_CURVE(d) (((d) & 0x38) >> 3)
#define SMART_CO_CURVE_NORMAL   0
#define SMART_CO_CURVE_TOO_LONG 0x01
#define SMART_CO_CURVE_TOO_SHORT    0x02
#define SMART_CO_CURVE_UNDER_BASELINE   0x03
#define SMART_CO_CURVE_TOO_HIGH 0x04
#define SMART_CO_CURVE_TOO_MUCH_PEAK    0x05
#define SMART_CO_STAT2_INJECTION_DELAY    (1 << 2)
#define SMART_CO_STAT2_INJECTION_TEMP_TOO_HIGH (1 << 1)
#define SMART_CO_STAT2_CHECK_CALC_CONST (1 << 0)


class SmartCOProviderPrivate
{
public:
    explicit SmartCOProviderPrivate(SmartCOProvider * const q_ptr) : q_ptr(q_ptr) {}


    /**
     * @brief calcChecksum calculate the data checksum
     * @param data the data buffer
     * @param len the data length
     * @return the checksum value
     */
    quint8 calcChecksum(quint8 *data, quint8 len)
    {
        quint8 ret = 0;
        for (quint8 i = 0; i < len;  i++)
        {
            ret += data[i];
        }
        return ret;
    }

    /**
     * @brief sendCmd send command to the module
     * @param cmdID the command id
     * @param data the comand data buffer
     * @param len the command data length
     */
    void sendCmd(quint8 cmdID, quint8 *data, quint8 len)
    {
        quint8 buf[MAX_FRAME_LEN];
        quint8 frameLen = MIN_FRAME_LEN + len;
        int j = 0;
        buf[j++] = FRAME_HEAD;
        buf[j++] = frameLen;
        buf[j++] = cmdID;
        for (quint8 i = 0; i < len; i++)
        {
            buf[j++] = data[i];
        }
        buf[j] = calcChecksum(buf, j);
        q_ptr->writeData(buf, frameLen);
    }

    /**
     * @brief handlePacket handle the receive packet
     * @param ID the message id
     * @param data the data buffer
     * @param length the data length
     */
    void handlePacket(quint8 ID, const quint8 *data, int length);

    /**
     * @brief setInjectionTemp set injection temperature
     * @param temp temperature in unit of 0.1 celsius degree
     */
    void setInjectionTemp(short temp);

    /**
     * @brief setInjectionVolumn set the injection volume
     * @param volume 1~10
     */
    void setInjectionVolumn(quint8 volume);

    /**
     * @brief setInjectionTempMode set injection temperature mode
     */
    void setInjectionTempMode(bool isAuto);

    /**
     * @brief startMeasure start CO measurement
     */
    void startMeasure();

    /**
     * @brief stopMeasure stop CO measurement
     */
    void stopMeasure();


    SmartCOProvider * const q_ptr;
};

SmartCOProvider::SmartCOProvider(const QString &port)
    : Provider("SMART_CO"), pimpl(new SmartCOProviderPrivate(this))
{
    Q_UNUSED(port)
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    initPort(portAttr);
}

SmartCOProvider::~SmartCOProvider()
{
}

bool SmartCOProvider::attachParam(Param *param)
{
    if (param->getName() == paramInfo.getParamName(PARAM_CO))
    {
        coParam.setProvider(this);
        Provider::attachParam(param);
        return true;
    }
    return false;
}

void SmartCOProvider::sendVersion()
{
    quint8 cmdData = SMART_CO_MODULE_VERSION;
    pimpl->sendCmd(CMD_MEASURE_SETTING, &cmdData, 1);
}

void SmartCOProvider::disconnected()
{
    if (isConnectedToParam)
    {
        coParam.setConnected(false);
    }
}

void SmartCOProvider::reconnected()
{
    if (isConnectedToParam)
    {
        coParam.setConnected(true);
    }
}

void SmartCOProvider::dataArrived()
{
    readData();
    quint8 buf[MAX_FRAME_LEN];

    while (ringBuff.dataSize() > MIN_FRAME_LEN)
    {
        /* find frame head */
        if (ringBuff.at(0) != FRAME_HEAD)
        {
            ringBuff.pop(1);
            continue;
        }

        quint8 length = ringBuff.at(1);
        if (length < MIN_FRAME_LEN || length > MAX_FRAME_LEN)
        {
            /* invalid length */
            qDebug() << Q_FUNC_INFO << "INvalid packet length:" << length;
            ringBuff.pop(1);
            continue;
        }

        if (ringBuff.dataSize() < length)
        {
            /* no enough data */
            break;
        }

        ringBuff.copy(0, buf, length);
        quint8  checksum = pimpl->calcChecksum(buf, length - 1);
        if (checksum == buf[length - 1])
        {
            /* packet is valid ,pop the packet data */
            ringBuff.pop(length);

            pimpl->handlePacket(buf[2], &buf[3], length - MIN_FRAME_LEN);
        }
        else
        {
            qDebug() << this->getName() << "Checksum failed!";
            ringBuff.pop(1);
        }
    }
}

void SmartCOProviderPrivate::handlePacket(quint8 ID, const quint8 *data, int length)
{
    Q_UNUSED(length)

    // if (!q_ptr->isConnectedToParam)
    // {
    //     return;
    // }

    // if (!q_ptr->isConnected)
    // {
    //     coParam.setConnected(true);
    // }

    switch (ID) {
    case RECV_MSG_VERSION:
    {
        short v = (data[1] << 8) + data[0];
        q_ptr->versionInfo = QString("V%1").arg(v);
    }
        break;

    case RECV_MSG_REALTIME_WAVE:
    {
        quint16 wave = (data[1] << 8) + data[0];
    }
        break;

    case RECV_MSG_DATA_STATUS:
    {
        short tb = (data[1] << 8) + data[0];
        short ti = (data[3] << 8) + data[2];
        short co = (data[5] << 8) + data[4];
        quint8 stat1 = data[6];
        quint8 stat2 = data[7];
    }
        break;
    default:
        qDebug() << Q_FUNC_INFO << "Unknown mesage id " << ID;
        break;
    }
}

void SmartCOProviderPrivate::setInjectionTemp(short temp)
{
    quint8 data[2];
    data[0] = temp & 0xFF;
    data[1] = (temp >> 8) & 0xFF;
    sendCmd(CMD_SET_INJECTION_TEMP, data, sizeof(data));
}

void SmartCOProviderPrivate::setInjectionVolumn(quint8 volume)
{
    sendCmd(CMD_SET_INJECTION_VOLUME, &volume, 1);
}

void SmartCOProviderPrivate::setInjectionTempMode(bool isAuto)
{
    quint8 data = isAuto ? SMART_CO_TEMP_AUTO : SMART_CO_TEMP_MANUAL;
    sendCmd(CMD_MEASURE_SETTING, &data, 1);
}

void SmartCOProviderPrivate::startMeasure()
{
    quint8 data = SMART_CO_START_MEASURE;
    sendCmd(CMD_MEASURE_SETTING, &data, 1);
}

void SmartCOProviderPrivate::stopMeasure()
{
    quint8 data = SMART_CO_STOP_MEASURE;
    sendCmd(CMD_MEASURE_SETTING, &data, 1);
}
