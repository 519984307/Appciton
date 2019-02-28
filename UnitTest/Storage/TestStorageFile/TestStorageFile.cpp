/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/2/28
 **/

#include <QString>
#include <QtTest>
#include "StorageFile.h"

class TestStorageFile : public QObject
{
    Q_OBJECT

public:
    TestStorageFile();

private Q_SLOTS:
    void init();
    void cleanup();
    void testFileType();
    void testFileSize();
    void testIsValid();
    void testSetReservedSize();
    void testGetReservedSize();
    void testReload();
    void testGetBlockNR();
    void testWriteAdditionalData();

private:
    StorageFile *m_File1;
    StorageFile *m_File2;
};

TestStorageFile::TestStorageFile()
    : m_File1(NULL), m_File2(NULL)
{
}

void TestStorageFile::init()
{
    m_File1 = new StorageFile();
    m_File2 = new StorageFile(":/TrendData", QIODevice::ReadWrite);
}

void TestStorageFile::cleanup()
{
    delete m_File1;
    delete m_File2;
}


void TestStorageFile::testFileType()
{
    QVERIFY(m_File1->fileType() == StorageFile::Type);
    QVERIFY(m_File2->fileType() == StorageFile::Type);
}

void TestStorageFile::testFileSize()
{
    QVERIFY(m_File1->fileSize() == 0);
    QVERIFY(m_File2->fileSize() == 404);
}

void TestStorageFile::testIsValid()
{
   QCOMPARE(m_File1->isValid(), false);
   QCOMPARE(m_File2->isValid(), true);
}

void TestStorageFile::testSetReservedSize()
{
    QCOMPARE(m_File1->setReservedSize(100), true);
    QCOMPARE(m_File2->setReservedSize(100), false);
}

void TestStorageFile::testGetReservedSize()
{
    m_File1->setReservedSize(100);
    m_File2->setReservedSize(100);

    QVERIFY(m_File1->getReservedSize() == 100);
    QVERIFY(m_File2->getReservedSize() != 100);
}

void TestStorageFile::testReload()
{
    m_File1->reload(":/TrendData", QIODevice::ReadOnly);
    m_File2->reload(":/TrendData", QIODevice::ReadOnly);

    QCOMPARE(m_File1->isValid(), true);
    QCOMPARE(m_File2->isValid(), true);
}

void TestStorageFile::testGetBlockNR()
{
    QVERIFY(m_File1->getBlockNR() == 0);
    QVERIFY(m_File2->getBlockNR() > 0);
}

void TestStorageFile::testWriteAdditionalData()
{
    int num = 12;
    QVERIFY(m_File1->writeAdditionalData(reinterpret_cast<char *>(&num), 4) == 0);
    QVERIFY(m_File2->writeAdditionalData(reinterpret_cast<char *>(&num), 4) > 0);
}

QTEST_APPLESS_MAIN(TestStorageFile)

#include "TestStorageFile.moc"
