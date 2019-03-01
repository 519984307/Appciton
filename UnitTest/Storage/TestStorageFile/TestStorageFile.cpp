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
    void testGetBlockType();
    void testGetBlockInfo();
    void testGetBlockEntryList();
    void testReadBlockData();
    void testAppendBlockData();
    void testModifyBlockData();
    void testMapAdditionalData();
    void testUnmapAdditionalData();
    void testRemove();
    void testRename();

private:
    StorageFile *m_StorageFile1;
    StorageFile *m_StorageFile2;
    QTemporaryFile *m_File1;
    QTemporaryFile *m_File2;
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
    : m_StorageFile1(NULL), m_StorageFile2(NULL), m_File1(NULL), m_File2(NULL)
{
}

void TestStorageFile::init()
{
    m_File1 = new QTemporaryFile();
    createTestFileOnDisk(m_File1, ":/TrendData.seq");
    m_StorageFile1 = new StorageFile();
    m_StorageFile1->reload(m_File1->fileName(), QIODevice::ReadWrite);
    m_File2 = new QTemporaryFile();
    createTestFileOnDisk(m_File2, ":/TrendData.seq");
    m_StorageFile2 = new StorageFile(m_File2->fileName(), QIODevice::ReadWrite);
}

void TestStorageFile::cleanup()
{
    delete m_File1;
    m_File1 = NULL;
    delete m_File2;
    m_File2 = NULL;
    m_StorageFile1->remove(m_StorageFile1);
    delete m_StorageFile1;
    m_StorageFile1 = NULL;
    m_StorageFile2->remove(m_StorageFile2);
    delete m_StorageFile2;
    m_StorageFile2 = NULL;
}


void TestStorageFile::testFileType()
{
    QVERIFY(m_StorageFile1->fileType() == StorageFile::Type);
    QVERIFY(m_StorageFile2->fileType() == StorageFile::Type);
}

void TestStorageFile::testFileSize()
{
    QVERIFY(StorageFile().fileSize() == 0);
    QVERIFY(m_StorageFile1->fileSize() > 0);
    QVERIFY(m_StorageFile2->fileSize() > 0);
}

void TestStorageFile::testIsValid()
{
    QCOMPARE(StorageFile().isValid(), false);
    QCOMPARE(m_StorageFile1->isValid(), true);
    QCOMPARE(m_StorageFile2->isValid(), true);
}

void TestStorageFile::testSetReservedSize()
{
    QCOMPARE(m_StorageFile1->setReservedSize(100), true);
    QCOMPARE(m_StorageFile2->setReservedSize(100), true);
    int blockData;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&blockData), 4);
    QCOMPARE(m_StorageFile1->setReservedSize(100), false);      // 当已存在blockdata时返回false
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
    m_StorageFile1->reload(m_File1->fileName(), QIODevice::ReadWrite);
    m_StorageFile2->reload(m_File2->fileName(), QIODevice::ReadWrite);

    QCOMPARE(m_StorageFile1->isValid(), true);
    QCOMPARE(m_StorageFile2->isValid(), true);
}

void TestStorageFile::testGetBlockNR()
{
    int data = 0;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&data), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&data), 4);

    QVERIFY(m_StorageFile1->getBlockNR() == 1);
    QVERIFY(m_StorageFile2->getBlockNR() == 1);
}

void TestStorageFile::testWriteAdditionalData()
{
    int data = 0;
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);

    QVERIFY(StorageFile().writeAdditionalData(reinterpret_cast<char *>(&data), 4) == 0);
    QVERIFY(m_StorageFile1->writeAdditionalData(reinterpret_cast<char *>(&data), 4) == 4);
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

    QVERIFY(m_StorageFile1->readAdditionalData(data1, 4) == 4);
    QVERIFY(m_StorageFile2->readAdditionalData(data2, 4) == 4);
}

void TestStorageFile::testGetBlockDataLen()
{
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int bloackData;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&bloackData), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&bloackData), 4);

    QVERIFY(m_StorageFile1->getBlockDataLen(0) == 4);
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

    QVERIFY(byteArray1.count() == 4);
    QVERIFY(byteArray2.count() == 4);
}

void TestStorageFile::testGetBlockType()
{
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int blockData;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&blockData), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&blockData), 4);

    quint32 type = m_StorageFile1->fileType();
    QVERIFY(m_StorageFile1->getBlockType(0) == type);
    type = m_StorageFile2->fileType();
    QVERIFY(m_StorageFile2->getBlockType(0) == type);
}

