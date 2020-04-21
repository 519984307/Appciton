/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/6/4
 **/

#include "PluginProvider.h"
#include "Framework/Uart/Uart.h"
#include "IConfig.h"
#include "Framework/Utility/RingBuff.h"
#include "Provider.h"
#include <QTimerEvent>
#include <QFile>
#include "SPO2Param.h"
#include <QTimer>
#include "ParamManager.h"
#include "RainbowProvider.h"
#include "BLMCO2Provider.h"
#include "Debug.h"

#define SPO2_RAINBOW_SOH             (0xA1)  // rainbow spo2 packet header
#define SPO2_RAINBOW_EOM             (0xAF)  // rainbow spo2 packet end
#define CO2_SOH_1            (0xAA)  // co2 packet header 1
#define CO2_SOH_2            (0x55)  // co2 packet header 2
#define MIN_PACKET_LEN   5      // 最小数据包长度: SOH,Length,Type,FCS
#define CO2_FRAME_LEN   21      // frame lenght of co2 packet
#define PACKET_BUFF_SIZE 64
#define RING_BUFFER_LENGTH 4096
#define READ_PLUGIN_PIN_INTERVAL        (200)   // 200ms读一次插件管脚
#define CHECK_CONNECT_TIMER_PERIOD         (200)   // 检查连接定时器周期
#define MODULE_RESET_DURATION          (500)
#define BAUDRATE_SWITCH_DURATION       (500)
#define WORKING_BAUDRATE_9600          (9600)
#define WORKING_BAUDRATE_57600         (57600)
#define MAX_PACKET_LEN          (40)

class PluginProviderPrivate
{
public:
    PluginProviderPrivate(const QString &name, PluginProvider *const q_ptr)
        : q_ptr(q_ptr),
          name(name),
          uart(new Uart(q_ptr)),
          ringBuff(RING_BUFFER_LENGTH),
          workingProvider(NULL),
          pluginDetectTimerID(-1),
          baudrateSwitchTimerID(-1),
          ConnectionCheckTimerID(-1),
          moudleResetTimerID(-1),
          curDetectBaudrate(WORKING_BAUDRATE_9600),
          connLostTickCounter(0),
          lastPluginState(false)
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
            return;
        }

        ringBuff.push(buf, ret);
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

    /**
     * @brief setupProvider setup the provider
     * @param type
     * @note
     * Should be invoked when we know the plugin provider type
     */
    void setupProvider(PluginProvider::PlugInType type)
    {
        /* module is already known before reset timer expired */
        if (moudleResetTimerID != -1)
        {
            q_ptr->killTimer(moudleResetTimerID);
            moudleResetTimerID = -1;
        }

        /* module is already known, no need to switch baudrate */
        if (baudrateSwitchTimerID != -1)
        {
            q_ptr->killTimer(baudrateSwitchTimerID);
            baudrateSwitchTimerID = -1;
        }

        if (NULL == dataHandlers[type])
        {
            /* initilize the provider */
            if (!initPluginModule(type))
            {
                return;
            }
        }

        workingProvider = dataHandlers[type];
        qDebug() << Q_FUNC_INFO << "Setup working provider" << type;

        if (ConnectionCheckTimerID == -1)
        {
            /* start the conneciton checking timer */
            ConnectionCheckTimerID = q_ptr->startTimer(CHECK_CONNECT_TIMER_PERIOD);

            /* need to swithc the baudrate for rainbow */
            if (type == PluginProvider::PLUGIN_TYPE_SPO2)
            {
                spo2Param.initModule(true);
                QTimer::singleShot(50, q_ptr, SLOT(startInitModule()));
            }
        }
    }

    bool initPluginModule(PluginProvider::PlugInType type)
    {
        if (type == PluginProvider::PLUGIN_TYPE_SPO2 && systemManager.isSupport(CONFIG_SPO2))
        {
            Provider *provider = NULL;
            provider = new RainbowProvider("RAINBOW_SPO2PlugIn", true);
            paramManager.addProvider(provider);
            if (systemManager.getCurWorkMode() != WORK_MODE_DEMO)
            {
                provider->attachParam(paramManager.getParam(PARAM_SPO2));
            }
            dataHandlers[type] = provider;
            return true;
        }
        return false;
    }

    /**
     * @brief isPluginConnected check whether a plugin is connected
     * @return true when a plugin is connected;
     * @note
     * When a plugin is connectd, the detect pin will be low level
     */
    bool isPluginConnected()
    {
        QByteArray data;
        QFile callFile("/sys/class/pmos/plugin_insert_recognition");
        if (!callFile.open(QIODevice::ReadWrite))
        {
            qDebug() << "fail to open plugin file";
            return -1;
        }
        data = callFile.read(1);
        return data.toInt() == 0;
    }

    /**
     * @brief sendRainbowBaudratetCmd send randbow spo2 baudrate command
     * @param baudrate the baudrate
     * @note
     * When the badurate is identical the rainbow current running baudrate,
     * the module will response NACK. We could send this command to get the module
     * response during detection
     */
    void sendRainbowBaudrateCmd(PluginProvider::PacketPortBaudrate baudrate)
    {
        int index = 0;
        unsigned char data[2] = {0x23, baudrate};
        unsigned char buff[PACKET_BUFF_SIZE] = {0};
        buff[index++] = SPO2_RAINBOW_SOH;
        buff[index++] = sizeof(data);
        for (unsigned int i = 0; i < sizeof(data); i++)
        {
            buff[index++] = data[i];
        }
        buff[index++] = calcChecksum(data, sizeof(data));
        buff[index++] = SPO2_RAINBOW_EOM;
        uart->write(buff, index);
    }

    PluginProvider *const q_ptr;
    QString name;
    Uart *uart;
    QMap<PluginProvider::PlugInType, Provider *> dataHandlers;
    RingBuff<unsigned char> ringBuff;
    static QMap<QString, PluginProvider *> plugInProviders;
    Provider *workingProvider;
    int pluginDetectTimerID;
    int baudrateSwitchTimerID;
    int ConnectionCheckTimerID;
    int moudleResetTimerID;         /* module reset timer ID */
    unsigned int curDetectBaudrate;
    unsigned int connLostTickCounter;
    bool lastPluginState;       /* record the last plugin state, true means the last plugin check result was connected*/

