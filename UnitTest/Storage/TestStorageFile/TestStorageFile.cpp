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
    void testFileType_data();
    void testFileType();
    void testFileSize_data();
    void testFileSize();
    void testIsValid_data();
    void testIsValid();
    void testSetReservedSize_data();
    void testSetReservedSize();
    void testGetReservedSize_data();
    void testGetReservedSize();
    void testReload_data();
    void testReload();
    void testGetBlockNR_data();
    void testGetBlockNR();
    void testWriteAdditionalData_data();
    void testWriteAdditionalData();
    void testReadAdditionalData_data();
    void testReadAdditionalData();
    void testGetBlockDataLen_data();
    void testGetBlockDataLen();
    void testGetBlockData_data();
    void testGetBlockData();
    void testGetBlockType_data();
    void testGetBlockType();
    void testGetBlockInfo_data();
    void testGetBlockInfo();
    void testGetBlockEntryList_data();
    void testGetBlockEntryList();
    void testReadBlockData_data();
    void testReadBlockData();
    void testAppendBlockData_data();
    void testAppendBlockData();
//    void testModifyBlockData();
//    void testMapAdditionalData();
//    void testUnmapAdditionalData();
//    void testRemove();
//    void testRename();

private:
    StorageFile *m_StorageFile;
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
    : m_StorageFile(NULL), m_File(NULL)
{
}

void TestStorageFile::init()
{
    m_File = new QTemporaryFile();
    createTestFileOnDisk(m_File, ":/TrendData.seq");
}

void TestStorageFile::cleanup()
{
    delete m_File;
    m_File = NULL;
    m_StorageFile->remove(m_StorageFile);
    delete m_StorageFile;
    m_StorageFile = NULL;
}

void TestStorageFile::testFileType_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 1 << true;
    QTest::newRow("init2") << 2 << true;
}

void TestStorageFile::testFileType()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    QCOMPARE(m_StorageFile->fileType() == StorageFile::Type, result);
}

void TestStorageFile::testFileSize_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<quint32>("result");

    QTest::newRow("init1") << 1 << static_cast<quint32>(24);
    QTest::newRow("init2") << 2 << static_cast<quint32>(24);
}

void TestStorageFile::testFileSize()
{
    QFETCH(int, init);
    QFETCH(quint32, result);

    if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    QCOMPARE(m_StorageFile->fileSize(), result);
}

void TestStorageFile::testIsValid_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 1 << true;
    QTest::newRow("init2") << 2 << true;
}

void TestStorageFile::testIsValid()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    QVERIFY(m_StorageFile->isValid() == result);
}

void TestStorageFile::testSetReservedSize_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result1");
    QTest::addColumn<bool>("result2");

    QTest::newRow("init1") << 1 << true << false;
    QTest::newRow("init2") << 2 << true << false;
}

void TestStorageFile::testSetReservedSize()
{
    QFETCH(int, init);
    QFETCH(bool, result1);
    QFETCH(bool, result2);
    if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    QCOMPARE(m_StorageFile->setReservedSize(100), result1);
    int blockData = 0;
    m_StorageFile->appendBlockData(m_StorageFile->fileType() , reinterpret_cast<char *>(&blockData), 4);
    QCOMPARE(m_StorageFile->setReservedSize(100), result2);      // 当已存在blockdata时返回false
}

void TestStorageFile::testGetReservedSize_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 1 << true;
    QTest::newRow("init2") << 2 << true;
}

void TestStorageFile::testGetReservedSize()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }
    m_StorageFile->setReservedSize(100);

    QCOMPARE(m_StorageFile->getReservedSize() == 100, result);
}

void TestStorageFile::testReload_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("init0") << 0 << true;
    QTest::newRow("init1") << 1 << true;
}

void TestStorageFile::testReload()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);

    QCOMPARE(m_StorageFile->isValid(), result);
}

void TestStorageFile::testGetBlockNR_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<int>("blockNum");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << 10 << true;
//    QTest::newRow("init1") << 0 << 50 << true;
//    QTest::newRow("init2") << 1 << 10 << true;
//    QTest::newRow("init2") << 1 << 50 << true;
}

void TestStorageFile::testGetBlockNR()
{
    QFETCH(int, init);
    QFETCH(int, blockNum);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    int data = 0;
    for (int i = 0; i < blockNum; i++)
    {
        m_StorageFile->appendBlockData(m_StorageFile->fileType() , reinterpret_cast<char *>(&data), 4);
    }

    int blockNR = m_StorageFile->getBlockNR();
    QCOMPARE(blockNR == blockNum, result);
}

