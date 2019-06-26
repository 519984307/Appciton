/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/6/4
 **/

#include "PlugInProvider.h"
#include "Uart.h"
#include "IConfig.h"
#include "RingBuff.h"
#include "crc8.h"
#include "Provider.h"
#include <QTimerEvent>
#include <QFile>
#include "SPO2Param.h"
#include <QTimer>
#include "ParamManager.h"
#include "RainbowProvider.h"
#include "SPO2Alarm.h"
#include "BLMCO2Provider.h"
#include "CO2Param.h"

#define SPO2_SOH             (0xA1)  // rainbow spo2 packet header
#define SPO2_EOM             (0xAF)  // rainbow spo2 packet end
#define CO2_SOH_1            (0xAA)  // co2 packet header 1
#define CO2_SOH_2            (0x55)  // co2 packet header 2
#define MIN_PACKET_LEN   5      // 最小数据包长度: SOH,Length,Type,FCS
#define PACKET_BUFF_SIZE 64
#define RING_BUFFER_LENGTH 4096
#define MAXIMUM_PACKET_SIZE 256 // largest packet size, should be larger enough
#define READ_PLUGIN_PIN_INTERVAL        (500)   // 500ms读一次插件管脚
#define RUN_BAUD_RATE_9600          (9600)
#define RUN_BAUD_RATE_115200        (115200)

enum PluginStatus
{
    PLUGIN_STATUS_CO2 = 0,
    PLUGIN_STATUS_SPO2 = 1
};

enum PacketPortCommand
{
    PORT_CMD_RESET  = 0x10,         // reset port
    PORT_CMD_RESET_RSP = 0x11,      // reset port response
    PORT_CMD_BAUDRATE = 0x12,       // baudrate
    PORT_CMD_BAUDRATE_RSP = 0x13,   // baudrate
};

class PlugInProviderPrivate
{
public:
    PlugInProviderPrivate(const QString &name, PlugInProvider *const q_ptr)
        : q_ptr(q_ptr),
          isLastSOHPaired(false),
          name(name),
          uart(new Uart(q_ptr)),
          ringBuff(RING_BUFFER_LENGTH),
          pluginTimerID(-1),
          baudrateTimerID(-1),
          baudrate(RUN_BAUD_RATE_9600)
    {
    }

    bool initPort(const UartAttrDesc &desc)
    {
        QString port;
        machineConfig.getStrValue(name + "Port", port);
        debug("%s", qPrintable(name));
        debug("%s", qPrintable(port));
        return uart->initPort(port, desc, true);
    }

    void readData()
    {
        unsigned char buf[1024];
        int ret = uart->read(buf, sizeof(buf));
        if (ret < 0)
        {
    //        debug("Data read error!\n");
            return;
        }

        ringBuff.push(buf, ret);
    }

    bool checkPacketValid(const unsigned char *data, unsigned int len)
    {
        if ((NULL == data) || (len < MIN_PACKET_LEN))
        {
            debug("Invalid packet!\n");
            debug("%s: FCS not match!\n", qPrintable(name));
            return false;
        }

        unsigned char crc = calcCRC(data, (len - 1));
        if (data[len - 1] != crc)
        {
            // outHex(data, (int)len);
            // debug("%s: FCS not match : %x!\n", qPrintable(getName()), crc);
            return false;
        }

        return true;
    }

    unsigned char calcChecksum(const unsigned char *data, int len)
    {
        unsigned char sum = 0;
        for (int i = 0; i < len; i++)
        {
            sum = sum + data[i];
        }
        sum = sum & 0xFF;
        return sum;
    }

    void handlePacket(unsigned char *data, int len, PlugInProvider::PlugInType type)
    {
        if (baudrateTimerID != -1)
        {
            q_ptr->killTimer(baudrateTimerID);
            baudrateTimerID = -1;
            baudrate = RUN_BAUD_RATE_9600;
        }
        if (NULL == dataHandlers[type])
        {
            // 初始化provider;
            initPluginModule(type);
            return;
        }
        dataHandlers[type]->dataArrived(data, len);
    }

