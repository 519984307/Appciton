#include "../Utility/DataStorage/StorageManager_p.h"
#include "SummaryStorageManager.h"
#include "AlarmSummary.h"
#include "NIBPSummary.h"
#include "CodeMarkerSummary.h"
#include "PrinterActionSummary.h"
#include "StorageFile.h"
#include "SupervisorConfigManager.h"
#include "TimeManager.h"
#include "DataStorageDirManager.h"
#include "ParamInfo.h"
#include "ParamManager.h"
#include "ECGParam.h"
#include "TrendCache.h"
#include "RESPParam.h"
#include "CO2Param.h"
#include "WaveformCache.h"
#include "SystemManager.h"
#include "PrintTriggerSummaryLayout.h"
#include "TimeDate.h"
#include "PatientManager.h"
#include <QDateTime>
#include "Utility.h"
#include "ECGSummary.h"
#include "SPO2Param.h"
#include "TrendCache.h"
#include "NIBPSymbol.h"
#include "SPO2Symbol.h"
#include "CO2Symbol.h"
#include "ECGSymbol.h"
#include "TEMPSymbol.h"
#include "RESPSymbol.h"
#include "WindowManager.h"

class SummaryStorageManagerPrivate : public StorageManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(SummaryStorageManager)

    SummaryStorageManagerPrivate(SummaryStorageManager * q_ptr)
        :StorageManagerPrivate(q_ptr), currentPacerModeDuration(0), lastPacerModeDuration(0){}

    void updateSummaryHead();

    // summary是否自动打印
    bool isAutoPrint(SummaryType type);
    PrintType triggerPrintType(SummaryType type);


    SummaryHead summaryHead;

    QList<SummaryItem*> summaryItemList;
    QList<SummaryItem*> triggerPrintItemList;

    unsigned currentPacerModeDuration;
    unsigned lastPacerModeDuration;
    QDateTime pacerStartTime;
    QMutex  pendingMutex;
};

/**************************************************************************************************
 * 更新summary头部
 *************************************************************************************************/
void SummaryStorageManagerPrivate::updateSummaryHead()
{
    summaryHead.totalItems = backend->getBlockNR();

    const char * patientName = patientManager.getName();
    if(strncmp(summaryHead.patientName, patientName, sizeof(summaryHead.patientName)))
    {
       Util::strlcpy(summaryHead.patientName, patientName, sizeof(summaryHead.patientName));
    }

    summaryHead.sum();

    backend->writeAdditionalData((char *)&summaryHead, sizeof(summaryHead));
}

/**************************************************************************************************
 * summary类型是否自动打印
 *************************************************************************************************/
bool SummaryStorageManagerPrivate::isAutoPrint(SummaryType type)
{
    int onoff = 0;
    switch (type)
    {
        case SUMMARY_ECG_RHYTHM:
            superConfig.getNumValue("Print|PresentingECG", onoff);
            break;
        case SUMMARY_SHOCK_DELIVERY:
            superConfig.getNumValue("Print|ShockDelivery", onoff);
            break;
        case SUMMARY_CHECK_PATIENT_ALARM:
            superConfig.getNumValue("Print|CheckPatient", onoff);
            break;
        case SUMMARY_ECG_FOREGROUND_ANALYSIS:
            superConfig.getNumValue("Print|ECGAnalysis", onoff);
            break;
        case SUMMARY_PHY_ALARM:
            superConfig.getNumValue("Print|PhysiologicalAlarm", onoff);
            break;
        case SUMMARY_PACER_STARTUP:
            superConfig.getNumValue("Print|PacerStartUp", onoff);
            break;
        case SUMMARY_PRINTER_ACTION:
        case SUMMARY_PRINTER_ACTION_2_TRACE:
        case SUMMARY_PRINTER_ACTION_3_TRACE:
        case SUMMARY_PRINTER_ACTION_4_TRACE:
            return false;
        case SUMMARY_CODE_MAKER:
            superConfig.getNumValue("Print|CoderMarker", onoff);
            break;
        case SUMMARY_NIBP:
            superConfig.getNumValue("Print|NIBPReading", onoff);
            break;
        case SUMMARY_DIAG_ECG:
            superConfig.getNumValue("Print|DiagnosticECG", onoff);
            break;
        default:
            return false;
    }

    return (onoff ? true : false);
}

/***************************************************************************************************
 * triggerPrintType : summary type to trigger print type
 **************************************************************************************************/