void TestStorageFile::testWriteAdditionalData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << true;
    QTest::newRow("init2") << 1 << true;
}

void TestStorageFile::testWriteAdditionalData()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    char data = 26;
    char readData = 0;
    m_StorageFile->setReservedSize(100);
    m_StorageFile->writeAdditionalData(&data, 1);
    m_StorageFile->readAdditionalData(&readData, 1);
    QCOMPARE(readData == data, result);
}

void TestStorageFile::testReadAdditionalData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << true;
    QTest::newRow("init2") << 1 << true;
}

void TestStorageFile::testReadAdditionalData()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    char data = 26;
    char readData = 0;
    m_StorageFile->setReservedSize(100);
    m_StorageFile->writeAdditionalData(&data, 1);
    m_StorageFile->readAdditionalData(&readData, 1);
    QCOMPARE(readData == data, result);
}

void TestStorageFile::testGetBlockDataLen_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<quint32>("len");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << static_cast<quint32>(4) << true;
    QTest::newRow("init1") << 0 << static_cast<quint32>(2000) << true;
    QTest::newRow("init2") << 1 << static_cast<quint32>(4) << true;
    QTest::newRow("init2") << 1 << static_cast<quint32>(2000) << true;
}

void TestStorageFile::testGetBlockDataLen()
{
    QFETCH(int, init);
    QFETCH(quint32, len);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->setReservedSize(100);
    int bloackData = 200;
    m_StorageFile->appendBlockData(m_StorageFile->fileType() , reinterpret_cast<char *>(&bloackData), len);

    QCOMPARE(m_StorageFile->getBlockDataLen(0) == len, result);
}

void TestStorageFile::testGetBlockData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<char>("blockData");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << static_cast<char>(4) << true;
    QTest::newRow("init1") << 0 << static_cast<char>(200) << true;
    QTest::newRow("init2") << 1 << static_cast<char>(4) << true;
    QTest::newRow("init2") << 1 << static_cast<char>(200) << true;
}

void TestStorageFile::testGetBlockData()
{
    QFETCH(int, init);
    QFETCH(char, blockData);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->setReservedSize(100);
    m_StorageFile->appendBlockData(m_StorageFile->fileType() , reinterpret_cast<char *>(&blockData), 1);

    QByteArray byteArray = m_StorageFile->getBlockData(0);
    QCOMPARE(byteArray.at(0) == blockData, result);
}

void TestStorageFile::testGetBlockType_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<char>("blockData");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << static_cast<char>(4) << true;
    QTest::newRow("init1") << 0 << static_cast<char>(200) << true;
    QTest::newRow("init2") << 1 << static_cast<char>(4) << true;
    QTest::newRow("init2") << 1 << static_cast<char>(200) << true;
}

void TestStorageFile::testGetBlockType()
{
    QFETCH(int, init);
    QFETCH(char, blockData);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->setReservedSize(100);
    m_StorageFile->appendBlockData(m_StorageFile->fileType() , reinterpret_cast<char *>(&blockData), 1);

    quint32 type = m_StorageFile->fileType();
    QCOMPARE(m_StorageFile->getBlockType(0) == type, result);
}

void TestStorageFile::testGetBlockInfo_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << true;
    QTest::newRow("init2") << 1 << true;
}

void TestStorageFile::testGetBlockInfo()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->setReservedSize(100);
    int blockData = 2000;
    m_StorageFile->appendBlockData(m_StorageFile->fileType() , reinterpret_cast<char *>(&blockData), sizeof(int));

    BlockEntry info;
    m_StorageFile->getBlockInfo(0, info);
    QCOMPARE(info.type == static_cast<quint32>(m_StorageFile->fileType()), result);
    QCOMPARE(info.length == sizeof(int), result);
}

void TestStorageFile::testGetBlockEntryList_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<int>("blockNum");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << 10 << true;
//    QTest::newRow("init2") << 1 << 300 << true;
}

