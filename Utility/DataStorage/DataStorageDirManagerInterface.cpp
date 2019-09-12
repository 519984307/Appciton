/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/6
 **/

#include "DataStorageDirManagerInterface.h"
#include "DataStorageDefine.h"
#include <QDir>

static DataStorageDirManagerInterface *currentIface = NULL;
static QString storageDir = DATA_STORE_PATH;
DataStorageDirManagerInterface *DataStorageDirManagerInterface::registerDataStorageDirManager(
    DataStorageDirManagerInterface *interface)
{
    DataStorageDirManagerInterface *old = currentIface;
    currentIface = interface;
    return old;
}

DataStorageDirManagerInterface *DataStorageDirManagerInterface::getDataStorageDirManager()
{
    return currentIface;
}

bool DataStorageDirManagerInterface::setDataStorageDir(const QString &dir)
{
    if (currentIface || !QDir(dir).exists())
    {
        return false;
    }
    storageDir = dir;
    if (!storageDir.endsWith('/')) {
        storageDir.append('/');
    }
    return true;
}

const QString &DataStorageDirManagerInterface::dataStorageDir()
{
    return storageDir;
}
