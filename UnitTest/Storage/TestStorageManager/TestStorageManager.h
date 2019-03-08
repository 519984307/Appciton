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
#include <QTest>
#include <QTemporaryFile>
#include "StorageManager.h"
#include "StorageFile.h"
#include <QObject>

class TestStorageManager : public QObject
{
    Q_OBJECT

public:
    TestStorageManager();
    ~TestStorageManager();

    StorageFile *getBackend() const;
    void setBackend(StorageFile *value);

private slots:
    void init();
    void cleanup();
    void testOpen_data();
    void testOpen();
    void testAddSaveData_data();
    void testAddSaveData();
    void testDiscardData_data();
    void testDiscardData();
    void testBackend();
    void testSize_data();
    void testSize();
    void testExist_data();
    void testExist();

private:
    QTemporaryFile *m_File;
    StorageFile *m_Backend;
};
#endif // TESTSTORAGEMANAGER_H