private:
    PluginProviderPrivate(const PluginProviderPrivate &);  // use to pass the cpplint check only, no implementation
};

QMap<QString, PluginProvider *> PluginProviderPrivate::plugInProviders;

PluginProvider::PluginProvider(const QString &name, QObject *parent)
    : QObject(parent), d_ptr(new PluginProviderPrivate(name, this))
{
    UartAttrDesc portAttr(9600, 8, 'N', 1);
    d_ptr->initPort(portAttr);
    d_ptr->pluginDetectTimerID = startTimer(READ_PLUGIN_PIN_INTERVAL);
    connect(d_ptr->uart, SIGNAL(activated(int)), this, SLOT(dataArrived()));
    connect(&systemManager, SIGNAL(workModeChanged(WorkMode)), this, SLOT(onWorkModeChanged(WorkMode)));
}

PluginProvider::~PluginProvider()
{
    delete d_ptr;
}

QString PluginProvider::getName() const
{
    return d_ptr->name;
}

void PluginProvider::connectProvider(PluginProvider::PlugInType type, Provider *provider)
{
    if (type == PLUGIN_TYPE_INVALID)
    {
        qDebug() << "provider " << provider->getName() << " connect to invalid packet type!";
    }
    d_ptr->dataHandlers[type] = provider;
}

int PluginProvider::sendData(PluginProvider::PlugInType type, const unsigned char *buff, int len)
{
    if (type == PLUGIN_TYPE_SPO2)
    {
        return d_ptr->uart->write(buff, len);
    }
    return d_ptr->uart->write(buff, len);
}

void PluginProvider::addPlugInProvider(PluginProvider *plugInProvider)
{
    if (plugInProvider)
    {
        PluginProviderPrivate::plugInProviders.insert(plugInProvider->getName(), plugInProvider);
    }
}

PluginProvider *PluginProvider::getPlugInProvider(const QString &name)
{
    return PluginProviderPrivate::plugInProviders.value(name, NULL);
}

bool PluginProvider::setPacketPortBaudrate(PluginProvider::PlugInType type, PluginProvider::PacketPortBaudrate baud)
{
    if (type == PLUGIN_TYPE_SPO2)
    {
        int index = 0;
        unsigned char data[2] = {0x23, baud};
        unsigned char buff[PACKET_BUFF_SIZE] = {0};
        buff[index++] = SPO2_RAINBOW_SOH;
        buff[index++] = sizeof(data);
        for (unsigned int i = 0; i < sizeof(data); i++)
        {
            buff[index++] = data[i];
        }
        buff[index++] = d_ptr->calcChecksum(data, sizeof(data));
        buff[index++] = SPO2_RAINBOW_EOM;
        return d_ptr->uart->write(buff, index);
    }
    return true;
}

