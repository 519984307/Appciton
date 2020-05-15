/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/19
 **/

#include "BLMMessageHandler.h"
#include <QThread>
#include <QDateTime>
#include <QList>
#include <QTimerEvent>
#include <time.h>
#include "JSONParser.h"
#include "BLMMessageDefine.h"
#include "WebSockets/websocket.h"
#include "JSONSerializer.h"
#include "ConfigManagerInterface.h"
#include "Framework/Config/Config.h"
#include "ParamManager.h"
#include "ParamPreProcessor.h"
#include "Provider.h"
#include "Params/ECGParam/ECGProviderIFace.h"
#include "Params/ECGParam/ECGParamInterface.h"
#include "Params/RESPParam/RESPParamInterface.h"
#include "Params/CO2Param/CO2ParamInterface.h"
#include "Params/SPO2Param/SPO2ParamInterface.h"
#include "Params/SPO2Param/SPO2ProviderIFace.h"
#include <QMap>
#include <QVariantList>
#include "AlarmSourceManager.h"
#include "NIBPDefine.h"
#include "TEMPDefine.h"
#include "PatientManager.h"

#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif

#define SEND_DATA_INTERVAL  200         /* 200 ms */
#define WAVE_COLLECT_DURATION 1000      /* 1000 ms */
#define TREND_COLLECT_DURATION 5000     /* 5000 ms */

static const char *blmMessageNames[BLM_MSG_NR] = {
    "None",
    "DeviceConfiguration",
    "WaveConfigs",
    "ContinueWaveform",
    "TrendConfigs",
    "Trend",
    "PatientInfo"
};

struct ConfigHandler
{
    ConfigHandler(): msgId(BLM_MSG_NONE), handle(NULL) {}

    ConfigHandler(BLMMessageID id, void (*handle)(const QString &val))
        : msgId(id), handle(handle)
    {
    }

    BLMMessageID msgId;
    void (*handle)(const QString &val);
};

#define INSERT_CONF_HANDLER(index, msg, cb) configHandlers.insert(QString(index), ConfigHandler(msg, cb))

/**
 * @brief The ParamDataCollector class  collect param trend and wave data
 */
class ParamDataCollector : public ParamPreProcessor
{
public:
    explicit ParamDataCollector(BLMMessageHandlerPrivate *handler)
        : m_handler(handler){}

    /* reimplement */
    int preProcessData(ParamID paramID, SubParamID subParamID, int value);

    /* reimplement */
    int preProcessData(ParamID paramID, const QString &name, int value);

    /* reimplement */
    void preProcessWave(WaveformID waveID, int *wave, int *flag);

    BLMMessageHandlerPrivate *m_handler;
};

class BLMMessageHandlerPrivate
{
public:
    explicit BLMMessageHandlerPrivate(BLMMessageHandler * const q_ptr)
        : q_ptr(q_ptr), socket(NULL), thread(NULL), paramCollector(new ParamDataCollector(this)),
          jsonSerializer(NULL), jsonParser(NULL), connectedTime(0), sendMessageTimerId(-1),
          waveCollectTimer(-1), trendCollectTimer(-1), connected(false)
    {
        instance = this;

        devConfig.msgVer = 1;   /* dev config message version 1 */

        INSERT_CONF_HANDLER("DateTime|DateFormat", BLM_MSG_DEV_CONF, handleDateFormat);
        INSERT_CONF_HANDLER("DateTime|TImeFormat", BLM_MSG_DEV_CONF, handleTimeFormat);

        paramsToCollectData << PARAM_ECG << PARAM_SPO2 << PARAM_RESP << PARAM_CO2
                            << PARAM_NIBP << PARAM_TEMP;
    }

    ~BLMMessageHandlerPrivate();

    /**
     * @brief waveIdToBLMWaveIdHelper convert the native wave id to BLM WAVE ID
     * @param waveID the native wave id
     * @return the BLM wave ID
     */
    BLMWaveformID waveIdToBLMWaveIdHelper(WaveformID waveID);

    /**
     * @brief unitToBLMUnitHelper convert the native unit to BLM unit
     * @param unit the native unit
     * @return the BLM wave ID
     */
    BLMUnit unitToBLMUnitHelper(UnitType unit);

    /**
     * @brief sendQVariantMap send vairant map to server
     * @param map
     */
    void sendQVariantMap(const QVariantMap &map);

    /**
     * @brief generateMsgHdr generate the message header
     * @param msgID the message id
     * @return the message header
     */
    QVariantMap generateMsgHdr(const BLMMessageID &msgID);

    /**
     * @brief collectDevConfiguration collect the device configurate info
     */
    void collectDevConfiguration();

    /**
     * @brief getDeviceConfig get the device config
     * @return the device config variant map
     */
    QVariantMap getDeviceConfig();

    /**
     * @brief collectPatientInfo collect the patient information
     */
    void collectPatientInfo();

    /**
     * @brief getPatientInfo get the patient information
     * @return
     */
    QVariantMap getPatientInfo();

    /**
     * @brief collectWaveConfig collect the wave configuration
     */
    void collectWaveConfig();

    /**
     * @brief getWaveConfigs get the wave configs
     * @return the wave configures
     */
    QVariantMap getWaveConfigs();

    /**
     * @brief collectTrendConfig collect the trend configuration
     */
    void collectTrendConfig();

    /**
     * @brief getTrendConfigs get the trend configs
     * @return the trend configures
     */
    QVariantMap getTrendConfigs();

