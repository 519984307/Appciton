#include "OxyCRGStorageManager.h"
#include "StorageManager_p.h"
#include "StorageFile.h"
#include "DataStorageDirManager.h"
#include "WindowManager.h"
#include "OxyCRGStorageItem.h"
#include <QList>
#include <QMutex>

class OxyCRGStorageManagerPrivate: public StorageManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(OxyCRGStorageManager)

    OxyCRGStorageManagerPrivate(OxyCRGStorageManager *q_ptr)
        :StorageManagerPrivate(q_ptr)
    {

    }


//    QList<WaveformID> getStoreWaveList(WaveformID paramWave);

    QList<OxyCRGStorageItem *> eventItemList;
    QMutex mutex;
};

OxyCRGStorageManager &OxyCRGStorageManager::getInstance()
{
    static OxyCRGStorageManager *instance = NULL;
    if(instance == NULL)
    {
        instance = new OxyCRGStorageManager();
    }
    return *instance;
}

OxyCRGStorageManager::~OxyCRGStorageManager()
{

}

void OxyCRGStorageManager::run()
{
    if(dataStorageDirManager.isCurRescueFolderFull())
    {
        discardData();
        return;
    }

    dataStorageDirManager.addDataSize(saveData());
}

OxyCRGStorageManager::OxyCRGStorageManager()
    :StorageManager(new OxyCRGStorageManagerPrivate(this), new StorageFile())
{
    Q_D(OxyCRGStorageManager);
    d->backend->reload(dataStorageDirManager.getCurFolder() + OXYCRG_DATA_FILE_NAME, QIODevice::ReadWrite);

}
