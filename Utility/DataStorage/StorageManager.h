/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/30
 **/


#pragma once
#include <QObject>
#include <QScopedPointer>
#include "IStorageBackend.h"

/***************************************************************************************************
 * StorageManager : Use to perform data storage, Storage manager need a storage backend to store
 *                  the data in to flash.
 **************************************************************************************************/
class StorageManagerPrivate;
class StorageManager : public QObject
{
    Q_OBJECT
public:
    explicit StorageManager(IStorageBackend *backend = NULL);
    virtual ~StorageManager();
    // save the internal cache data through the backend
    quint32 saveData();

    // discard data in the internal cache
    void discardData();

    // get the storage backend
    IStorageBackend *backend() const;
    // add the data into the internal cache, no write to flash yet
    void addData(quint32 dataID, const char *data, quint32 len, quint32 extraData = 0);
    // overload
    void addData(quint32 dataID, const QByteArray &data, quint32 extraData = 0);
    // return the size of storage backend
    quint32 size() const;

    // open a storage file
    static IStorageBackend *open(const QString &filename, QIODevice::OpenMode openmode);

    // check whether a storage file exists
    static bool exist(const QString &filename);

    virtual void createDir() { }

protected:
    const QScopedPointer<StorageManagerPrivate> d_ptr;
    StorageManager(StorageManagerPrivate *d_ptr, IStorageBackend *backend);
    // callback when trying to save a block of data
    virtual void saveDataCallback(quint32 dataID, const char *data, quint32 len);

    /**
     * @brief newPatientHandle　新建病人后的处理
     */
    virtual void newPatientHandle(){}

private slots:
    /**
     * @brief onNewPatientHandle 新建病人后响应函数
     */
    void onNewPatientHandle();

private:
    Q_DECLARE_PRIVATE(StorageManager)
    Q_DISABLE_COPY(StorageManager)
};
