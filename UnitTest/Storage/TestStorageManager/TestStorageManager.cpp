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

#define ADD_DATA_CHAR_POINTER       (1)
#define ADD_DATA_QBYTEARRAY         (2)

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
    : m_File(NULL), m_Backend(NULL)
{
}

TestStorageManager::~TestStorageManager()
{
}

void TestStorageManager::init()
{
    m_File = new QTemporaryFile();
    createTestFileOnDisk(m_File, ":/EventData.seq");
}

void TestStorageManager::cleanup()
{
    delete m_File;
    m_File = NULL;
}

Q_DECLARE_METATYPE(QIODevice::OpenModeFlag)
void TestStorageManager::testOpen_data()
{
    QTest::addColumn<QIODevice::OpenModeFlag>("openMode");
    QTest::addColumn<bool>("result");

    QTest::newRow("ReadOnly") << QIODevice::ReadOnly << false;
    QTest::newRow("ReadWrite") << QIODevice::ReadWrite << true;
}

void TestStorageManager::testOpen()
{
    QFETCH(QIODevice::OpenModeFlag, openMode);
    QFETCH(bool, result);

    StorageManager::open(m_File->fileName(), openMode);
    QFile storageFile(m_File->fileName() + ".seq");
    QFile backupFile(m_File->fileName() + ".seq.bak");

    bool exist = storageFile.exists();

    if (storageFile.exists())
    {
        storageFile.remove();
    }

    if (backupFile.exists())
    {
        backupFile.remove();
    }

    QCOMPARE(exist, result);
}

void TestStorageManager::testAddSaveData_data()
{
    QTest::addColumn<int>("addData");
    QTest::addColumn<quint32>("dataID");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<quint32>("extraData");
    QTest::addColumn<quint32>("idResult");
    QTest::addColumn<QByteArray>("dataResult");
    QTest::addColumn<quint32>("extraResult");

    char data1[6] = {0x13, 0x52, 0x0, 0x65, 0x14, static_cast<char>(0xff)};
    QTest::newRow("add string data")
            << ADD_DATA_CHAR_POINTER
            << static_cast<quint32>(0xff23)
            << QByteArray("add data")
            << static_cast<quint32>(0x12345)
            << static_cast<quint32>(0xff23)
            << QByteArray("add data")
            << static_cast<quint32>(0x12345);

    QTest::newRow("add integer data")
            << ADD_DATA_CHAR_POINTER
            << static_cast<quint32>(0xff23)
            << QByteArray(data1, 6)
            << static_cast<quint32>(0x12345)
            << static_cast<quint32>(0xff23)
            << QByteArray(data1, 6)
            << static_cast<quint32>(0x12345);

    QTest::newRow("save string data")
            << ADD_DATA_CHAR_POINTER
            << static_cast<quint32>(0xffffffff)
            << QByteArray("save data")
            << static_cast<quint32>(0xffffffff)
            << static_cast<quint32>(0xffffffff)
            << QByteArray("save data")
            << static_cast<quint32>(0xffffffff);

    QTest::newRow("save integer data")
            << ADD_DATA_CHAR_POINTER
            << static_cast<quint32>(0)
            << QByteArray(data1, 6)
            << static_cast<quint32>(0)
            << static_cast<quint32>(0)
            << QByteArray(data1, 6)
            << static_cast<quint32>(0);

    QTest::newRow("add string data")
            << ADD_DATA_QBYTEARRAY
            << static_cast<quint32>(0xff23)
            << QByteArray("add data")
            << static_cast<quint32>(0x12345)
            << static_cast<quint32>(0xff23)
            << QByteArray("add data")
            << static_cast<quint32>(0x12345);

    QTest::newRow("add integer data")
            << ADD_DATA_QBYTEARRAY
            << static_cast<quint32>(0xff23)
            << QByteArray(data1, 6)
            << static_cast<quint32>(0x12345)
            << static_cast<quint32>(0xff23)
            << QByteArray(data1, 6)
            << static_cast<quint32>(0x12345);

    QTest::newRow("save string data")
            << ADD_DATA_QBYTEARRAY
            << static_cast<quint32>(0x98765432)
            << QByteArray("save data")
            << static_cast<quint32>(0x12345678)
            << static_cast<quint32>(0x98765432)
            << QByteArray("save data")
            << static_cast<quint32>(0x12345678);

    QTest::newRow("save integer data")
            << ADD_DATA_QBYTEARRAY
            << static_cast<quint32>(0)
            << QByteArray(data1, 6)
            << static_cast<quint32>(0)
            << static_cast<quint32>(0)
            << QByteArray(data1, 6)
            << static_cast<quint32>(0);
}

