/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/20
 **/

#include "TestDataStorageDirManager.h"
#include "DataStorageDirManager.h"

TestDataStorageDirManager::TestDataStorageDirManager()
{
}

void TestDataStorageDirManager::init()
{
    dataStorageDirManager.getInstance();
}

void TestDataStorageDirManager::cleanup()
{
}

void TestDataStorageDirManager::testCreateDir()
{
}