void PluginProvider::updateUartBaud(unsigned int baud)
{
    UartAttrDesc attr(baud, 8, 'N', 1);
    d_ptr->uart->updateSetting(attr);
}

void PluginProvider::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == d_ptr->pluginDetectTimerID)
    {
        if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
        {
            return;
        }

        /* plugin connectd state is changed or we need restart scanning */
        if (d_ptr->lastPluginState != d_ptr->isPluginConnected())
        {
            bool hasPluginConnected = d_ptr->isPluginConnected();
            if (hasPluginConnected)
            {
                /* haven't detect any module */
                if (d_ptr->workingProvider == NULL)
                {
                    // clear the buffer
                    d_ptr->ringBuff.clear();
                    d_ptr->moudleResetTimerID = startTimer(MODULE_RESET_DURATION);
                    /* reset baudrate */
                    d_ptr->curDetectBaudrate = WORKING_BAUDRATE_9600;
                    updateUartBaud(d_ptr->curDetectBaudrate);
                }
                qDebug() << Q_FUNC_INFO << "Plugin connected";
            }
            else
            {
                /* stop the module reset timer */
                if (d_ptr->moudleResetTimerID)
                {
                    killTimer(d_ptr->moudleResetTimerID);
                    d_ptr->moudleResetTimerID = -1;
                }

                /* stop baudrate switching timer */
                if (d_ptr->baudrateSwitchTimerID != -1)
                {
                    killTimer(d_ptr->baudrateSwitchTimerID);
                    d_ptr->baudrateSwitchTimerID = -1;
                }

                /* stop connection checking timer */
                if (d_ptr->ConnectionCheckTimerID != -1)
                {
                    d_ptr->connLostTickCounter = 0;
                    killTimer(d_ptr->ConnectionCheckTimerID);
                    d_ptr->ConnectionCheckTimerID = -1;
                }

                d_ptr->workingProvider = NULL;
                qDebug() << Q_FUNC_INFO << "Plugin disconnected";
            }

            d_ptr->lastPluginState = hasPluginConnected;
        }
    }
    else if (ev->timerId() == d_ptr->moudleResetTimerID)
    {
        /* module should reset complete, stop reset timer */
        killTimer(d_ptr->moudleResetTimerID);
        d_ptr->moudleResetTimerID = -1;

        if (d_ptr->workingProvider)
        {
            /* moudle has beed detected during reset time */
            return;
        }

        /* send test command, we start test with the baudrate of 9600 */
        d_ptr->sendRainbowBaudrateCmd(BAUDRATE_9600);

        d_ptr->baudrateSwitchTimerID = startTimer(BAUDRATE_SWITCH_DURATION);
    }
    else if (ev->timerId() == d_ptr->baudrateSwitchTimerID)
    {
        if (d_ptr->curDetectBaudrate == WORKING_BAUDRATE_9600)
        {
            d_ptr->curDetectBaudrate = WORKING_BAUDRATE_57600;
            updateUartBaud(d_ptr->curDetectBaudrate);
            d_ptr->sendRainbowBaudrateCmd(BAUDRATE_57600);
        }
        else
        {
            /* no any response, stop detection */
            killTimer(d_ptr->baudrateSwitchTimerID);
            d_ptr->baudrateSwitchTimerID = -1;
        }
    }
    else if (ev->timerId() == d_ptr->ConnectionCheckTimerID)
    {
        if (d_ptr->isPluginConnected())
        {
            d_ptr->connLostTickCounter++;
            if (d_ptr->connLostTickCounter > 3 * (1000 / CHECK_CONNECT_TIMER_PERIOD ))
            {
                /* 3 seconds has passed without receive any data */
                /* the moudle might be disconectd */
                d_ptr->lastPluginState = false;
                d_ptr->connLostTickCounter = 0;
                d_ptr->workingProvider = NULL;
                killTimer(d_ptr->ConnectionCheckTimerID);
                d_ptr->ConnectionCheckTimerID = -1;
                qDebug() << Q_FUNC_INFO << "Communication stop";
            }
        }
        else
        {
            /* already disconnect, stop the timer */
            d_ptr->connLostTickCounter = 0;
            d_ptr->workingProvider = NULL;
            killTimer(d_ptr->ConnectionCheckTimerID);
            d_ptr->ConnectionCheckTimerID = -1;
            qDebug() << Q_FUNC_INFO << "Plugin disconnect, communicaton stop";
        }
    }
}