void TestStorageFile::testGetBlockEntryList()
{
    QFETCH(int, init);
    QFETCH(int, blockNum);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->setReservedSize(100);
    int blockData = 66;
    for (int i = 0; i < blockNum; i++)
    {
        m_StorageFile->appendBlockData(m_StorageFile->fileType() , reinterpret_cast<char *>(&blockData), sizeof(int));
    }

    QList<BlockEntry> list;
    m_StorageFile->getBlockEntryList(list);

    QCOMPARE(list.count() == blockNum, result);
}

void TestStorageFile::testReadBlockData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<char>("blockData");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << static_cast<char>(10) << true;
    QTest::newRow("init2") << 1 << static_cast<char>(200) << true;
}

void TestStorageFile::testReadBlockData()
{
    QFETCH(int, init);
    QFETCH(char, blockData);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->setReservedSize(100);
    m_StorageFile->appendBlockData(m_StorageFile->fileType() , &blockData, sizeof(char));

    char readData = 0;
    m_StorageFile->readBlockData(0, &readData, sizeof(char));

    QCOMPARE(readData == blockData, result);
}

void TestStorageFile::testAppendBlockData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("init1") << 0 << true;
    QTest::newRow("init2") << 1 << true;
}

void TestStorageFile::testAppendBlockData()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->setReservedSize(100);
    int blockData = 0;
    int dataNum = m_StorageFile->appendBlockData(m_StorageFile->fileType() , reinterpret_cast<char *>(&blockData),
                  sizeof(int));

    QCOMPARE(dataNum == sizeof(int), result);
}

// void TestStorageFile::testModifyBlockData()
// {
//     m_StorageFile1->setReservedSize(100);
//     m_StorageFile2->setReservedSize(100);
//     int blockData = 10;
//     m_StorageFile1->appendBlockData(m_StorageFile1->fileType() , reinterpret_cast<char *>(&blockData), sizeof(int));
//     m_StorageFile2->appendBlockData(m_StorageFile2->fileType() , reinterpret_cast<char *>(&blockData), sizeof(int));

//     int modifyData1 = 20;
//     int modifyData2 = 30;
//     m_StorageFile1->modifyBlockData(0, reinterpret_cast<char *>(&modifyData1), sizeof(int));
//     m_StorageFile2->modifyBlockData(0, reinterpret_cast<char *>(&modifyData2), sizeof(int));
//     QByteArray data1 = m_StorageFile1->getBlockData(0);
//     QByteArray data2 = m_StorageFile2->getBlockData(0);

//     QVERIFY(data1.at(0) == 20);
//     QVERIFY(data2.at(0) == 30);
// }

// void TestStorageFile::testMapAdditionalData()
// {
//     m_StorageFile1->setReservedSize(256);
//     m_StorageFile2->setReservedSize(256);

//     QVERIFY(m_StorageFile1->mapAdditionalData() != NULL);
//     QVERIFY(m_StorageFile2->mapAdditionalData() != NULL);
// }

// void TestStorageFile::testUnmapAdditionalData()
// {
//     m_StorageFile1->setReservedSize(256);
//     m_StorageFile2->setReservedSize(256);
//     uchar *address1 = m_StorageFile1->mapAdditionalData();
//     uchar *address2 = m_StorageFile2->mapAdditionalData();

//     QVERIFY(m_StorageFile1->unmapAdditionalData(address1));
//     QVERIFY(m_StorageFile2->unmapAdditionalData(address2));
// }

// void TestStorageFile::testRemove()
// {
//     m_StorageFile1->remove(m_StorageFile1);
//     m_StorageFile2->remove(m_StorageFile2);

//     QVERIFY(m_StorageFile1->fileSize() == 0);
//     QVERIFY(m_StorageFile2->fileSize() == 0);
// }

// void TestStorageFile::testRename()
// {
//     QString newName1 = m_File1->fileName();
//     QString newName2 = m_File2->fileName();
//     newName1.insert(5, "test");
//     newName2.insert(5, "test");
//     StorageFile *storageFile = new StorageFile();
//     storageFile->reload(m_File1->fileName(), QIODevice::ReadOnly); // 只读文件才能rename .bak文件;

//     QVERIFY(storageFile->rename(storageFile, newName1) == true);
//     QVERIFY(m_StorageFile2->rename(m_StorageFile2, newName2) == false);
//     QVERIFY(m_StorageFile1->rename(m_StorageFile1, newName1) == false);

//     storageFile->remove(storageFile);
// }

QTEST_MAIN(TestStorageFile)

#include "TestStorageFile.moc"
