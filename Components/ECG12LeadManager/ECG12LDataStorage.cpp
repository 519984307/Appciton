#include "../Utility/DataStorage/StorageManager_p.h"
#include "ECG12LDataStorage.h"
#include "StorageFile.h"
#include "DataStorageDirManager.h"
#include "TimeManager.h"
#include "SupervisorConfigManager.h"
#include "Utility.h"
#include "PatientManager.h"
#include "ECGParam.h"
#include "Alarm.h"
#include "TimeDate.h"

ECG12LDataStorage *ECG12LDataStorage::_selfObj=NULL;

class ECG12LDataStoragePrivate : public StorageManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(ECG12LDataStorage)

    ECG12LDataStoragePrivate(ECG12LDataStorage *q_ptr)
        :StorageManagerPrivate(q_ptr){}
};

/**************************************************************************************************
 * 构造
 *************************************************************************************************/
ECG12LDataStorage::ECG12LDataStorage()
    :StorageManager(new ECG12LDataStoragePrivate(this), new StorageFile())
{
    createDir();
}

void ECG12LDataStorage::createDir()
{
    Q_D(ECG12LDataStorage);
    d->backend->reload(dataStorageDirManager.getCurFolder() + ECG12L_FILE_NAME, QIODevice::ReadWrite);
    _identifier = 10;
}

ECG12LDataStorage::~ECG12LDataStorage()
{
    _identifier = 10;
}

/**************************************************************************************************
 * 添加一个12L数据
 *************************************************************************************************/
bool ECG12LDataStorage::add12LData(ECG12LDataStorage::ECG12LeadData *data)
{
    if (NULL == data)
    {
        return false;
    }

    addData(0, (char *) data, sizeof(ECG12LDataStorage::ECG12LeadData));

    return true;
}

/**************************************************************************************************
 * 获取common信息
 *************************************************************************************************/
void ECG12LDataStorage::getCommonInfo(ECG12LDataStorage::ECG12LCommonInfo &commonInfo)
{
    commonInfo.timeStamp = timeManager.getCurTime() - 10;
    commonInfo.rescueStartTime = timeManager.getStartTime();
    commonInfo.ms = timeDate.getTimeMsec();

    QString tmpStr;
    superConfig.getStrValue("General|DeviceIdentifier", tmpStr);
    if(tmpStr.isNull())
    {
        tmpStr = QString();
    }
    //memcpy(commonInfo.deviceID, tmpStr.toLocal8Bit().constData(), sizeof(commonInfo.deviceID));
    Util::strlcpy((char *)commonInfo.deviceID, tmpStr.toLocal8Bit().constData(),
                  sizeof(commonInfo.deviceID));

    tmpStr.clear();
    machineConfig.getStrValue("SerialNumber", tmpStr);
    if(tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy((char *)commonInfo.serialsNum, tmpStr.toLocal8Bit().constData(),
               sizeof(commonInfo.serialsNum));

    Util::strlcpy((char *)commonInfo.patientID, patientManager.getPatID(), MAX_PATIENT_ID_LEN);
    Util::strlcpy((char *)commonInfo.patientName, patientManager.getName(), MAX_PATIENT_NAME_LEN);
    commonInfo.patientAge = patientManager.getAge();
    commonInfo.patientSex = patientManager.getSex();

    commonInfo.bandWidth = (unsigned char)ecgParam.getBandwidth();
    commonInfo.gain = ecgParam.get12LGain(ECG_LEAD_I);
    commonInfo.notchFilter = (unsigned char)ecgParam.getNotchFilter();
    commonInfo.hr = ecgParam.getHR();

    commonInfo.pacerMarkersOn = ecgParam.getPacermaker();

    QString tempstr = dataStorageDirManager.getFDFileName();
    tempstr.remove(QChar('/'));
    Util::strlcpy((char *)commonInfo.fulldisclosureName, tempstr.toLocal8Bit().constData(),
                  sizeof(commonInfo.fulldisclosureName));
}

/**************************************************************************************************
 * 循环运行
 *************************************************************************************************/
void ECG12LDataStorage::run()
{
    if(dataStorageDirManager.isCurRescueFolderFull())
    {
        //clear the backup list, save nothing
       discardData();
       return;
    }
    dataStorageDirManager.addDataSize(saveData());
}