void PluginProvider::dataArrived()
{
    d_ptr->connLostTickCounter = 0;

    d_ptr->readData();  // 读取数据到RingBuff中

    if (d_ptr->workingProvider)
    {
        /* the working provier is alreay known, pass data directory to the provider */
        while (d_ptr->ringBuff.dataSize() > 0)
        {
            unsigned char buff[PACKET_BUFF_SIZE];
            int readNum = d_ptr->ringBuff.copy(0, buff, PACKET_BUFF_SIZE);
            d_ptr->ringBuff.pop(readNum);
            d_ptr->workingProvider->dataArrived(buff, readNum);
        }
        return;
    }

    /* check the working provider */
    while (d_ptr->ringBuff.dataSize() >= MIN_PACKET_LEN)
    {
        // 如果查询不到帧头，移除ringbuff缓冲区最旧的数据，下次继续查询
        if (d_ptr->ringBuff.at(0) == SPO2_RAINBOW_SOH)
        {
            // 如果查询不到帧尾，移除ringbuff缓冲区最旧的数据，下次继续查询
            unsigned char len = d_ptr->ringBuff.at(1);     // data field length
            // 1 frame head + 1 len byte + data length + 1 checksum + 1 frame end
            unsigned char totalLen = 2 + len + 2;

            if (totalLen > MAX_PACKET_LEN)
            {
                qDebug() << "packet too large";
                d_ptr->ringBuff.pop(1);
                continue;
            }

            if (d_ptr->ringBuff.dataSize() < totalLen)
            {
                // no enough data
                break;
            }

            if (d_ptr->ringBuff.at(totalLen - 1) != SPO2_RAINBOW_EOM)
            {
                d_ptr->ringBuff.pop(1);
                continue;
            }

            // 将ringbuff中数据读取到临时缓冲区buff中,并移除ringbuff的旧数据
            unsigned char buff[PACKET_BUFF_SIZE] = {0};
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
                // d_ptr->handlePacket(buff, len + 4, PLUGIN_TYPE_SPO2);
                d_ptr->setupProvider(PLUGIN_TYPE_SPO2);
                break;
            }
            else
            {
                outHex(buff, PACKET_BUFF_SIZE);
                debug("0x%02x", csum);
                debug("FCS error (%s)\n", qPrintable(getName()));
                d_ptr->ringBuff.pop(1);
            }
            continue;
        }
        else if ((d_ptr->ringBuff.at(0) == 0xAA) && (d_ptr->ringBuff.at(1) == 0x55))
        {
            if (d_ptr->ringBuff.dataSize() >= CO2_FRAME_LEN)
            {
                unsigned char sum = 0;
                for (int i = 2; i < CO2_FRAME_LEN; i++)
                {
                    sum += d_ptr->ringBuff.at(i);
                }

                if (sum == 0)
                {
                    d_ptr->setupProvider(PLUGIN_TYPE_CO2);
                    break;
                }
                else
                {
                    /* checksum fail */
                    d_ptr->ringBuff.pop(1);
                }
            }
            else
            {
                /* no enough data */
                break;
            }
        }
        else
        {
            d_ptr->ringBuff.pop(1);
        }
    }
}

void PluginProvider::changeBaudrate()
{
    setPacketPortBaudrate(PLUGIN_TYPE_SPO2, BAUDRATE_57600);
}

void PluginProvider::startInitModule()
{
    if (d_ptr->workingProvider == d_ptr->dataHandlers[PLUGIN_TYPE_SPO2])
    {
        d_ptr->sendRainbowBaudrateCmd(BAUDRATE_57600);
    }
}

void PluginProvider::onWorkModeChanged(WorkMode curMode)
{
    if (curMode == WORK_MODE_NORMAL)
    {
        if (d_ptr->dataHandlers[PLUGIN_TYPE_SPO2] != NULL)
        {
            d_ptr->dataHandlers[PLUGIN_TYPE_SPO2]->attachParam(paramManager.getParam(PARAM_SPO2));
        }
        /* force disconnect the plugin, if the plugin is connected, the state will update in the timeEvent */
        d_ptr->lastPluginState = false;
    }
    else if (curMode == WORK_MODE_DEMO)
    {
        if (d_ptr->dataHandlers[PLUGIN_TYPE_SPO2] != NULL)
        {
            d_ptr->dataHandlers[PLUGIN_TYPE_SPO2]->detachParam(paramManager.getParam(PARAM_SPO2));
        }
    }
}
