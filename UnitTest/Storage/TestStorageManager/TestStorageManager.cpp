/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/6
 **/

#include "TestStorageManager.h"

static void createTestFileOnDisk(QTemporaryFile *tempFile, const QString &resFilename)
{
    QFile resFile(resFilename);
    QCOMPARE(resFile.open(QIODevice::ReadOnly), true);
    tempFile->open();
    tempFile->write(resFile.readAll());
    resFile.close();
    tempFile->close();
}

TestStorageManager::TestStorageManager()
    : m_File(NULL), backend(NULL)
{
}

TestStorageManager::~TestStorageManager()
{
}

void TestStorageManager::init()
{
    m_File = new QTemporaryFile();
    createTestFileOnDisk(m_File, ":/EventData.seq");
    backend = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
}

void TestStorageManager::cleanup()
{
    delete m_File;
    m_File = NULL;
    backend->remove(backend);
    delete backend;
    backend = NULL;
}

void TestStorageManager::testOpen_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("result");

    QTest::newRow("valid file name") << m_File->fileName() << true;
}

void TestStorageManager::testOpen()
{
}

void TestStorageManager::testSaveData_data()
{
}

void TestStorageManager::testSaveData()
{
}
