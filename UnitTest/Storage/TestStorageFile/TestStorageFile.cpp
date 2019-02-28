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
    void testReadAdditionalData();
    void testGetBlockDataLen();
    void testGetBlockData();

private:
    StorageFile *m_StorageFile1;
    StorageFile *m_StorageFile2;
    QTemporaryFile *m_File;
};

static void createTestFileOnDisk(QTemporaryFile *tempFile, const QString &resFilename)
{
    QFile resFile(resFilename);
    QCOMPARE(resFile.open(QIODevice::ReadOnly), true);
    tempFile->open();
    tempFile->write(resFile.readAll());
    resFile.close();
    tempFile->close();
}

TestStorageFile::TestStorageFile()
    : m_StorageFile1(NULL), m_StorageFile2(NULL), m_File(NULL)
{
}

void TestStorageFile::init()
{
    m_StorageFile1 = new StorageFile();
    m_File = new QTemporaryFile();
    createTestFileOnDisk(m_File, ":/TrendData.seq");
    m_StorageFile2 = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
}

void TestStorageFile::cleanup()
{
    delete m_StorageFile1;
    delete m_StorageFile2;
}


void TestStorageFile::testFileType()
{
    QVERIFY(m_StorageFile1->fileType() == StorageFile::Type);
    QVERIFY(m_StorageFile2->fileType() == StorageFile::Type);
}

void TestStorageFile::testFileSize()
{
    QVERIFY(m_StorageFile1->fileSize() == 0);
    QVERIFY(m_StorageFile2->fileSize() > 0);
}

void TestStorageFile::testIsValid()
{
    QCOMPARE(m_StorageFile1->isValid(), false);
    QCOMPARE(m_StorageFile2->isValid(), true);
}

void TestStorageFile::testSetReservedSize()
{
    QCOMPARE(m_StorageFile1->setReservedSize(100), true);
    QCOMPARE(m_StorageFile2->setReservedSize(100), true);
}

void TestStorageFile::testGetReservedSize()
{
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);

    QVERIFY(m_StorageFile1->getReservedSize() == 100);
    QVERIFY(m_StorageFile2->getReservedSize() == 100);
}

void TestStorageFile::testReload()
{
    m_StorageFile1->reload(m_File->fileName(), QIODevice::ReadWrite);
    m_StorageFile2->reload(m_File->fileName(), QIODevice::ReadWrite);

    QCOMPARE(m_StorageFile1->isValid(), true);
    QCOMPARE(m_StorageFile2->isValid(), true);
}

void TestStorageFile::testGetBlockNR()
{
    int data = 0;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&data), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&data), 4);

    QVERIFY(m_StorageFile1->getBlockNR() == 0);
    QVERIFY(m_StorageFile2->getBlockNR() == 1);
}

void TestStorageFile::testWriteAdditionalData()
{
    int data = 0;
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);

    QVERIFY(m_StorageFile1->writeAdditionalData(reinterpret_cast<char *>(&data), 4) == 0);
    QVERIFY(m_StorageFile2->writeAdditionalData(reinterpret_cast<char *>(&data), 4) == 4);
}

void TestStorageFile::testReadAdditionalData()
{
    char *data1 = new char[10];
    char *data2 = new char[10];
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int bloackData;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&bloackData), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&bloackData), 4);

    QVERIFY(m_StorageFile1->readAdditionalData(data1, 4) == 0);
    QVERIFY(m_StorageFile2->readAdditionalData(data2, 4) == 4);
}

void TestStorageFile::testGetBlockDataLen()
{
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int bloackData;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&bloackData), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&bloackData), 4);

    QVERIFY(m_StorageFile1->getBlockDataLen(0) == 0);
    QVERIFY(m_StorageFile2->getBlockDataLen(0) == 4);
}

void TestStorageFile::testGetBlockData()
{
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int bloackData;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&bloackData), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&bloackData), 4);

    QByteArray byteArray1 = m_StorageFile1->getBlockData(0);
    QByteArray byteArray2 = m_StorageFile2->getBlockData(0);

    QVERIFY(byteArray1.count() == 0);
    QVERIFY(byteArray2.count() == 4);
}

QTEST_MAIN(TestStorageFile)

#include "TestStorageFile.moc"
