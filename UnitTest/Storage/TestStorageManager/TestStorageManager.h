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
#include "StorageFile.h"

class TestStorageManager : public QObject
{
    Q_OBJECT

public:
    TestStorageManager();
    ~TestStorageManager();

private slots:
    void init();
    void cleanup();
    void testOpen_data();
    void testOpen();
    void testSaveData_data();
    void testSaveData();

private:
    QTemporaryFile *m_File;
    StorageFile *backend;
};
#endif // TESTSTORAGEMANAGER_H
