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

static DataStorageDirManagerInterface *currentIface = NULL;
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