    /**
     * @brief startWaveCollection start the wave collection
     */
    void startWaveCollection();

    /**
     * @brief stopWaveCollection stop the wave collection
     */
    void stopWaveCollection();

    /**
     * @brief startTrendCollection start the trend collection
     */
    void startTrendCollection();

    /**
     * @brief stopTrendCollection stop the trend collection
     */
    void stopTrendCollection();

    /**
     * @brief clearCollectedWaveData clear the collected wave data
     */
    void clearCollectedWaveData();

    /**
     * @brief getContinuousWaves get the continuous wave
     * @return  the continuous wave message body
     */
    QVariantMap getContinuousWaves();

    /**
     * @brief getTrendData get param trend data
     * @return the trend data message body
     */
    QVariantMap getTrendData();

    /**
     * @brief paramIdConvertParam trend param id convert param id
     * @param id trend param id
     * @return param id
     */
    ParamID paramIdConvertParam(BLMTrendParamID id);

    /**
     * @brief addPendingMessage add a new message to the pending sending queue
     * @param msg the message id
     * @note if the message is alreay in the queue, it won't be added
     */
    void addPendingMessage(const BLMMessageID &msg)
    {
        if (!pendingMsgQueue.contains(msg))
        {
            pendingMsgQueue.append(msg);
        }
    }

    /**
     * @brief systcmConfigChangedCallback handle the sysconfig change
     * @param index
     * @param val
     */
    static void sysConfigCallback(const QString &index, const QString &val);

    /* device Config related config handler */
    static void handleDateFormat(const QString &val)
    {
        instance->devConfig.dateFormat = static_cast<BLMDateFormat>(val.toInt());
    }

    static void handleTimeFormat(const QString &val)
    {
        instance->devConfig.timeFormat = static_cast<BLMTimeFormat>(val.toInt());
    }

    BLMMessageHandler * const q_ptr;
    WebSocket *socket;
    QThread *thread;
    ParamDataCollector * paramCollector;
    JSONSerializer *jsonSerializer;
    JSONParser *jsonParser;
    quint32 connectedTime;  /* record the duration we has connected to the server */
    int sendMessageTimerId;
    int waveCollectTimer;    /* wave collection timer id */
    int trendCollectTimer;      /* trend collection timer id */
    bool connected;

    BLMMessageDevConfig devConfig;
    QMap<BLMWaveformID, BLMWaveConfig> waveConfigs;
    QMap<QString, ConfigHandler> configHandlers;
    QVariantMap contiWaveMsgHdr;     /* the continuous wave MessageHeader */
    QList<BLMMessageID> pendingMsgQueue;
    QMap<BLMWaveformID, QVariantList> waveSampleList;
    QMap<BLMWaveformID, QVariantList> waveStatusList;

    QMap<BLMTrendParamID, BLMTrendConfig> trendConfigs;

    BLMPatientInfo patientInfo;

    QList<ParamID> paramsToCollectData;

    static BLMMessageHandlerPrivate *instance;
private:
    BLMMessageHandlerPrivate(const BLMMessageHandlerPrivate &o);
};

BLMMessageHandlerPrivate *BLMMessageHandlerPrivate::instance = NULL;

