#include "StorageManager_p.h"
#include "TrendDataStorageManager.h"
#include "StorageFile.h"
#include "DataStorageDirManager.h"
#include "TimeManager.h"
#include "IConfig.h"
#include "Utility.h"
#include "PatientManager.h"
#include "SystemManager.h"
#include "TrendCache.h"
#include "ParamManager.h"
#include <QVector>
#include "Alarm.h"
#include "ECGDupParam.h"
#include "RESPDupParam.h"
#include "TimeDate.h"

class TrendDataStorageManagerPrivate: public StorageManagerPrivate
{
public:
    TrendDataStorageManagerPrivate(TrendDataStorageManager * q_ptr)
        :StorageManagerPrivate(q_ptr), firstSave(false), lastStoreTimestamp(0)
    {
        memset(&dataDesc, 0, sizeof(TrendDataDescription));
    }


    //check and update addition info
    void updateAdditionInfo();

    TrendDataDescription dataDesc;
    bool firstSave; //first time save after initialize
    unsigned lastStoreTimestamp;
};

void TrendDataStorageManagerPrivate::updateAdditionInfo()
{
    bool changed = false;

    if(strcmp(dataDesc.patientName, patientManager.getName()))
    {
        Util::strlcpy(dataDesc.patientName, patientManager.getName(), MAX_PATIENT_NAME_LEN);
        changed = true;
    }

    if(strcmp(dataDesc.patientID, patientManager.getPatID()))
    {
        Util::strlcpy(dataDesc.patientID, patientManager.getName(), MAX_PATIENT_ID_LEN);
        changed = true;
    }

    if(dataDesc.patientAge != patientManager.getAge())
    {
        dataDesc.patientAge = patientManager.getAge();
        changed = true;
    }

    if(dataDesc.patientGender != patientManager.getSex())
    {
        dataDesc.patientGender = patientManager.getSex();
        changed = true;
    }

    if(dataDesc.patientHeight != patientManager.getHeight())
    {
        dataDesc.patientHeight = patientManager.getHeight();
        changed = true;
    }

    if(dataDesc.patientType != patientManager.getType())
    {
        dataDesc.patientType = patientManager.getType();
        changed = true;
    }

    if(dataDesc.patientWeight != patientManager.getWeight())
    {
        dataDesc.patientWeight = patientManager.getWeight();
    }

    if(!changed && !firstSave)
    {
        return;
    }

    backend->writeAdditionalData((char *) &dataDesc, sizeof(dataDesc));
    firstSave = false;
}

TrendDataStorageManager &TrendDataStorageManager::getInstance()
{
    static TrendDataStorageManager * instance = NULL;
    if(instance == NULL)
    {
        instance = new TrendDataStorageManager();
    }
    return *instance;
}

TrendDataStorageManager::~TrendDataStorageManager()
{

}

void TrendDataStorageManager::periodRun(unsigned t)
{
    Q_D(TrendDataStorageManager);

    if (0 != t % 5 || d->lastStoreTimestamp == t)
    {
        return;
    }

    storeData(t, CollectStatusPeriod);
}