PrintType SummaryStorageManagerPrivate::triggerPrintType(SummaryType type)
{
    switch(type)
    {
    case SUMMARY_ECG_RHYTHM:
        return PRINT_TYPE_TRIGGER_ECG_RHYTHM;
    case SUMMARY_DIAG_ECG:
        return PRINT_TYPE_TRIGGER_DIAG_ECG;
    case SUMMARY_SHOCK_DELIVERY:
        return PRINT_TYPE_TRIGGER_SHOCK_DELIVERY;
    case SUMMARY_CHECK_PATIENT_ALARM:
        return PRINT_TYPE_TRIGGER_CHECK_PATIENT;
    case SUMMARY_ECG_FOREGROUND_ANALYSIS:
        return PRINT_TYPE_TRIGGER_FOREGROUND_ECG_ANALYSIS;
    case SUMMARY_PACER_STARTUP:
        return PRINT_TYPE_TRIGGER_PACER_STARTUP;
    case SUMMARY_PHY_ALARM:
        return PRINT_TYPE_TRIGGER_PHY_ALARM;
    case SUMMARY_CODE_MAKER:
        return PRINT_TYPE_TRIGGER_CODE_MARKER;
    case SUMMARY_NIBP:
        return PRINT_TYPE_TRIGGER_NIBP;
    default:
        return PRINT_TYPE_NONE;
    }
}

/**************************************************************************************************
 * 析构
 *************************************************************************************************/
SummaryStorageManager::SummaryStorageManager()
    :StorageManager(new SummaryStorageManagerPrivate(this), new StorageFile())
{
    createDir();
}