    void initPluginModule(PlugInProvider::PlugInType type)
    {
        Provider *provider = NULL;
        if (type == PlugInProvider::PLUGIN_TYPE_SPO2)
        {
            provider = new RainbowProvider("RAINBOW_SPO2PlugIn", true);
            paramManager.addProvider(*provider);
            provider->attachParam(*paramManager.getParam(PARAM_SPO2));
        }
        else if (type == PlugInProvider::PLUGIN_TYPE_CO2)
        {
            provider = new BLMCO2Provider();
            paramManager.addProvider(*provider);
            provider->attachParam(*paramManager.getParam(PARAM_CO2));
        }
    }

    int readPluginPinSta()
    {
        QByteArray data;
        QFile callFile("/sys/class/pmos/plugin_insert_recognition");
        if (!callFile.open(QIODevice::ReadWrite))
        {
            qDebug() << "fail to open plugin file";
            return -1;
        }
        data = callFile.read(1);
        return data.toInt();
    }

    PlugInProvider *const q_ptr;
    bool isLastSOHPaired; // 遗留在ringBuff最后一个数据（该数据为SOH）是否已经剃掉了多余的SOH
    QString name;
    Uart *uart;
    QMap<PlugInProvider::PlugInType, Provider *> dataHandlers;
    RingBuff<unsigned char> ringBuff;
    static QMap<QString, PlugInProvider *> plugInProviders;
    int pluginTimerID;
    int baudrateTimerID;
    unsigned int baudrate;

private:
    PlugInProviderPrivate(const PlugInProviderPrivate &);  // use to pass the cpplint check only, no implementation
};

QMap<QString, PlugInProvider *> PlugInProviderPrivate::plugInProviders;

PlugInProvider::PlugInProvider(const QString &name, QObject *parent)
    : QObject(parent), d_ptr(new PlugInProviderPrivate(name, this))
{
    UartAttrDesc portAttr(9600, 8, 'N', 1);
    d_ptr->initPort(portAttr);
    d_ptr->pluginTimerID = startTimer(READ_PLUGIN_PIN_INTERVAL);
    connect(d_ptr->uart, SIGNAL(activated(int)), this, SLOT(dataArrived()));
}

PlugInProvider::~PlugInProvider()
{
    delete d_ptr;
}

QString PlugInProvider::getName() const
{
    return d_ptr->name;
}

void PlugInProvider::connectProvider(PlugInProvider::PlugInType type, Provider *provider)
{
    if (type == PLUGIN_TYPE_INVALID)
    {
        qDebug() << "provider " << provider->getName() << " connect to invalid packet type!";
    }
    d_ptr->dataHandlers[type] = provider;
}

int PlugInProvider::sendData(PlugInProvider::PlugInType type, const unsigned char *buff, int len)
{
    if (type == PLUGIN_TYPE_SPO2)
    {
        return d_ptr->uart->write(buff, len);
    }
    return d_ptr->uart->write(buff, len);
}

void PlugInProvider::addPlugInProvider(PlugInProvider *plugInProvider)
{
    if (plugInProvider)
    {
        PlugInProviderPrivate::plugInProviders.insert(plugInProvider->getName(), plugInProvider);
    }
}

PlugInProvider *PlugInProvider::getPlugInProvider(const QString &name)
{
    return PlugInProviderPrivate::plugInProviders.value(name, NULL);
}

bool PlugInProvider::setPacketPortBaudrate(PlugInProvider::PlugInType type, PlugInProvider::PacketPortBaudrate baud)
{
    if (type == PLUGIN_TYPE_SPO2)
    {
        int index = 0;
        unsigned char data[2] = {0x23, baud};
        unsigned char buff[PACKET_BUFF_SIZE] = {0};
        buff[index++] = SPO2_SOH;
        buff[index++] = sizeof(data);
        for (unsigned int i = 0; i < sizeof(data); i++)
        {
            buff[index++] = data[i];
        }
        buff[index++] = d_ptr->calcChecksum(data, sizeof(data));
        buff[index++] = SPO2_EOM;
        return d_ptr->uart->write(buff, index);
    }
    return true;
}

