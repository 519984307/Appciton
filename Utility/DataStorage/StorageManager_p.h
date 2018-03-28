#pragma once
#include "StorageManager.h"
#include <QList>
#include <QByteArray>
#include <QMutex>

struct StorageItem
{
    StorageItem():datatype(0), extraData(0) {}

    StorageItem(quint32 type, const QByteArray &content, quint32 extraData)
        : datatype(type), content(content), extraData(extraData)
    {}

    StorageItem(quint32 type, char *buf, quint32 len, quint32 extraData = 0)
        : datatype(type), content(buf, len), extraData(extraData) {

    }
    quint32 datatype;
    QByteArray content;
    quint32 extraData;
};

class StorageManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(StorageManager)

    StorageManagerPrivate(StorageManager *q_ptr):q_ptr(q_ptr), backend(NULL), currentCacheList(0){}
    virtual ~StorageManagerPrivate();

    void switchList();

    StorageManager * const q_ptr;
    IStorageBackend *backend;
    QList<StorageItem> storageItemList[2];
    QMutex mutex;
    quint8 currentCacheList;
};