void TestStorageFile::testGetBlockInfo()
{
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int blockData;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&blockData), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&blockData), 4);

    BlockEntry info1;
    BlockEntry info2;
    m_StorageFile1->getBlockInfo(0, info1);
    m_StorageFile2->getBlockInfo(0, info2);
    quint32 fileType1 = info1.type;
    quint32 fileType2 = info2.type;

    quint32 type = m_StorageFile1->fileType();
    QVERIFY(fileType1 == type);
    type = m_StorageFile2->fileType();
    QVERIFY(fileType2 == type);
}

void TestStorageFile::testGetBlockEntryList()
{
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int blockData;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&blockData), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&blockData), 4);

    QList<BlockEntry> list1;
    QList<BlockEntry> list2;
    m_StorageFile1->getBlockEntryList(list1);
    m_StorageFile2->getBlockEntryList(list2);

    QVERIFY(list1.count() == 1);
    QVERIFY(list2.count() == 1);
}

void TestStorageFile::testReadBlockData()
{
    char *data1 = new char[10];
    char *data2 = new char[10];
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int bloackData;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&bloackData), 4);
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&bloackData), 4);

    QVERIFY(m_StorageFile1->readBlockData(0, data1, 4) == 4);
    QVERIFY(m_StorageFile2->readBlockData(0, data2, 4) == 4);
}

void TestStorageFile::testAppendBlockData()
{
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int blockData;
    int dataNum1 = m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&blockData), 4);
    int dataNum2 = m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&blockData), 4);

    QVERIFY(dataNum1 == 4);
    QVERIFY(dataNum2 == 4);
}

void TestStorageFile::testModifyBlockData()
{
    m_StorageFile1->setReservedSize(100);
    m_StorageFile2->setReservedSize(100);
    int blockData = 10;
    m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&blockData), sizeof(int));
    m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&blockData), sizeof(int));

    int modifyData1 = 20;
    int modifyData2 = 30;
    m_StorageFile1->modifyBlockData(0, reinterpret_cast<char *>(&modifyData1), sizeof(int));
    m_StorageFile2->modifyBlockData(0, reinterpret_cast<char *>(&modifyData2), sizeof(int));
    QByteArray data1 = m_StorageFile1->getBlockData(0);
    QByteArray data2 = m_StorageFile2->getBlockData(0);

    QVERIFY(data1.at(0) == 20);
    QVERIFY(data2.at(0) == 30);
}

void TestStorageFile::testMapAdditionalData()
{
    m_StorageFile1->setReservedSize(256);
    m_StorageFile2->setReservedSize(256);

    QVERIFY(m_StorageFile1->mapAdditionalData() != NULL);
    QVERIFY(m_StorageFile2->mapAdditionalData() != NULL);
}

void TestStorageFile::testUnmapAdditionalData()
{
    m_StorageFile1->setReservedSize(256);
    m_StorageFile2->setReservedSize(256);
    uchar *address1 = m_StorageFile1->mapAdditionalData();
    uchar *address2 = m_StorageFile2->mapAdditionalData();

    QVERIFY(m_StorageFile1->unmapAdditionalData(address1));
    QVERIFY(m_StorageFile2->unmapAdditionalData(address2));
}

void TestStorageFile::testRemove()
{
    m_StorageFile1->remove(m_StorageFile1);
    m_StorageFile2->remove(m_StorageFile2);

    QVERIFY(m_StorageFile1->fileSize() == 0);
    QVERIFY(m_StorageFile2->fileSize() == 0);
}

void TestStorageFile::testRename()
{
    QString newName1 = m_File1->fileName();
    QString newName2 = m_File2->fileName();
    newName1.insert(5, "test");
    newName2.insert(5, "test");
    StorageFile *storageFile = new StorageFile();
    storageFile->reload(m_File1->fileName(), QIODevice::ReadOnly); // 只读文件才能rename .bak文件;

    QVERIFY(storageFile->rename(storageFile, newName1) == true);
    QVERIFY(m_StorageFile2->rename(m_StorageFile2, newName2) == false);
    QVERIFY(m_StorageFile1->rename(m_StorageFile1, newName1) == false);

    storageFile->remove(storageFile);
}

QTEST_MAIN(TestStorageFile)

#include "TestStorageFile.moc"
