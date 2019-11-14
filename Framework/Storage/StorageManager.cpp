/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/30
 **/


#include "StorageManager_p.h"
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include "StorageFile.h"

QList<StorageManager*> StorageManagerPrivate::activeStorageManagers;

StorageManagerPrivate::~StorageManagerPrivate()
{
    delete backend;
}

/***************************************************************************************************
 * switchList : switch storage cache list
 **************************************************************************************************/
void StorageManagerPrivate::switchList()
{
    mutex.lock();
    currentCacheList = !currentCacheList;
    mutex.unlock();
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
StorageManager::StorageManager(IStorageBackend *backend)
    : d_ptr(new StorageManagerPrivate(this))
{
    Q_D(StorageManager);
    d->backend = backend;
    d->activeStorageManagers.append(this);
}

/***************************************************************************************************
 * destructor
 **************************************************************************************************/
StorageManager::~StorageManager()
{
    Q_D(StorageManager);
    d->mutex.lock();
    d->storageItemList[d->currentCacheList].append(d->storageItemList[!d->currentCacheList]);
    d->mutex.unlock();
    saveData();
    d->activeStorageManagers.removeAll(this);
}

/***************************************************************************************************
 * saveData : save data to backend, return the sum of all data length
 **************************************************************************************************/
quint32 StorageManager::saveData()
{
    Q_D(StorageManager);
    if (!d->backend)
    {
        return 0;
    }

    StorageItem item;
    quint32 totalLength = 0;

    // flush item
    d->mutex.lock();
    while (!d->storageItemList[!d->currentCacheList].isEmpty())
    {
        item = d->storageItemList[!d->currentCacheList].takeFirst();
        d->mutex.unlock();
        saveDataCallback(item.datatype, item.content.constData(), item.content.length());
        totalLength += d->backend->appendBlockData(item.datatype, item.content.constData(), item.content.length(),
                       item.extraData);
        d->mutex.lock();
    }
    d->mutex.unlock();



    // switch cache list
    d->switchList();
    return totalLength;
}

/***************************************************************************************************
 * discard the data int the cache list
 **************************************************************************************************/
void StorageManager::discardData()
{
    Q_D(StorageManager);

    StorageItem item;
    d->mutex.lock();
    while (!d->storageItemList[!d->currentCacheList].isEmpty())
    {
        item = d->storageItemList[!d->currentCacheList].takeFirst();
    }
    d->mutex.unlock();

    d->switchList();
}

/***************************************************************************************************
 * backend : get the storage backend
 **************************************************************************************************/
IStorageBackend *StorageManager::backend() const
{
    Q_D(const StorageManager);
    return d->backend;
}

/***************************************************************************************************
 * addData : Add data to cache, the data will no write immediately, the data will write to backend
 *           when saveData() is called.
 **************************************************************************************************/
void StorageManager::addData(quint32 dataID, const char *data, quint32 len, quint32 extraData)
{
    Q_D(StorageManager);
    if (data == NULL || len == 0)
    {
        return;
    }
    char *buf = const_cast<char *>(data);
    StorageItem item(dataID, buf, len, extraData);
    QMutexLocker(&d->mutex);
    d->storageItemList[d->currentCacheList].append(item);
}

/***************************************************************************************************
 * addData : Add data to cache, the data will no write immediately, the data will write to backend
 *           when saveData() is called.
 **************************************************************************************************/
void StorageManager::addData(quint32 dataID, const QByteArray &data, quint32 extraData)
{
    Q_D(StorageManager);
    StorageItem item(dataID, data, extraData);
    QMutexLocker(&d->mutex);
    d->storageItemList[d->currentCacheList].append(item);
}

/***************************************************************************************************
 * size : get the current storage backend size, return 0 if no backend
 **************************************************************************************************/
quint32 StorageManager::size() const
{
    Q_D(const StorageManager);

    if (d->backend)
    {
        return d->backend->fileSize();
    }
    return 0;
}

/***************************************************************************************************
 * open : open a storage file and return the storage backend object to manipulate the file
 *        Use should explicit delete the storage backend obj to avoid memory leap.
 **************************************************************************************************/
IStorageBackend *StorageManager::open(const QString &filename, QIODevice::OpenMode openmode)
{
    /* TODO: make a backend factory; */
    return new StorageFile(filename, openmode);
}


/***************************************************************************************************
 * exist : check whether a storage file exists
 **************************************************************************************************/
bool StorageManager::exist(const QString &filename)
{
    StorageFile f(filename, QIODevice::ReadOnly);
    return f.isValid();
}

QList<StorageManager *> StorageManager::getActiveStorageManager()
{
    return StorageManagerPrivate::activeStorageManagers;
}

/***************************************************************************************************
 * constructor : call by inherited class to perforem Q and D pointer
 **************************************************************************************************/
StorageManager::StorageManager(StorageManagerPrivate *d_ptr, IStorageBackend *backend)
    : d_ptr(d_ptr)
{
    Q_D(StorageManager);
    d->backend = backend;
    d->activeStorageManagers.append(this);
}

void StorageManager::saveDataCallback(quint32 dataID, const char *data, quint32 len)
{
    Q_UNUSED(dataID);
    Q_UNUSED(data);
    Q_UNUSED(len);
}