void PlugInProvider::updateUartBaud(unsigned int baud)
{
    UartAttrDesc attr(baud, 8, 'N', 1);
    d_ptr->uart->updateSetting(attr);
}

void PlugInProvider::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == d_ptr->pluginTimerID)
    {
        static int pluginSta = 1;
        if (pluginSta != d_ptr->readPluginPinSta())
        {
            if (pluginSta == 1)
            {
                updateUartBaud(d_ptr->baudrate);
                spo2Param.initPluginModule();                 // 初始化SpO2插件模块
                QTimer::singleShot(1000, this, SLOT(changeBaudrate())); // 预留rainbow模块重启时间
                d_ptr->baudrateTimerID = startTimer(1500);
            }
            pluginSta = d_ptr->readPluginPinSta();
        }
    }
    else if (ev->timerId() == d_ptr->baudrateTimerID)
    {
        if (d_ptr->baudrate == RUN_BAUD_RATE_9600)
        {
            d_ptr->baudrate = RUN_BAUD_RATE_115200;
        }
        else if (d_ptr->baudrate == RUN_BAUD_RATE_115200)
        {
            d_ptr->baudrate = RUN_BAUD_RATE_9600;
            killTimer(d_ptr->baudrateTimerID);
            d_ptr->baudrateTimerID = -1;
            return;
        }
        updateUartBaud(d_ptr->baudrate);
    }
}

#define ArraySize(arr) ((int)(sizeof(arr)/sizeof(arr[0])))      // NOLINT

void PlugInProvider::dataArrived()
{
    d_ptr->readData(); // 读取数据到RingBuff中

    unsigned char buff[PACKET_BUFF_SIZE];
    while (d_ptr->ringBuff.dataSize() >= MIN_PACKET_LEN)
    {
        // 如果查询不到帧头，移除ringbuff缓冲区最旧的数据，下次继续查询
        if (d_ptr->ringBuff.at(0) == SPO2_SOH)
        {
            // 如果查询不到帧尾，移除ringbuff缓冲区最旧的数据，下次继续查询
            unsigned char len = d_ptr->ringBuff.at(1);     // data field length
            unsigned char totalLen = 2 + len + 2;   // 1 frame head + 1 len byte + data length + 1 checksum + 1 frame end

            if (d_ptr->ringBuff.dataSize() < totalLen)
            {
                // no enough data
                break;
            }

            if (d_ptr->ringBuff.at(totalLen - 1) != SPO2_EOM)
            {
                d_ptr->ringBuff.pop(1);
                continue;
            }

            // 将ringbuff中数据读取到临时缓冲区buff中,并移除ringbuff的旧数据
            for (int i = 0; i < totalLen; i++)
            {
                buff[i] = d_ptr->ringBuff.at(0);
                d_ptr->ringBuff.pop(1);
            }

            // 计算帧的校验码
            unsigned char csum = d_ptr->calcChecksum(&buff[2], len);

            // 如果求和检验码匹配，则进一步处理数据包，否则丢弃最旧数据
            if (csum == buff[totalLen - 2])
            {
                d_ptr->handlePacket(buff, len + 4, PLUGIN_TYPE_SPO2);
            }
            else
            {
                outHex(buff, PACKET_BUFF_SIZE);
                debug("0x%02x", csum);
                debug("FCS error (%s)\n", qPrintable(getName()));
                d_ptr->ringBuff.pop(1);
            }
            d_ptr->ringBuff.pop(1);
            continue;
        }
        else if ((d_ptr->ringBuff.at(0) == 0xAA) && (d_ptr->ringBuff.at(1) == 0x55))
        {
            int len = 21;
            int i = 0;
            for (; i < len; i++)
            {
                buff[i] = d_ptr->ringBuff.at(0);
                d_ptr->ringBuff.pop(1);
            }
            d_ptr->handlePacket(buff, i, PLUGIN_TYPE_CO2);
        }
        else
        {
            d_ptr->ringBuff.pop(1);
        }
    }
}

void PlugInProvider::changeBaudrate()
{
    setPacketPortBaudrate(PLUGIN_TYPE_SPO2, BAUDRATE_57600);
}