BLMMessageHandler::BLMMessageHandler(WebSocket *socket, QObject *parent)
    : QObject(parent), pimpl(new BLMMessageHandlerPrivate(this))
{
    pimpl->socket = socket;

    connect(pimpl->socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(pimpl->socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    pimpl->thread = new QThread(this);
    pimpl->jsonSerializer = new JSONSerializer();
    pimpl->jsonSerializer->moveToThread(pimpl->thread);
    connect(pimpl->thread, SIGNAL(finished()), pimpl->jsonSerializer, SLOT(deleteLater()));
    connect(pimpl->jsonSerializer, SIGNAL(dataReady(QString)),
            this, SLOT(sendMessage(QString)), Qt::QueuedConnection);

    pimpl->jsonParser = new JSONParser();
    pimpl->jsonParser->moveToThread(pimpl->thread);
    connect(pimpl->thread, SIGNAL(finished()), pimpl->jsonParser, SLOT(deleteLater()));
    connect(pimpl->socket, SIGNAL(textMessageReceived(QString)),
            pimpl->jsonParser, SLOT(parse(QString)), Qt::QueuedConnection);
    connect(pimpl->jsonParser, SIGNAL(dataReady(QVariantMap)),
            this, SLOT(receiveMessage(QVariantMap)), Qt::QueuedConnection);

    pimpl->thread->start();

    /* check and send message every 200ms */
    pimpl->sendMessageTimerId = startTimer(SEND_DATA_INTERVAL);
}

BLMMessageHandler::~BLMMessageHandler()
{
    pimpl->thread->quit();
    pimpl->thread->wait();
    delete pimpl;
}

void BLMMessageHandler::timerEvent(QTimerEvent *ev)
{
    Q_UNUSED(ev)

    if (pimpl->sendMessageTimerId == ev->timerId())
    {
        if (!pimpl->connected)
        {
            /* if not connected, clear the pending queue */
            pimpl->pendingMsgQueue.clear();
            return;
        }

        if (!pimpl->pendingMsgQueue.isEmpty())
        {
            BLMMessageID msgId = pimpl->pendingMsgQueue.takeFirst();
            QVariantMap map;
            switch (msgId) {
            case BLM_MSG_DEV_CONF:
            {
                QVariantMap devMap = pimpl->getDeviceConfig();
                devMap["MsgHdr"] = pimpl->generateMsgHdr(msgId);
                map[blmMessageNames[msgId]] = devMap;
                pimpl->sendQVariantMap(map);
            }
                break;
            case BLM_MSG_WAVE_CONF:
            {
                pimpl->collectWaveConfig();
                QVariantMap waveConfigs = pimpl->getWaveConfigs();
                waveConfigs["MsgHdr"] = pimpl->generateMsgHdr(msgId);
                map[blmMessageNames[msgId]] = waveConfigs;
                pimpl->sendQVariantMap(map);
                break;
            }
            case BLM_MSG_TREND_CONF:
            {
                pimpl->collectTrendConfig();
                QVariantMap trendConfigs = pimpl->getTrendConfigs();
                trendConfigs["MsgHdr"] = pimpl->generateMsgHdr(msgId);
                map[blmMessageNames[msgId]] = trendConfigs;
                pimpl->sendQVariantMap(map);
                break;
            }
            case BLM_MSG_PATIENT_INFO:
            {
                QVariantMap infoMap = pimpl->getPatientInfo();
                infoMap["MsgHdr"] = pimpl->generateMsgHdr(msgId);
                map[blmMessageNames[msgId]] = infoMap;
                pimpl->sendQVariantMap(map);
                break;
            }
            default:
                break;
            }
        }
    }
    else if (pimpl->waveCollectTimer == ev->timerId())
    {
        QVariantMap contiWaves = pimpl->getContinuousWaves();
        if (!contiWaves.isEmpty())
        {
            /* try to send continuous wave data */
        }
        contiWaves["MsgHdr"] = pimpl->contiWaveMsgHdr;
        /* generate a new wave msg header */
        pimpl->contiWaveMsgHdr = pimpl->generateMsgHdr(BLM_MSG_CONTINUE_WAVE);
        QVariantMap map;
        map[blmMessageNames[BLM_MSG_CONTINUE_WAVE]] = contiWaves;
        pimpl->sendQVariantMap(map);
    }
    else if (pimpl->trendCollectTimer == ev->timerId())
    {
        QVariantMap trend = pimpl->getTrendData();
        trend["MsgHdr"] = pimpl->generateMsgHdr(BLM_MSG_TREND);
        QVariantMap map;
        map[blmMessageNames[BLM_MSG_TREND]] = trend;
        pimpl->sendQVariantMap(map);
    }
}

void BLMMessageHandler::onConnected()
{
    pimpl->connected = true;
    pimpl->connectedTime = QDateTime::currentDateTime().toTime_t();

    Config *sysConfig = ConfigManagerInterface::getConfigManager()->getSysConfig();
    sysConfig->setConfigChangedCallback(BLMMessageHandlerPrivate::sysConfigCallback);

    /* collect devcie config */
    pimpl->collectDevConfiguration();
    /* send device config later */
    pimpl->addPendingMessage(BLM_MSG_DEV_CONF);
    /* send wave config later */
    pimpl->addPendingMessage(BLM_MSG_WAVE_CONF);
    /* send trend config later */
    pimpl->addPendingMessage(BLM_MSG_TREND_CONF);
    /* start wave collection */
    pimpl->startWaveCollection();
    /* start trend collection */
    pimpl->startTrendCollection();
    /* collect patient info */
    pimpl->collectPatientInfo();
    /* send patient info later */
    pimpl->addPendingMessage(BLM_MSG_PATIENT_INFO);
}

void BLMMessageHandler::onDisconnected()
{
    pimpl->connected = false;
    pimpl->connectedTime = 0;
    Config *sysConfig = ConfigManagerInterface::getConfigManager()->getSysConfig();
    sysConfig->setConfigChangedCallback(NULL);
    /* stop wave collection */
    pimpl->stopWaveCollection();
    /* stop trend collection */
    pimpl->stopTrendCollection();
}

void BLMMessageHandler::sendMessage(const QString &message)
{
    if (pimpl->socket->isValid())
    {
        pimpl->socket->send(message);
    }
}

void BLMMessageHandler::receiveMessage(const QVariantMap &map)
{
    /* TODO */
}

BLMMessageHandlerPrivate::~BLMMessageHandlerPrivate()
{
    if (instance == this)
    {
        instance = NULL;
    }
}

BLMWaveformID BLMMessageHandlerPrivate::waveIdToBLMWaveIdHelper(WaveformID waveID)
{
    BLMWaveformID blmWaveID = BLM_WAVE_NONE;
    switch (waveID) {
    case WAVE_ECG_I:
        blmWaveID = BLM_WAVE_I;
        break;
    case WAVE_ECG_II:
        blmWaveID = BLM_WAVE_II;
        break;
    case WAVE_ECG_III:
        blmWaveID = BLM_WAVE_III;
        break;
    case WAVE_ECG_aVR:
        blmWaveID = BLM_WAVE_AVR;
        break;
    case WAVE_ECG_aVL:
        blmWaveID = BLM_WAVE_AVL;
        break;
    case WAVE_ECG_aVF:
        blmWaveID = BLM_WAVE_AVF;
        break;
    case WAVE_ECG_V1:
        blmWaveID = BLM_WAVE_V1;
        break;
    case WAVE_ECG_V2:
        blmWaveID = BLM_WAVE_V2;
        break;
    case WAVE_ECG_V3:
        blmWaveID = BLM_WAVE_V3;
        break;
    case WAVE_ECG_V4:
        blmWaveID = BLM_WAVE_V4;
        break;
    case WAVE_ECG_V5:
        blmWaveID = BLM_WAVE_V5;
        break;
    case WAVE_ECG_V6:
        blmWaveID = BLM_WAVE_V6;
        break;
    case WAVE_RESP:
        blmWaveID = BLM_WAVE_RESP;
        break;
    case WAVE_SPO2:
        blmWaveID = BLM_WAVE_SPO2;
        break;
    case WAVE_CO2:
        blmWaveID = BLM_WAVE_CO2;
        break;
    default:
        break;
    }

    return blmWaveID;
}

BLMUnit BLMMessageHandlerPrivate::unitToBLMUnitHelper(UnitType unit)
{
    BLMUnit blmUnit = BLM_UNIT_NONE;
    switch (unit) {
    case UNIT_TC:
        blmUnit = BLM_UNIT_C;
        break;
    case UNIT_TF:
        blmUnit = BLM_UNIT_F;
        break;
    case UNIT_MMHG:
        blmUnit = BLM_UNIT_MMHG;
        break;
    case UNIT_KPA:
        blmUnit = BLM_UNIT_KPA;
        break;
    case UNIT_PERCENT:
        blmUnit = BLM_UNIT_PERCENT;
        break;
    case UNIT_RPM:
        blmUnit = BLM_UNIT_RPM;
        break;
    case UNIT_BPM:
        blmUnit = BLM_UNIT_BPM;
        break;
    case UNIT_MICROVOLT:
        blmUnit = BLM_UNIT_UV;
        break;
    case UNIT_MV:
        blmUnit = BLM_UNIT_MV;
        break;
    case UNIT_KG:
        blmUnit = BLM_UNIT_KG;
        break;
    case UNIT_LB:
        blmUnit = BLM_UNIT_LB;
        break;
    case UNIT_CM:
        blmUnit = BLM_UNIT_CM;
        break;
    case UNIT_MM:
        blmUnit = BLM_UNIT_MM;
        break;
    case UNIT_INCH:
        blmUnit = BLM_UNIT_INCH;
        break;
    case UNIT_MS:
        blmUnit = BLM_UNIT_MS;
        break;
    default:
        break;
    }
    return blmUnit;
}

void BLMMessageHandlerPrivate::sendQVariantMap(const QVariantMap &map)
{
    QMetaObject::invokeMethod(jsonSerializer, "serialize", Qt::QueuedConnection, Q_ARG(QVariantMap, map));
}

QVariantMap BLMMessageHandlerPrivate::generateMsgHdr(const BLMMessageID &msgID)
{
    QVariantMap hdr;
    struct timespec tp;
    quint64 msec;
    if (clock_gettime(CLOCK_BOOTTIME, &tp))
    {
        qDebug() << Q_FUNC_INFO << "Fail to get boottime";
    }
    msec = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
    msec &= 0xFFFFFFFFFF;   // 40bits

    quint32 timestamp = QDateTime::currentDateTime().toTime_t();

    hdr["MSecTime"] = msec;
    hdr["ElapsedTime"] =  timestamp - connectedTime;
    hdr["MsgId"] = msgID;
    hdr["DevTime"] = timestamp;

    return hdr;
}

void BLMMessageHandlerPrivate::collectDevConfiguration()
{
    int val = 0;
    QString str;
    int errorCount = 0;
    Config *sysConfig = ConfigManagerInterface::getConfigManager()->getSysConfig();
    Config *machConfig = ConfigManagerInterface::getConfigManager()->getMachineConfig();
    devConfig.productType = BLM_PRODUCT_VITAVUE_10; /* TODO: find way to get current product type */
    errorCount += !sysConfig->getNumValue("DateTime|DateFormat", val);
    devConfig.dateFormat = static_cast<BLMDateFormat>(val);
    errorCount += !sysConfig->getNumValue("DateTime|TimeFormat", val);
    devConfig.timeFormat = static_cast<BLMTimeFormat>(val);
    errorCount += !machConfig->getStrValue("SerialNumber", str);
    devConfig.serialNumber = str;
    errorCount += !sysConfig->getNumAttr("General|Language", "CurrentOption", val);
    devConfig.lang =  static_cast<BLMLanguage>(val);
    errorCount += !sysConfig->getNumValue("Others|ECGStandard", val);
    devConfig.ecgConv = static_cast<BLMEcgConvention>(val);
    errorCount += !sysConfig->getStrValue("General|BedNumber", str);
    devConfig.bedID = str;

    if (errorCount)
    {
        qWarning() << Q_FUNC_INFO << "parse config file failed";
    }

    int moduleConfig = 0;

    if (machConfig->getNumValue("ECGEnable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_ECG;
    }
    if (machConfig->getNumValue("RESEEnable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_RESP;
    }
    if (machConfig->getNumValue("SPO2Enable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_SPO2;
    }
    if (machConfig->getNumValue("NIBPEnable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_NIBP;
    }
    if (machConfig->getNumValue("CO2Enable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_CO2;
    }
    if (machConfig->getNumValue("ECG12LeadEnable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_ECG12;
    }
    if (machConfig->getNumValue("IBPEnable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_IBP;
    }
    if (machConfig->getNumValue("COEnable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_CO;
    }
    if (machConfig->getNumValue("PrinterEnable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_RECORDER;
    }
    if (machConfig->getNumValue("HDMIEnable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_HDMI;
    }
    if (machConfig->getNumValue("NurseCallEnable", val) && val)
    {
        moduleConfig |= BLM_MOD_CONF_NURSE_CALL;
    }

    devConfig.moduleConfig = moduleConfig;

    Provider *provider = NULL;
    BLMSoftwareVersionInfo swInfo;
    provider = paramManager.getProvider("BLM_E5");
    if (provider)
    {
        swInfo.e5SwVer = provider->getVersionString();
    }
    provider = paramManager.getProvider("BLM_S5");
    if (provider)
    {
        swInfo.s5SwVer = provider->getVersionString();
    }
    provider = paramManager.getProvider("BLM_N5");
    if (provider)
    {
        swInfo.n5SwVer = provider->getVersionString();
    }
    provider = paramManager.getProvider("BLM_T5");
    if (provider)
    {
        swInfo.t5SwVer = provider->getVersionString();
    }
    provider = paramManager.getProvider("SystemBoard");
    if (provider)
    {
        swInfo.sysSwVer = provider->getVersionString();
    }
    swInfo.hostSwVer = GIT_VERSION;

    devConfig.swVers = swInfo;
}

QVariantMap BLMMessageHandlerPrivate::getDeviceConfig()
{
    QVariantMap map;
    map["MsgVer"] = devConfig.msgVer;
    map["SerialNumber"] = devConfig.serialNumber;
    map["ProductType"] = devConfig.productType;
    map["BedID"] = devConfig.bedID;
    map["Language"] = devConfig.lang;
    map["DateFormat"] = devConfig.dateFormat;
    map["TimeFormat"] = devConfig.timeFormat;
    map["EcgLeadLabels"] = devConfig.ecgConv;
    map["ModuleConfig"] = devConfig.moduleConfig;
    QVariantMap swInfo;
    swInfo["HostSwVer"] = devConfig.swVers.hostSwVer;
    swInfo["E5SwVer"] = devConfig.swVers.e5SwVer;
    swInfo["S5SwVer"] = devConfig.swVers.s5SwVer;
    swInfo["N5SwVer"] = devConfig.swVers.n5SwVer;
    swInfo["T5SwVer"] = devConfig.swVers.t5SwVer;
    swInfo["SysSwVer"] = devConfig.swVers.sysSwVer;
    map["SoftwareVersions"] = swInfo;

    return map;
}

void BLMMessageHandlerPrivate::collectPatientInfo()
{
    patientInfo.id = patientManager.getPatID();
    patientInfo.name = patientManager.getName();
    patientInfo.bedId = patientManager.getBedNum().toInt();
    patientInfo.type = patientManager.getType();
    patientInfo.gender = patientManager.getSex();
    patientInfo.blood = static_cast<PatientBloodType>(patientManager.getBlood());
    patientInfo.weight = patientManager.getWeight();
    patientInfo.height = patientManager.getHeight();
    int curYear = QDate::currentDate().year();
    int bornYear = patientManager.getBornDate().year();
    patientInfo.age = curYear - bornYear;
}

QVariantMap BLMMessageHandlerPrivate::getPatientInfo()
{
    QVariantMap map;
    map["MsgVer"] = 1;
    QVariantMap info;
    info["ID"] = patientInfo.id;
    info["Name"] = patientInfo.name;
    info["BedID"] = patientInfo.bedId;
    info["Type"] = patientInfo.type;
    info["Gender"] = patientInfo.gender;
    info["Weight"] = patientInfo.weight;
    info["Height"] = patientInfo.height;
    info["Age"] = patientInfo.age;

    map["PatientData"] = info;
    return map;
}

void BLMMessageHandlerPrivate::collectWaveConfig()
{
    Param *param = NULL;
    Provider *provider = NULL;
    BLMWaveConfig waveConf;
    /* ECG */
    param = paramManager.getParam(PARAM_ECG);
    if (param)
    {
        provider = paramManager.getParamProvider(param->getName());
        ECGParamInterface *ecgParam = dynamic_cast<ECGParamInterface *>(param);
        ECGProviderIFace *ecgProvider = dynamic_cast<ECGProviderIFace *>(provider);
        Q_ASSERT(ecgParam);
        Q_ASSERT(ecgProvider);
        ECGLeadMode leadMode = ecgParam->getLeadMode();
        int nr = 5;
        if (leadMode == ECG_LEAD_MODE_12)
        {
            nr = 12;
        }
        else if (leadMode == ECG_LEAD_MODE_3)
        {
            nr = 3;
        }

        for (int i = ECG_LEAD_I; i < nr; i++)
        {
            waveConf.waveId = waveIdToBLMWaveIdHelper(static_cast<WaveformID>(i));
            waveConf.maxVal = 8388607;  /* 2^23 - 1 */
            waveConf.minVal = -8388608; /* -2^23 */
            waveConf.sampleRate = ecgProvider->getWaveformSample();
            waveConf.baseLine = 0;
            waveConf.unit  = BLM_UNIT_UV;
            waveConf.scale = 500.0f / ecgProvider->getP05mV();
            this->waveConfigs.insert(waveConf.waveId, waveConf);
        }
    }

    /* resp */
    param = paramManager.getParam(PARAM_RESP);
    if (param)
    {
        provider = paramManager.getParamProvider(param->getName());
        RESPParamInterface *respParam = dynamic_cast<RESPParamInterface *>(param);
        RESPProviderIFace *respProvider = dynamic_cast<RESPProviderIFace *>(provider);
        Q_ASSERT(respParam);
        Q_ASSERT(respProvider);

        waveConf.waveId = waveIdToBLMWaveIdHelper(WAVE_RESP);
        waveConf.sampleRate = respProvider->getRESPWaveformSample();
        waveConf.maxVal = respProvider->maxRESPWaveValue();
        waveConf.minVal = respProvider->minRESPWaveValue();
        waveConf.baseLine = respProvider->getRESPBaseLine();
        waveConf.unit = BLM_UNIT_NONE;
        waveConf.scale = 1;
        this->waveConfigs.insert(waveConf.waveId, waveConf);
    }

    /* SPO2 */
    param = paramManager.getParam(PARAM_SPO2);
    if (param)
    {
        provider = paramManager.getParamProvider(param->getName());
        SPO2ParamInterface *spo2Param = dynamic_cast<SPO2ParamInterface *>(param);
        SPO2ProviderIFace *spo2Provider = dynamic_cast<SPO2ProviderIFace *>(provider);
        Q_ASSERT(spo2Param);
        Q_ASSERT(spo2Provider);

        waveConf.waveId = waveIdToBLMWaveIdHelper(WAVE_SPO2);
        waveConf.sampleRate = spo2Provider->getSPO2WaveformSample();
        waveConf.maxVal = spo2Provider->getSPO2MaxValue();
        waveConf.minVal = 0;
        waveConf.baseLine = spo2Provider->getSPO2BaseLine();
        waveConf.unit = BLM_UNIT_NONE;
        waveConf.scale = 1;
        this->waveConfigs.insert(waveConf.waveId, waveConf);
    }

    /* CO2 */
    param = paramManager.getParam(PARAM_CO2);
    if (param)
    {
        provider = paramManager.getParamProvider(param->getName());
        Co2ParamInterface *co2Param = dynamic_cast<Co2ParamInterface *>(param);
        CO2ProviderIFace *co2Provider = dynamic_cast<CO2ProviderIFace *>(provider);
        Q_ASSERT(co2Param);
        Q_ASSERT(co2Provider);

        waveConf.waveId = waveIdToBLMWaveIdHelper(WAVE_CO2);
        waveConf.sampleRate = co2Provider->getCO2WaveformSample();
        waveConf.maxVal = co2Provider->getCO2MaxWaveform();
        waveConf.minVal = 0;
        waveConf.baseLine = co2Provider->getCO2BaseLine();
        waveConf.unit = BLM_UNIT_NONE;
        waveConf.scale = 1;
        this->waveConfigs.insert(waveConf.waveId, waveConf);
    }
}

QVariantMap BLMMessageHandlerPrivate::getWaveConfigs()
{
   QVariantMap map;
   map["MsgVer"] = 1;
   QVariantList waveConfig;
   QMap<BLMWaveformID, BLMWaveConfig>::ConstIterator iter = this->waveConfigs.constBegin();
   for (; iter != this->waveConfigs.constEnd(); ++iter)
   {
       QVariantMap wavConf;
       BLMWaveConfig c = iter.value();
       wavConf["WaveId"] = c.waveId;
       wavConf["SampleRate"] = c.sampleRate;
       wavConf["BaseLine"] = c.baseLine;
       wavConf["MaxVal"] = c.maxVal;
       wavConf["MinVal"] = c.minVal;
       wavConf["Unit"] = c.unit;
       wavConf["Scale"] = c.scale;
       waveConfig.append(wavConf);
   }
   map["WaveConfig"] = waveConfig;

   return map;
}

void BLMMessageHandlerPrivate::collectTrendConfig()
{
    AlarmLimitIFace *alarmLimit = NULL;
    BLMTrendConfig conf;
    /* ECG */
    conf.paramID = BLM_PARAM_ECG;
    conf.trendID = BLM_TREND_PARAM_HR_PR;
    alarmLimit = alarmSourceManager.getLimitAlarmSource(LIMIT_ALARMSOURCE_ECGDUP);
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(PARAM_ECG);
        conf.highLimit = alarmLimit->getUpper(PARAM_ECG);
    }
    conf.scale = 1;
    conf.unit = BLM_UNIT_BPM;
    trendConfigs.insert(BLM_TREND_PARAM_HR_PR, conf);

    /* RESP */
    conf.paramID = BLM_PARAM_RESP;
    conf.trendID = BLM_TREND_PARAM_RR_BR;
    alarmLimit = alarmSourceManager.getLimitAlarmSource(LIMIT_ALARMSOURCE_RESPDUP);
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(PARAM_RESP);
        conf.highLimit = alarmLimit->getUpper(PARAM_RESP);
    }
    conf.scale = 1;
    conf.unit = BLM_UNIT_RPM;
    trendConfigs.insert(BLM_TREND_PARAM_RR_BR, conf);

    /* SPO2 */
    conf.paramID = BLM_PARAM_SPO2;
    conf.trendID = BLM_TREND_PARAM_SPO2;
    alarmLimit = alarmSourceManager.getLimitAlarmSource(LIMIT_ALARMSOURCE_SPO2);
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(PARAM_SPO2);
        conf.highLimit = alarmLimit->getUpper(PARAM_SPO2);
    }
    conf.scale = 1;
    conf.unit = BLM_UNIT_PERCENT;
    trendConfigs.insert(BLM_TREND_PARAM_SPO2, conf);

    /* NIBP */
    conf.paramID = BLM_PARAM_NIBP;
    conf.trendID = BLM_TREND_PARAM_NIBP_SYS;
    alarmLimit = alarmSourceManager.getLimitAlarmSource(LIMIT_ALARMSOURCE_NIBP);
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(NIBP_LIMIT_ALARM_SYS_LOW);
        conf.highLimit = alarmLimit->getUpper(NIBP_LIMIT_ALARM_SYS_HIGH);
    }
    conf.scale = 1;
    conf.unit = BLM_UNIT_MMHG;
    trendConfigs.insert(BLM_TREND_PARAM_NIBP_SYS, conf);

    conf.trendID = BLM_TREND_PARAM_NIBP_DIA;
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(NIBP_LIMIT_ALARM_DIA_LOW);
        conf.highLimit = alarmLimit->getUpper(NIBP_LIMIT_ALARM_DIA_HIGH);
    }
    conf.scale = 1;
    conf.unit = BLM_UNIT_MMHG;
    trendConfigs.insert(BLM_TREND_PARAM_NIBP_DIA, conf);

    conf.trendID = BLM_TREND_PARAM_NIBP_MAP;
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(NIBP_LIMIT_ALARM_MAP_LOW);
        conf.highLimit = alarmLimit->getUpper(NIBP_LIMIT_ALARM_MAP_HIGH);
    }
    conf.scale = 1;
    conf.unit = BLM_UNIT_MMHG;
    trendConfigs.insert(BLM_TREND_PARAM_NIBP_MAP, conf);

    /* Temp */
    conf.paramID = BLM_PARAM_TEMP;
    conf.trendID = BLM_TREND_PARAM_TEMP_T1;
    alarmLimit = alarmSourceManager.getLimitAlarmSource(LIMIT_ALARMSOURCE_TEMP);
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(TEMP_LIMIT_ALARM_T1_LOW);
        conf.highLimit = alarmLimit->getUpper(TEMP_LIMIT_ALARM_T1_HIGH);
    }
    conf.scale = 10;
    conf.unit = BLM_UNIT_C;
    trendConfigs.insert(BLM_TREND_PARAM_TEMP_T1, conf);

    conf.trendID = BLM_TREND_PARAM_TEMP_T2;
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(TEMP_LIMIT_ALARM_T2_LOW);
        conf.highLimit = alarmLimit->getUpper(TEMP_LIMIT_ALARM_T2_HIGH);
    }
    conf.scale = 10;
    conf.unit = BLM_UNIT_C;
    trendConfigs.insert(BLM_TREND_PARAM_TEMP_T2, conf);

    conf.trendID = BLM_TREND_PARAM_TEMP_TD;
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(TEMP_LIMIT_ALARM_TD_LOW);
        conf.highLimit = alarmLimit->getUpper(TEMP_LIMIT_ALARM_TD_HIGH);
    }
    conf.scale = 10;
    conf.unit = BLM_UNIT_C;
    trendConfigs.insert(BLM_TREND_PARAM_TEMP_TD, conf);

    /* CO2 */
    conf.paramID = BLM_PARAM_CO2;
    conf.trendID = BLM_TREND_PARAM_ETCO2;
    alarmLimit = alarmSourceManager.getLimitAlarmSource(LIMIT_ALARMSOURCE_CO2);
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(CO2_LIMIT_ALARM_ETCO2_LOW);
        conf.highLimit = alarmLimit->getUpper(CO2_LIMIT_ALARM_ETCO2_HIGH);
    }
    conf.scale = 1;
    conf.unit = BLM_UNIT_MMHG;
    trendConfigs.insert(BLM_TREND_PARAM_ETCO2, conf);

    conf.trendID = BLM_TREND_PARAM_FICO2;
    if (alarmLimit)
    {
        conf.lowLimit = alarmLimit->getLower(CO2_LIMIT_ALARM_FICO2_LOW);
        conf.highLimit = alarmLimit->getUpper(CO2_LIMIT_ALARM_FICO2_HIGH);
    }
    conf.scale = 1;
    conf.unit = BLM_UNIT_MMHG;
    trendConfigs.insert(BLM_TREND_PARAM_FICO2, conf);
}

QVariantMap BLMMessageHandlerPrivate::getTrendConfigs()
{
    QVariantMap map;
    map["MsgVer"] = 1;
    QVariantList trendConfig;
    QMap<BLMTrendParamID, BLMTrendConfig>::ConstIterator iter = this->trendConfigs.constBegin();
    for (; iter != this->trendConfigs.constEnd(); ++iter)
    {
        QVariantMap conf;
        BLMTrendConfig c = iter.value();
        conf["ParamID"] = c.paramID;
        conf["TrendID"] = c.trendID;
        conf["LowLimit"] = c.lowLimit;
        conf["HighLimit"] = c.highLimit;
        conf["Scale"] = c.scale;
        conf["Unit"] = c.unit;
        trendConfig.append(conf);
    }
    map["TrendConfig"] = trendConfig;
    return map;
}

void BLMMessageHandlerPrivate::startWaveCollection()
{
    foreach(ParamID paramID, paramsToCollectData) {
        Param *param = paramManager.getParam(paramID);
        if (param)
        {
            param->setPreProcessor(paramCollector);
        }
    }
    contiWaveMsgHdr = generateMsgHdr(BLM_MSG_CONTINUE_WAVE);
    waveCollectTimer = q_ptr->startTimer(WAVE_COLLECT_DURATION);
}

void BLMMessageHandlerPrivate::stopWaveCollection()
{
    foreach(ParamID paramID, paramsToCollectData) {
        Param *param = paramManager.getParam(paramID);
        if (param)
        {
            param->setPreProcessor(NULL);
        }
    }
    clearCollectedWaveData();
    q_ptr->killTimer(waveCollectTimer);
}

void BLMMessageHandlerPrivate::startTrendCollection()
{
    trendCollectTimer = q_ptr->startTimer(TREND_COLLECT_DURATION);
}

void BLMMessageHandlerPrivate::stopTrendCollection()
{
    q_ptr->killTimer(trendCollectTimer);
}

void BLMMessageHandlerPrivate::clearCollectedWaveData()
{
   foreach(BLMWaveformID wavID, waveSampleList.keys()) {
       waveSampleList[wavID].clear();
       waveStatusList[wavID].clear();
   }
}

QVariantMap BLMMessageHandlerPrivate::getContinuousWaves()
{
   QVariantMap map;
   QVariantList waveDatas;
   foreach(BLMWaveformID wavID, waveSampleList.keys()) {
       int length = waveSampleList[wavID].length();
       if (length == 0)
           continue;
       QVariantMap waveData;
       waveData["WaveId"] = wavID;
       waveData["FrameLen"] = length;
       waveData["Samples"] = waveSampleList[wavID];
       waveData["Status"] = waveStatusList[wavID];
       waveDatas.append(waveData);
   }
   if (waveDatas.length() == 0)
   {
       return map;
   }
   map["MsgVer"] = 1;
   map["WaveData"] = waveDatas;
   /* clear the wave */
   clearCollectedWaveData();
   return map;
}

QVariantMap BLMMessageHandlerPrivate::getTrendData()
{
    QVariantMap map;
    QVariantList trends;
    for (int i = 1; i < BLM_TREND_PARAM_NR; i++)
    {
        Param *param = paramManager.getParam(paramIdConvertParam(static_cast<BLMTrendParamID>(i)));
        if (param)
        {
            trends.append(param->getTrendVariant(i));
        }
    }
    map["TrendData"] = trends;
    map["MsgVer"] = 1;
    return map;
}

ParamID BLMMessageHandlerPrivate::paramIdConvertParam(BLMTrendParamID id)
{
    switch (id)
    {
    case BLM_TREND_PARAM_HR_PR:
        return PARAM_DUP_ECG;
    case BLM_TREND_PARAM_RR_BR:
        return PARAM_DUP_RESP;
    case BLM_TREND_PARAM_SPO2:
    case BLM_TREND_PARAM_PI:
        return PARAM_SPO2;
    case BLM_TREND_PARAM_NIBP_SYS:
    case BLM_TREND_PARAM_NIBP_DIA:
    case BLM_TREND_PARAM_NIBP_MAP:
        return PARAM_NIBP;
    case BLM_TREND_PARAM_TEMP_T1:
    case BLM_TREND_PARAM_TEMP_T2:
    case BLM_TREND_PARAM_TEMP_TD:
        return PARAM_TEMP;
    case BLM_TREND_PARAM_ETCO2:
    case BLM_TREND_PARAM_FICO2:
        return PARAM_CO2;
    default:
        return PARAM_NONE;
    }
}

void BLMMessageHandlerPrivate::sysConfigCallback(const QString &index, const QString &val)
{
    if (instance->configHandlers.contains(index))
    {
        const ConfigHandler &handler = instance->configHandlers[index];
        handler.handle(val);
        instance->addPendingMessage(handler.msgId);
    }
}

int ParamDataCollector::preProcessData(ParamID paramID, SubParamID subParamID, int value)
{
    return value;
}

int ParamDataCollector::preProcessData(ParamID paramID, const QString &name, int value)
{
    Q_UNUSED(paramID)
    Q_UNUSED(name)

    return value;
}

void ParamDataCollector::preProcessWave(WaveformID waveID, int *wave, int *flag)
{
    BLMWaveformID id = m_handler->waveIdToBLMWaveIdHelper(waveID);
    if (!m_handler->waveConfigs.contains(id))
    {
        return;
    }

    QMap<BLMWaveformID, QVariantList>::Iterator iter  = m_handler->waveSampleList.find(id);
    if (iter == m_handler->waveSampleList.end())
    {
        iter = m_handler->waveSampleList.insert(id, QVariantList());
    }

    iter.value().append(qVariantFromValue(*wave));

    QMap<BLMWaveformID, QVariantList>::Iterator stIter  = m_handler->waveStatusList.find(id);
    if (stIter == m_handler->waveStatusList.end())
    {
        stIter = m_handler->waveStatusList.insert(id, QVariantList());
    }

    int st = 0;
    if ((*flag) & INVALID_WAVE_FALG_BIT)
    {
        st |= BLM_WAVE_ST_INVALID;
    }
    if ((*flag) & WAVE_QRS_FLAG)
    {
        st |= BLM_WAVE_ST_QRS;
    }
    if ((*flag) & ECG_INTERNAL_FLAG_BIT)
    {
        st |= BLM_WAVE_ST_PACE;
    }
    stIter.value().append(qVariantFromValue(st));
}