void SummaryStorageManager::createDir()
{
    Q_D(SummaryStorageManager);
    d->backend->reload(dataStorageDirManager.getCurFolder() + SUMMARY_DATA_FILE_NAME, QIODevice::ReadWrite);
    d->backend->setReservedSize(sizeof(SummaryHead));
    memset(&d->summaryHead, 0, sizeof(d->summaryHead));
    d->backend->readAdditionalData((char *)&d->summaryHead, sizeof(d->summaryHead));
//    if (timeManager.getPowerOnSession() == POWER_ON_SESSION_CONTINUE)
//    {
//        d->backend->readAdditionalData((char *)&d->summaryHead, sizeof(d->summaryHead));
//    }
//    else
//    {
//        dataStorageDirManager.addDataSize(sizeof(d->summaryHead));
//    }

    d->summaryHead.moduleConfig = systemManager.getModuleConfig();

    QString tmpStr;
    superConfig.getStrValue("General|DeviceID", tmpStr);
    if(tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy(d->summaryHead.deviceID, tmpStr.toLocal8Bit().constData(), sizeof(d->summaryHead.deviceID));

    //patient name
    tmpStr = patientManager.getName();
    if(tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy(d->summaryHead.patientName, tmpStr.toLocal8Bit().constData(), sizeof(d->summaryHead.patientName));

    //serial num
    machineConfig.getStrValue("SerialNumber", tmpStr);
    if(tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy(d->summaryHead.serialNum, tmpStr.toLocal8Bit().constData(), sizeof(d->summaryHead.serialNum));
}

/***************************************************************************************************
 * construction : singleton
 **************************************************************************************************/
SummaryStorageManager &SummaryStorageManager::construction()
{
    static SummaryStorageManager *_instance = NULL;
    if(_instance == NULL)
    {
        _instance = new SummaryStorageManager();
    }
    return *_instance;
}

/**************************************************************************************************
 * 析构
 *************************************************************************************************/
SummaryStorageManager::~SummaryStorageManager()
{

}

/***************************************************************************************************
 * addSummaryItem : add a summary Item to the storage queue
 **************************************************************************************************/
bool SummaryStorageManager::addSummaryItem(SummaryItem *item)
{
    if(item == NULL)
    {
        return false;
    }

    addData(item->type(), item->getSummaryData(), item->summaryDataLength());
    return true;
}

/**************************************************************************************************
 * 获取summary公共信息
 *************************************************************************************************/
void SummaryStorageManager::getSummaryCommonInfo(SummaryCommonInfo &commonInfo)
{
    Q_D(SummaryStorageManager);
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        ParamID id = paramInfo.getParamID((SubParamID)i);
        commonInfo.paramValue[i].value = paramManager.getSubParamValue(id, (SubParamID)i);
        commonInfo.paramValue[i].type = paramManager.getSubParamUnit(id, (SubParamID)i);
        commonInfo.paramValue[i].mul = paramInfo.getMultiOfSubParam((SubParamID)i);
    }

    TrendCacheData data;
    trendCache.getTendData(timeManager.getCurTime(), data);
    commonInfo.lastMeasureTime = data.lastNibpMeasureTime;
    commonInfo.co2bro = data.co2baro;
    commonInfo.co2Zoom = co2Param.getDisplayZoom();

    commonInfo.ECGLeadName = ecgParam.getCalcLead();
    commonInfo.ECGGain = ecgParam.getGain(ecgParam.getCalcLead());
    unsigned char gain;
    if(windowManager.getUFaceType() == UFACE_MONITOR_12LEAD)
    {
        gain = ecgParam.get12LGain(ECG_LEAD_I);
        commonInfo.ECGGain = gain;
        commonInfo.normalEcgGain = (gain << 16) | (gain << 8) | gain;
    }
    else
    {
        QList<int> waveID;
        windowManager.getDisplayedWaveform(waveID);
        //skip ignore the calcurate lead;
        waveID.removeFirst();
        commonInfo.normalEcgGain = 0;
        int offset = 0;
        while(waveID.size() && offset <= 24)
        {
            int id = waveID.takeFirst();
            if(id >= WAVE_ECG_I && id < WAVE_ECG_V6)
            {
                gain = ecgParam.getGain((ECGLead)id);
                commonInfo.normalEcgGain |= (gain<<offset);
            }
            offset += 8;
        }

        commonInfo.normalEcgGain |= (1<<24);
    }
    commonInfo.spo2Gain = spo2Param.getGain();
    commonInfo.respGain = respParam.getZoom();
    commonInfo.ECGBandwidth = ecgParam.getCalcLeadBandWidth();
    commonInfo.normalEcgBandWidth = commonInfo.ECGBandwidth;
    commonInfo.ECGNotchFilter = ecgParam.getCalcLeadNotchFilter();
    commonInfo.patientType = patientManager.getType();

    commonInfo.moduleConfig = systemManager.getModuleConfig();

    Util::strlcpy(commonInfo.patientName, d->summaryHead.patientName, sizeof(commonInfo.patientName));

    Util::strlcpy(commonInfo.deviceID, d->summaryHead.deviceID, sizeof(commonInfo.deviceID));
    Util::strlcpy(commonInfo.serialNum, d->summaryHead.serialNum, sizeof(commonInfo.serialNum));

}

/**************************************************************************************************
 * 读取波形缓存数据。
 *************************************************************************************************/
char SummaryStorageManager::getWaveGain(WaveformID id)
{
    switch (id)
    {
        case WAVE_ECG_I:
        case WAVE_ECG_II:
        case WAVE_ECG_III:
        case WAVE_ECG_aVR:
        case WAVE_ECG_aVL:
        case WAVE_ECG_aVF:
        case WAVE_ECG_V1:
        case WAVE_ECG_V2:
        case WAVE_ECG_V3:
        case WAVE_ECG_V4:
        case WAVE_ECG_V5:
        case WAVE_ECG_V6:
            return (char)ecgParam.getGain((ECGLead)id);
        case WAVE_RESP:
            return (char)respParam.getZoom();
        case WAVE_SPO2:
            return 0;
        case WAVE_CO2:
            return (char)co2Param.getDisplayZoom();
        default:
            return 0;
    }
}

/**************************************************************************************************
 * 循环运行
 *************************************************************************************************/
void SummaryStorageManager::run()
{
    Q_D(SummaryStorageManager);
    if(dataStorageDirManager.isCurRescueFolderFull())
    {
        discardData();
        d->updateSummaryHead();
        return;
    }


    dataStorageDirManager.addDataSize(saveData());
    d->updateSummaryHead();
}

/***************************************************************************************************
 * triggerPrintCallback : callback to pop the print item from the list
 **************************************************************************************************/
static void triggerPrintCallback(PrintResult result, unsigned char *data, int len)
{
    Q_UNUSED(result)
    if(len != sizeof(long))
    {
        qdebug("Callabck data length isn't match.\n");
        return;
    }

    //remove the first trigger print item
    //convert the data back to SummaryItem pointer
    SummaryItem *item = (SummaryItem *)(*((long *) data));
    summaryStorageManager.triggerPrintComplete(item);
}

/***************************************************************************************************
 * requestTriggerPrint : item request to perform trigger print
 **************************************************************************************************/
void SummaryStorageManager::requestTriggerPrint(SummaryItem *item)
{
    Q_D(SummaryStorageManager);
    if(NULL == item)
    {
        return;
    }

    if(!d->isAutoPrint(item->type()))
    {
        return;
    }

    d->pendingMutex.lock();
    d->triggerPrintItemList.append(item);
    d->pendingMutex.unlock();
    item->increaseRef();

    long data = (long)item;
    PrintType printType = d->triggerPrintType(item->type());
    bool requestSuccess = false;
    if(printType != PRINT_TYPE_NONE)
    {
        requestSuccess = printManager.requestPrint(printType,
                                                    PRINT_LAYOUT_ID_SUMMARY_TRIGGER_REPORT, &data, sizeof(long), triggerPrintCallback);
    }

    if(!requestSuccess)
    {
        //request print failed, item won't be printed, delete it
        d->pendingMutex.lock();
        item = d->triggerPrintItemList.takeLast();
        d->pendingMutex.unlock();
        item->decreaseRef();
    }
}

/***************************************************************************************************
 * triggerPrintComplete : the item has beed print
 **************************************************************************************************/
void SummaryStorageManager::triggerPrintComplete(SummaryItem *item)
{
    Q_D(SummaryStorageManager);
    if(item == NULL)
    {
        return;
    }

    QMutexLocker locker(&d->pendingMutex);
    int index = d->triggerPrintItemList.indexOf(item);
    if (index >= 0)
    {
        d->triggerPrintItemList.removeAt(index);
    }
    item->decreaseRef();
}

/***************************************************************************************************
 * checkCompletedItem : handle complete items
 **************************************************************************************************/
void SummaryStorageManager::checkCompletedItem()
{
    Q_D(SummaryStorageManager);
    QMutexLocker locker(&d->pendingMutex);
    while(!d->summaryItemList.isEmpty())
    {
        SummaryItem *item = d->summaryItemList.first();
        if(item == NULL)
        {
            d->summaryItemList.takeFirst();
            continue;
        }

        if(item->isCompleted())
        {
            d->summaryItemList.takeFirst();
            item->stopWaveCache();
            item->updateChecksum();
            if(item->type() != SUMMARY_MONITOR)
            {
                addSummaryItem(item);
            }

            item->decreaseRef();

        }
        else
        {
            break;
        }
    }
}

/**************************************************************************************************
 * 添加一个生理报警项目
 *************************************************************************************************/
void SummaryStorageManager::addPhyAlarm(unsigned alarmTime, ParamID paramID, int alarmType, bool oneshot, WaveformID id)
{
    Q_D(SummaryStorageManager);
    SummaryItem *item = PhyAlarmSummaryItem::create(alarmTime, paramID, alarmType, oneshot, id);
    d->pendingMutex.lock();
    d->summaryItemList.append(item);
    d->pendingMutex.unlock();
    item->increaseRef();

    requestTriggerPrint(item);
}

/**************************************************************************************************
 * 添加一个生命报警项目
 *************************************************************************************************/
void SummaryStorageManager::addOneshotAlarm(unsigned alarmTime)
{
    Q_D(SummaryStorageManager);
    SummaryItem *item = OneShotAlarmSummaryItem::create(alarmTime);
    d->pendingMutex.lock();
    d->summaryItemList.append(item);
    d->pendingMutex.unlock();
    item->increaseRef();
    requestTriggerPrint(item);
}

/**************************************************************************************************
 * 添加一个Code Marker Summary
 *************************************************************************************************/
void SummaryStorageManager::addCodeMarker(unsigned time, const char *codeName)
{
    Q_D(SummaryStorageManager);
    SummaryItem *item = CodeMarkerSummaryItem::create(time, codeName);
    d->pendingMutex.lock();
    d->summaryItemList.append(item);
    d->pendingMutex.unlock();
    item->increaseRef();

    requestTriggerPrint(item);
}

/***************************************************************************************************
 * add a nibp summary
 **************************************************************************************************/
void SummaryStorageManager::addNIBP(unsigned time, int errorCode)
{
    Q_D(SummaryStorageManager);
    //collect trend data first
    trendCache.collectTrendData(time, true);
    SummaryItem *item = NIBPSummaryItem::create(time, (NIBPOneShotType)errorCode);
    d->pendingMutex.lock();
    d->summaryItemList.append(item);
    d->pendingMutex.unlock();
    item->increaseRef();

    requestTriggerPrint(item);
}

/**************************************************************************************************
 * 添加一个Printer Action Summary
 *************************************************************************************************/
void SummaryStorageManager::addPrinterAction(unsigned time)
{
    Q_D(SummaryStorageManager);
    SummaryItem *item = PrinterActionSummaryItem::create(time);
    d->pendingMutex.lock();
    d->summaryItemList.append(item);
    d->pendingMutex.unlock();
    item->increaseRef();
}

/**************************************************************************************************
 * 添加一个Presenting rhythm summary
 *************************************************************************************************/
void SummaryStorageManager::addPrensentingRhythm(unsigned time)
{
    Q_D(SummaryStorageManager);
    SummaryItem *item = PresentingRhythmSummaryItem::create(time);
    d->pendingMutex.lock();
    d->summaryItemList.append(item);
    d->pendingMutex.unlock();
    item->increaseRef();

    requestTriggerPrint(item);
}

/***************************************************************************************************
 * add a diagnostic ecg snapshot
 **************************************************************************************************/
void SummaryStorageManager::addDiagnosticECG(unsigned time, ECGBandwidth diagBandwidth)
{
    Q_D(SummaryStorageManager);
    SummaryItem *item = DiagnosticEcgSummaryItem::create(time, diagBandwidth);
    d->pendingMutex.lock();
    d->summaryItemList.append(item);
    d->pendingMutex.unlock();
    item->increaseRef();

    requestTriggerPrint(item);
}

/***************************************************************************************************
 * get the physical alarm message
 **************************************************************************************************/
QString SummaryStorageManager::getPhyAlarmMessage(ParamID paramId, int alarmType, bool isOneShot)
{
    switch(paramId)
    {
    case PARAM_ECG:
        if(isOneShot)
        {
            return ECGSymbol::convert((ECGOneShotType)alarmType, ecgParam.getLeadConvention());
        }
        else
        {
            //no limit alarm
        }
        break;
    case PARAM_DUP_ECG:
        if(isOneShot)
        {
            //no oneshot alarm
        }
        else
        {
            return ECGSymbol::convert((ECGDupLimitAlarmType)alarmType);
        }
        break;
    case PARAM_SPO2:
        if(isOneShot)
        {
            return SPO2Symbol::convert((SPO2OneShotType)alarmType);
        }
        else
        {
            return SPO2Symbol::convert((SPO2LimitAlarmType)alarmType);
        }
        break;
    case PARAM_RESP:
        if(isOneShot)
        {
            return RESPSymbol::convert((RESPOneShotType)alarmType);
        }
        else
        {
            //no limit alarm
        }
        break;
    case PARAM_DUP_RESP:
        if(isOneShot)
        {
            //no oneshot alarm
        }
        else
        {
            return RESPSymbol::convert((RESPDupLimitAlarmType)alarmType);
        }
        break;
    case PARAM_NIBP:
        if(isOneShot)
        {
            return NIBPSymbol::convert((NIBPOneShotType)alarmType);
        }
        else
        {
            return NIBPSymbol::convert((NIBPLimitAlarmType)alarmType);
        }
        break;
    case PARAM_CO2:
        if(isOneShot)
        {
            return CO2Symbol::convert((CO2OneShotType)alarmType);
        }
        else
        {
            return CO2Symbol::convert((CO2LimitAlarmType)alarmType);
        }
        break;
    case PARAM_TEMP:
        if(isOneShot)
        {
            return TEMPSymbol::convert((TEMPOneShotType)alarmType);
        }
        else
        {
            return TEMPSymbol::convert((TEMPLimitAlarmType)alarmType);
        }
        break;
    default:
        break;
    }
    return "";
}

/***************************************************************************************************
 * saveDataCallback : call back to update summary header
 **************************************************************************************************/
void SummaryStorageManager::saveDataCallback(quint32 dataID, const char *data, quint32 len)
{
    Q_UNUSED(len);
    Q_UNUSED(dataID);
    Q_D(SummaryStorageManager);
    SummaryItemDesc *desc = (SummaryItemDesc *) data;
    d->summaryHead.lastItemTime = desc->time;
}