void TestStorageManager::testAddSaveData()
{
    QFETCH(int, addData);
    QFETCH(quint32, dataID);
    QFETCH(QByteArray, data);
    QFETCH(quint32, extraData);
    QFETCH(quint32, idResult);
    QFETCH(QByteArray, dataResult);
    QFETCH(quint32, extraResult);

    m_Backend = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    StorageManager manager(m_Backend);

    if (addData == ADD_DATA_CHAR_POINTER)
    {
        manager.addData(dataID, data.data(), data.count(), extraData);
        manager.saveData();
        manager.saveData();
    }
    else if (addData == ADD_DATA_QBYTEARRAY)
    {
        manager.addData(dataID, data, extraData);
        manager.saveData();
        manager.saveData();
    }

    QByteArray readData = m_Backend->getBlockData(0);
    BlockEntry info;
    m_Backend->getBlockInfo(0, info);
    m_Backend->remove(m_Backend);

    QCOMPARE(readData, dataResult);
    QCOMPARE(info.type, idResult);
    QCOMPARE(info.extraData, extraResult);
    QCOMPARE(info.length, static_cast<quint32>(dataResult.count()));
}

void TestStorageManager::testDiscardData_data()
{
    QTest::addColumn<QByteArray>("data1");
    QTest::addColumn<QByteArray>("data2");
    QTest::addColumn<int>("discardNum");
    QTest::addColumn<QByteArray>("result");

    QTest::newRow("discard 0 second")
            << QByteArray("save data 11111")
            << QByteArray("save data 22222")
            << 0
            << QByteArray("save data 11111");

    QTest::newRow("discard 1 second")
            << QByteArray("save data 11111")
            << QByteArray("save data 22222")
            << 1
            << QByteArray("save data 22222");

    QTest::newRow("discard 2 second")
            << QByteArray("save data 11111")
            << QByteArray("save data 22222")
            << 2
            << QByteArray("");
}

void TestStorageManager::testDiscardData()
{
    QFETCH(QByteArray, data1);
    QFETCH(QByteArray, data2);
    QFETCH(int, discardNum);
    QFETCH(QByteArray, result);

    m_Backend = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    StorageManager manager(m_Backend);
    quint32 dataID = 0xff;
    QByteArray readData;

    manager.addData(dataID, data1);
    manager.saveData();
    manager.addData(dataID, data2);
    for (int i = 0; i < discardNum; i++)
    {
        manager.discardData();
    }
    manager.saveData();

    readData = m_Backend->getBlockData(0);
    m_Backend->remove(m_Backend);

    QCOMPARE(readData, result);
}

void TestStorageManager::testBackend()
{
    m_Backend = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    StorageManager manager(m_Backend);
    IStorageBackend *backend = manager.backend();
    m_Backend->remove(m_Backend);
    QCOMPARE(m_Backend, backend);
}

void TestStorageManager::testSize_data()
{
    QTest::addColumn<bool>("addData");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<quint32>("result");

    char inteData[5] = {0x12, 0x34, 0x0, 0x56, 0x78};
    QTest::newRow("empty file")
            << false
            << QByteArray()
            << static_cast<quint32>(24);

    QTest::newRow("add data")
            << true
            << QByteArray("add data size")
            << static_cast<quint32>(37);

    QTest::newRow("add empty data")
            << true
            << QByteArray("")
            << static_cast<quint32>(24);

    QTest::newRow("add integer data")
            << true
            << QByteArray(inteData, 5)
            << static_cast<quint32>(29);
}

void TestStorageManager::testSize()
{
    QFETCH(bool, addData);
    QFETCH(QByteArray, data);
    QFETCH(quint32, result);

    m_Backend = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    StorageManager manager(m_Backend);

    if (addData)
    {
        quint32 dataID = 0xff;
        manager.addData(dataID, data);
        manager.saveData();
        manager.saveData();
    }

    quint32 size = manager.size();
    m_Backend->remove(m_Backend);

    QCOMPARE(size, result);
}

void TestStorageManager::testExist_data()
{
    QTest::addColumn<bool>("validFileName");
    QTest::addColumn<bool>("result");

    QTest::newRow("invalid file name")
            << false
            << false;

    QTest::newRow("valid file name")
            << true
            << true;
}

void TestStorageManager::testExist()
{
    QFETCH(bool, validFileName);
    QFETCH(bool, result);

    if (validFileName)
    {
        m_Backend = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_Backend = new StorageFile("", QIODevice::ReadWrite);
    }
    StorageManager manager(m_Backend);
    bool exist = manager.exist(m_File->fileName());
    m_Backend->remove(m_Backend);
    QCOMPARE(exist, result);
}
