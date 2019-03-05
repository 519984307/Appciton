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
#include <QMetaType>
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
    void testModifyBlockData_data();
    void testModifyBlockData();
    void testMapAdditionalData_data();
    void testMapAdditionalData();
    void testUnmapAdditionalData_data();
    void testUnmapAdditionalData();
    void testRemove_data();
    void testRemove();
    void testRename_data();
    void testRename();

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

    QTest::newRow("test1") << 1 << true;
    QTest::newRow("test2") << 2 << true;
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

    QTest::newRow("test1") << 1 << static_cast<quint32>(24);
    QTest::newRow("test2") << 2 << static_cast<quint32>(24);
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

    QTest::newRow("test1") << 1 << true;
    QTest::newRow("test2") << 2 << true;
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

    QTest::newRow("test1") << 1 << true << false;
    QTest::newRow("test2") << 2 << true << false;
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

    QTest::newRow("test1") << 1 << true;
    QTest::newRow("test2") << 2 << true;
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

    QTest::newRow("test0") << 0 << true;
    QTest::newRow("test1") << 1 << true;
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

    QTest::newRow("test1") << 0 << 10 << true;
    QTest::newRow("test2") << 0 << 50 << true;
    QTest::newRow("test3") << 1 << 10 << true;
    QTest::newRow("test4") << 1 << 50 << true;
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

    QTest::newRow("test1") << 0 << true;
    QTest::newRow("test2") << 1 << true;
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

    QTest::newRow("test1") << 0 << true;
    QTest::newRow("test2") << 1 << true;
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

    QTest::newRow("test1") << 0 << static_cast<quint32>(4) << true;
    QTest::newRow("test2") << 0 << static_cast<quint32>(2000) << true;
    QTest::newRow("test3") << 1 << static_cast<quint32>(4) << true;
    QTest::newRow("test4") << 1 << static_cast<quint32>(2000) << true;
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

    QTest::newRow("test1") << 0 << static_cast<char>(4) << true;
    QTest::newRow("test2") << 0 << static_cast<char>(200) << true;
    QTest::newRow("test3") << 1 << static_cast<char>(4) << true;
    QTest::newRow("test4") << 1 << static_cast<char>(200) << true;
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

    QTest::newRow("test1") << 0 << static_cast<char>(4) << true;
    QTest::newRow("test2") << 0 << static_cast<char>(200) << true;
    QTest::newRow("test3") << 1 << static_cast<char>(4) << true;
    QTest::newRow("test4") << 1 << static_cast<char>(200) << true;
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

    QTest::newRow("test1") << 0 << true;
    QTest::newRow("test2") << 1 << true;
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

    QTest::newRow("test1") << 0 << 10 << true;
    QTest::newRow("test2") << 1 << 300 << true;
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

    QTest::newRow("test1") << 0 << static_cast<char>(10) << true;
    QTest::newRow("test2") << 1 << static_cast<char>(200) << true;
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
    QTest::addColumn<int>("blockNum");
    QTest::addColumn<bool>("result");

    QTest::newRow("test1") << 0 << 10 << true;
    QTest::newRow("test2") << 0 << 50 << true;
    QTest::newRow("test3") << 1 << 10 << true;
    QTest::newRow("test4") << 1 << 50 << true;
}

void TestStorageFile::testAppendBlockData()
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

void TestStorageFile::testModifyBlockData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<int>("modifyData");
    QTest::addColumn<bool>("result");

    QTest::newRow("test1") << 0 << 20 << true;
    QTest::newRow("test2") << 0 << 300 << true;
    QTest::newRow("test3") << 1 << 40 << true;
    QTest::newRow("test4") << 1 << 500 << true;
}

static int bytesToInt(QByteArray bytes) {
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

void TestStorageFile::testModifyBlockData()
{
    QFETCH(int, init);
    QFETCH(int, modifyData);
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
    int blockData = 10;
    m_StorageFile->appendBlockData(m_StorageFile->fileType() , reinterpret_cast<char *>(&blockData), sizeof(int));

    m_StorageFile->modifyBlockData(0, reinterpret_cast<char *>(&modifyData), sizeof(int));
    QByteArray byteArray = m_StorageFile->getBlockData(0);
    int data = bytesToInt(byteArray);

    QCOMPARE(data == modifyData, result);
}

void TestStorageFile::testMapAdditionalData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("reserved");
    QTest::addColumn<bool>("result");

    QTest::newRow("test1") << 0 << false << false;
    QTest::newRow("test2") << 0 << true << true;
    QTest::newRow("test3") << 1 << false << false;
    QTest::newRow("test4") << 1 << true << true;
}

void TestStorageFile::testMapAdditionalData()
{
    QFETCH(int, init);
    QFETCH(bool, reserved);
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

    if (reserved)
    {
        m_StorageFile->setReservedSize(256);
    }

    QCOMPARE(m_StorageFile->mapAdditionalData() != NULL, result);
}

void TestStorageFile::testUnmapAdditionalData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("test1") << 0 << true;
    QTest::newRow("test2") << 1 << true;
}

void TestStorageFile::testUnmapAdditionalData()
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

    m_StorageFile->setReservedSize(256);
    uchar *address = m_StorageFile->mapAdditionalData();

    QCOMPARE(m_StorageFile->unmapAdditionalData(address), result);
}

void TestStorageFile::testRemove_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("test1") << 0 << true;
    QTest::newRow("test2") << 1 << true;
}

void TestStorageFile::testRemove()
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

    m_StorageFile->remove(m_StorageFile);

    QCOMPARE(m_StorageFile->fileSize() == 0, result);
}

Q_DECLARE_METATYPE(QIODevice::OpenModeFlag)
void TestStorageFile::testRename_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("test1") << 0 << true;
    QTest::newRow("test2") << 1 << true;
}

void TestStorageFile::testRename()
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

    QString newName = m_File->fileName();
    newName.insert(5, "test");

    QCOMPARE(m_StorageFile->rename(m_StorageFile, newName), result);
}

QTEST_MAIN(TestStorageFile)

#include "TestStorageFile.moc"
