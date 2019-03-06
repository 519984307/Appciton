/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/6
 **/

#ifndef TESTSTORAGEMANAGER_H
#define TESTSTORAGEMANAGER_H
#include <QString>
#include <QtTest>
#include "StorageManager.h"

class TestStorageManager : public QObject
{
    Q_OBJECT

public:
    TestStorageManager();
    ~TestStorageManager();

private slots:
    void init();
    void cleanup();
    void testSaveData();
};
#endif // TESTSTORAGEMANAGER_H