void TrendDataStorageManager::storeData(unsigned t, TrendDataFlags dataStatus)
{
    Q_D(TrendDataStorageManager);
    if(dataStorageDirManager.isCurRescueFolderFull())
    {
        return;
    }

    TrendCacheData data;
    if(!trendCache.getTendData(t, data))
    {
        //no trend data
        return;
    }

    d->lastStoreTimestamp = t;


    QList<ParamID> idList;
    paramManager.getParams(idList);

    QVector<TrendValueSegment> valueSegments;

    bool hasAlarm = false;
    ParamID paramId = PARAM_NONE;
    //collect sub param that have valid value
    for(int i = 0; i < SUB_PARAM_NR; i++)
    {
        paramId = paramInfo.getParamID((SubParamID) i);
        if(-1 == idList.indexOf(paramId))
        {
            continue;
        }

        if(!data.values.contains((SubParamID)i))
        {
            continue;
        }

        TrendValueSegment valueSegment;
        valueSegment.subParamId = i;
        valueSegment.value = data.values.value((SubParamID)i);
        valueSegment.alarmFlag = alertor.getAlarmSourceStatus(paramInfo.getParamName(paramId), (SubParamID) i);
        if(!hasAlarm)
        {
            hasAlarm = valueSegment.alarmFlag;
        }

        valueSegments.append(valueSegment);
    }

    int dataSize = sizeof(TrendDataSegment) + valueSegments.size() * sizeof(TrendValueSegment);
    QByteArray content(dataSize, 0);

    TrendDataSegment *dataSegment = (TrendDataSegment *) content.data();

    dataSegment->timestamp = t;
    dataSegment->co2Baro = data.co2baro;
    dataSegment->status = dataStatus;

    if(ecgDupParam.getHrSource() == ECGDupParam::HR_SOURCE_SPO2)
    {
        dataSegment->status |= HRSourceIsSpo2;
    }

    if(respDupParam.getBrSource() == RESPDupParam::BR_SOURCE_RESP)
    {
        dataSegment->status |= BRSourceIsResp;
    }

    if(hasAlarm)
    {
        dataSegment->status |= HasAlarm;
    }

    dataSegment->trendValueNum = valueSegments.size();

    //copy the trend values
    qMemCopy((char *)dataSegment + sizeof(TrendDataSegment), valueSegments.constData(), sizeof(TrendValueSegment) * valueSegments.size());

    addData(0, content);
}

TrendDataPackage TrendDataStorageManager::parseTrendSegment(const TrendDataSegment *dataSegment)
{
    TrendDataPackage dataPackage;
    if(dataSegment)
    {
        dataPackage.time = dataSegment->timestamp;
        dataPackage.co2Baro = dataSegment->co2Baro;
        for(int i = 0; i < dataSegment->trendValueNum; i++)
        {
            dataPackage.subparamValue[(SubParamID)dataSegment->values[i].subParamId] = dataSegment->values[i].value;
            dataPackage.subparamAlarm[(SubParamID)dataSegment->values[i].subParamId] = dataSegment->values[i].alarmFlag;
            if (!dataPackage.alarmFlag && dataSegment->values[i].alarmFlag)
            {
                dataPackage.alarmFlag = dataSegment->values[i].alarmFlag;
            }
        }
    }
    return dataPackage;
}

/* move data from memory to flash */
void TrendDataStorageManager::run()
{
    Q_D(TrendDataStorageManager);
    d->updateAdditionInfo();

    if(dataStorageDirManager.isCurRescueFolderFull())
    {
        discardData();
        return;
    }

    dataStorageDirManager.addDataSize(saveData());
}

TrendDataStorageManager::TrendDataStorageManager()
    :StorageManager(new TrendDataStorageManagerPrivate(this), new StorageFile())
{
    Q_D(TrendDataStorageManager);
    d->backend->reload(dataStorageDirManager.getCurFolder() + TREND_DATA_FILE_NAME,
                       QIODevice::ReadWrite);
    d->backend->setReservedSize(sizeof(TrendDataDescription));
    d->backend->readAdditionalData((char *) &d->dataDesc, sizeof(TrendDataDescription));

    if(d->dataDesc.startTime != 0)
    {
        //read a exist file, no need to initialize
        return;
    }

    //initialize the file
    d->dataDesc.startTime = timeManager.getStartTime();

    d->dataDesc.moduleConfig = systemManager.getModuleConfig();

    //device id;
    QString tmpStr = QString();
    systemConfig.getStrValue("General|DeviceID", tmpStr);
    if(tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy(d->dataDesc.deviceID, tmpStr.toLocal8Bit().constData(),
                  sizeof(d->dataDesc.deviceID));

    //serial number
    machineConfig.getStrValue("SerialNumber",  tmpStr);
    if(tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy(d->dataDesc.serialNum, tmpStr.toLocal8Bit().constData(),
                  sizeof(d->dataDesc.serialNum));

    //other field will initialize when write date to flash
    d->firstSave = true;
}

