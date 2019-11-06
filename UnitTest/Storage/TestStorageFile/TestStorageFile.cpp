/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/2/28
 **/

#include "TestStorageFile.h"

#define DEFAULT_CONSTRUCTOR                 (0)
#define DEFAULT_CONSTRUCTOR_RELOAD          (1)
#define PARAM_CONSTRUCTOR                   (2)

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
    QTest::addColumn<int>("result");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << static_cast<int>(StorageFile::Type);

    QTest::newRow("default constructor reload")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << static_cast<int>(StorageFile::Type);

    QTest::newRow("param constructor")
            << PARAM_CONSTRUCTOR
            << static_cast<int>(StorageFile::Type);
}

void TestStorageFile::testFileType()
{
    QFETCH(int, init);
    QFETCH(int, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    QCOMPARE(m_StorageFile->fileType(), result);
}

void TestStorageFile::testFileSize_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("addData");
    QTest::addColumn<QString>("data");
    QTest::addColumn<quint32>("result");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << false
            << ""
            << static_cast<quint32>(0);

    QTest::newRow("default constructor reload")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << false
            << "test"
            << static_cast<quint32>(24);

    QTest::newRow("default constructor reload, add data")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << true
            << "test this "
            << static_cast<quint32>(34);

    QTest::newRow("param constructor, add data")
            << PARAM_CONSTRUCTOR
            << true
            << ""
            << static_cast<quint32>(24);

    QTest::newRow("param constructor, add data")
            << PARAM_CONSTRUCTOR
            << true
            << "test this funtion"
            << static_cast<quint32>(41);
}

void TestStorageFile::testFileSize()
{
    QFETCH(int, init);
    QFETCH(bool, addData);
    QFETCH(QString, data);
    QFETCH(quint32, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    if (addData)
    {
        m_StorageFile->appendBlockData(0x02, data.toLatin1().data(), data.count());
    }

    QCOMPARE(m_StorageFile->fileSize(), result);
}

void TestStorageFile::testIsValid_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << false;

    QTest::newRow("default constructor reload")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << true;

    QTest::newRow("param constructor")
            << PARAM_CONSTRUCTOR
            << true;
}

void TestStorageFile::testIsValid()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    QCOMPARE(m_StorageFile->isValid(), result);
}

void TestStorageFile::testReservedSize_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("addData");
    QTest::addColumn<quint32>("reservedSize");
    QTest::addColumn<quint32>("result");

    QTest::newRow("default constructor, 200 size")
            << DEFAULT_CONSTRUCTOR
            << false
            << static_cast<quint32>(200)
            << static_cast<quint32>(0);

    QTest::newRow("param constructor, 200 size")
            << PARAM_CONSTRUCTOR
            << false
            << static_cast<quint32>(200)
            << static_cast<quint32>(200);

    QTest::newRow("param constructor, 100000 size")
            << PARAM_CONSTRUCTOR
            << false
            << static_cast<quint32>(100000)
            << static_cast<quint32>(100000);

    QTest::newRow("default constructor reload, add data, 200 size")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << true
            << static_cast<quint32>(100)
            << static_cast<quint32>(0);

    QTest::newRow("param constructor, add data, 200 size")
            << PARAM_CONSTRUCTOR
            << true
            << static_cast<quint32>(200)
            << static_cast<quint32>(0);
}

void TestStorageFile::testReservedSize()
{
    QFETCH(int, init);
    QFETCH(bool, addData);
    QFETCH(quint32, reservedSize);
    QFETCH(quint32, result);
    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    if (addData)
    {
        QString data = "add data";
        m_StorageFile->appendBlockData(0x66, data.toLatin1().data(), data.count());
    }
    m_StorageFile->setReservedSize(reservedSize);

    QCOMPARE(m_StorageFile->getReservedSize(), result);
}
Q_DECLARE_METATYPE(QIODevice::OpenModeFlag)
void TestStorageFile::testReload_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<QIODevice::OpenModeFlag>("openMode");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor, read only")
            << DEFAULT_CONSTRUCTOR
            << QIODevice::ReadOnly
            << false;

    QTest::newRow("default constructor, read write")
            << DEFAULT_CONSTRUCTOR
            << QIODevice::ReadWrite
            << true;

    QTest::newRow("param constructor, read only")
            << PARAM_CONSTRUCTOR
            << QIODevice::ReadOnly
            << true;
}

void TestStorageFile::testReload()
{
    QFETCH(int, init);
    QFETCH(QIODevice::OpenModeFlag, openMode);
    QFETCH(bool, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == PARAM_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->reload(m_File->fileName(), openMode);

    QCOMPARE(m_StorageFile->isValid(), result);
}

void TestStorageFile::testGetBlockNR_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<int>("blockNum");
    QTest::addColumn<quint32>("result");

    QTest::newRow("default constructor, 10 blocknum")
            << DEFAULT_CONSTRUCTOR
            << 10
            << static_cast<quint32>(0);

    QTest::newRow("default constructor reload, 30 blocknum")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << 30
            << static_cast<quint32>(30);

    QTest::newRow("param constructor, 5 blocknum")
            << PARAM_CONSTRUCTOR
            << 5
            << static_cast<quint32>(5);

    QTest::newRow("param constructor, 50 blocknum")
            << PARAM_CONSTRUCTOR
            << 50
            << static_cast<quint32>(50);
}

void TestStorageFile::testGetBlockNR()
{
    QFETCH(int, init);
    QFETCH(int, blockNum);
    QFETCH(quint32, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    QString data = "test getBlockNR()";
    for (int i = 0; i < blockNum; i++)
    {
        m_StorageFile->appendBlockData(m_StorageFile->fileType() , data.toLatin1().data(), data.count());
    }

    QCOMPARE(m_StorageFile->getBlockNR(), result);
}

void TestStorageFile::testAdditionalData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<int>("reservedSize");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("result");

    char data1[5] = {static_cast<char>(0xff), static_cast<char>(0xff), static_cast<char>(0xff),
                     static_cast<char>(0xff), static_cast<char>(0xff)};
    char data2[4] = {0x12, 0, 0x56, 0x78};
    char data3[5] = {0, 0, 0, 0, 0};
    QTest::newRow("default constructor, 100 size, string")
            << DEFAULT_CONSTRUCTOR
            << 100
            << QByteArray("test ")
            << QByteArray(data3, 5);

    QTest::newRow("default constructor reload, 100 size, string")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << 100
            << QByteArray("test additional")
            << QByteArray("test additional");

    QTest::newRow("param constructor, 10 size, string")
            << PARAM_CONSTRUCTOR
            << 10
            << QByteArray("test additional data")
            << QByteArray("test addit");

    QTest::newRow("default constructor, 100 size, integer")
            << DEFAULT_CONSTRUCTOR
            << 100
            << QByteArray(data1, 5)
            << QByteArray(data3, 5);

    QTest::newRow("default constructor reload, 100 size, integer(0xff)")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << 100
            << QByteArray(data1, 5)
            << QByteArray(data1, 5);

    QTest::newRow("default constructor reload, 100 size, integer")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << 100
            << QByteArray(data2, 4)
            << QByteArray(data2, 4);

    QTest::newRow("param constructor, 2 size, integer")
            << PARAM_CONSTRUCTOR
            << 2
            << QByteArray(data2, 4)
            << QByteArray(data2, 2);
}

void TestStorageFile::testAdditionalData()
{
    QFETCH(int, init);
    QFETCH(int, reservedSize);
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    char readData[100] = {0};
    m_StorageFile->setReservedSize(reservedSize);
    m_StorageFile->writeAdditionalData(data.data(), data.count());
    m_StorageFile->readAdditionalData(readData, data.count());
    int readLen = (data.count() < reservedSize) ? data.count() : reservedSize;
    QByteArray readByteArray(readData, readLen);
    QCOMPARE(readByteArray, result);
}

void TestStorageFile::testGetBlockDataLen_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<quint32>("result");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << QByteArray("test")
            << static_cast<quint32>(0);

    QTest::newRow("default constructor reload, frist type len")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << QByteArray("test get blockdata")
            << static_cast<quint32>(18);

    QTest::newRow("param constructor,  second type len")
            << PARAM_CONSTRUCTOR
            << QByteArray("test get blockdata len")
            << static_cast<quint32>(22);

    QTest::newRow("param constructor,  0 len")
            << PARAM_CONSTRUCTOR
            << QByteArray("")
            << static_cast<quint32>(0);
}

void TestStorageFile::testGetBlockDataLen()
{
    QFETCH(int, init);
    QFETCH(QByteArray, data);
    QFETCH(quint32, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->appendBlockData(m_StorageFile->fileType() , data.data(), data.count());

    QCOMPARE(m_StorageFile->getBlockDataLen(0), result);
}

void TestStorageFile::testBlockData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("result");

    char data1[5] = {static_cast<char>(0xff), static_cast<char>(0xff), static_cast<char>(0xff),
                     static_cast<char>(0xff), static_cast<char>(0xff)};
    char data2[4] = {0x12, 0x0, 0x56, 0x78};
    char data3[5] = {0, 0, 0, 0, 0};
    QTest::newRow("default constructor, string")
            << DEFAULT_CONSTRUCTOR
            << QByteArray("test ")
            << QByteArray(data3, 5);

    QTest::newRow("default constructor reload, string")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << QByteArray("test additional")
            << QByteArray("test additional");

    QTest::newRow("param constructor,  string")
            << PARAM_CONSTRUCTOR
            << QByteArray("test additional data")
            << QByteArray("test additional data");

    QTest::newRow("default constructor, integer")
            << DEFAULT_CONSTRUCTOR
            << QByteArray(data1, 5)
            << QByteArray(data3, 5);

    QTest::newRow("default constructor reload, integer(0xff)")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << QByteArray(data1, 5)
            << QByteArray(data1, 5);

    QTest::newRow("default constructor reload, integer")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << QByteArray(data2, 4)
            << QByteArray(data2, 4);

    QTest::newRow("param constructor, integer")
            << PARAM_CONSTRUCTOR
            << QByteArray(data2, 4)
            << QByteArray(data2, 4);
}

void TestStorageFile::testBlockData()
{
    QFETCH(int, init);
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->appendBlockData(0x66, data.data(), data.count());

    char readData[100] = {0};
    m_StorageFile->readBlockData(0, readData, data.count());
    QByteArray readByteArray(readData, data.count());
    QCOMPARE(readByteArray, result);
}

void TestStorageFile::testGetBlockType_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<quint32>("type");
    QTest::addColumn<quint32>("result");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << static_cast<quint32>(0xff)
            << static_cast<quint32>(0);

    QTest::newRow("default constructor reload")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << static_cast<quint32>(0)
            << static_cast<quint32>(0);

    QTest::newRow("param constructor, 0x66")
            << PARAM_CONSTRUCTOR
            << static_cast<quint32>(0x66)
            << static_cast<quint32>(0x66);

    QTest::newRow("param constructor, 0xffffffff")
            << PARAM_CONSTRUCTOR
            << static_cast<quint32>(0xffffffff)
            << static_cast<quint32>(0xffffffff);
}

void TestStorageFile::testGetBlockType()
{
    QFETCH(int, init);
    QFETCH(quint32, type);
    QFETCH(quint32, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    QString data = "test get block type";
    m_StorageFile->appendBlockData(type, data.toLatin1().data(), data.count());

    QCOMPARE(m_StorageFile->getBlockType(0), result);
}

void TestStorageFile::testGetBlockInfo_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<quint32>("type");
    QTest::addColumn<quint32>("typeResult");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<quint32>("dataLen");
    QTest::addColumn<quint32>("extraData");
    QTest::addColumn<quint32>("extraResult");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << static_cast<quint32>(0x32)
            << static_cast<quint32>(0)
            << QByteArray("test")
            << static_cast<quint32>(0)
            << static_cast<quint32>(48)
            << static_cast<quint32>(0);

    QTest::newRow("default constructor reload")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << static_cast<quint32>(0x32)
            << static_cast<quint32>(0x32)
            << QByteArray("test get block")
            << static_cast<quint32>(14)
            << static_cast<quint32>(0xff3252)
            << static_cast<quint32>(0xff3252);

    QTest::newRow("param constructor")
            << PARAM_CONSTRUCTOR
            << static_cast<quint32>(0xfff32194)
            << static_cast<quint32>(0xfff32194)
            << QByteArray("test get block info")
            << static_cast<quint32>(19)
            << static_cast<quint32>(0x39585013)
            << static_cast<quint32>(0x39585013);

    QTest::newRow("param constructor, empty data block")
            << PARAM_CONSTRUCTOR
            << static_cast<quint32>(0x3f)
            << static_cast<quint32>(0)
            << QByteArray()
            << static_cast<quint32>(0)
            << static_cast<quint32>(0)
            << static_cast<quint32>(0);
}

void TestStorageFile::testGetBlockInfo()
{
    QFETCH(int, init);
    QFETCH(quint32, type);
    QFETCH(quint32, typeResult);
    QFETCH(QByteArray, data);
    QFETCH(quint32, dataLen);
    QFETCH(quint32, extraData);
    QFETCH(quint32, extraResult);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->appendBlockData(type, data.data(), data.count(), extraData);

    BlockEntry info = m_StorageFile->getBlockInfo(0);
    QCOMPARE(info.type, typeResult);
    QCOMPARE(info.length, dataLen);
    QCOMPARE(info.extraData, extraResult);
}

void TestStorageFile::testGetBlockEntryList_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<quint32>("type");
    QTest::addColumn<quint32>("typeResult");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<quint32>("dataLen");
    QTest::addColumn<quint32>("extraData");
    QTest::addColumn<quint32>("extraResult");
    QTest::addColumn<int>("blockNum");
    QTest::addColumn<int>("result");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << static_cast<quint32>(0x32)
            << static_cast<quint32>(0)
            << QByteArray("test")
            << static_cast<quint32>(0)
            << static_cast<quint32>(48)
            << static_cast<quint32>(0)
            << 10
            << 0;

    QTest::newRow("default constructor reload")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << static_cast<quint32>(0x42)
            << static_cast<quint32>(0x42)
            << QByteArray("test get block")
            << static_cast<quint32>(14)
            << static_cast<quint32>(36)
            << static_cast<quint32>(36)
            << 20
            << 20;

    QTest::newRow("param constructor")
            << PARAM_CONSTRUCTOR
            << static_cast<quint32>(0x52)
            << static_cast<quint32>(0x52)
            << QByteArray("test get block info")
            << static_cast<quint32>(19)
            << static_cast<quint32>(95)
            << static_cast<quint32>(95)
            << 5
            << 5;

    QTest::newRow("param constructor, empty data block")
            << PARAM_CONSTRUCTOR
            << static_cast<quint32>(0x52)
            << static_cast<quint32>(0)
            << QByteArray(0)
            << static_cast<quint32>(19)
            << static_cast<quint32>(95)
            << static_cast<quint32>(0)
            << 15
            << 0;
}

void TestStorageFile::testGetBlockEntryList()
{
    QFETCH(int, init);
    QFETCH(quint32, type);
    QFETCH(quint32, typeResult);
    QFETCH(QByteArray, data);
    QFETCH(quint32, dataLen);
    QFETCH(quint32, extraData);
    QFETCH(quint32, extraResult);
    QFETCH(int, blockNum);
    QFETCH(int, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    for (int i = 0; i < blockNum; i++)
    {
        m_StorageFile->appendBlockData(type, data.data(), data.count(), extraData);
    }

    QVector<BlockEntry> list;
    list = m_StorageFile->getBlockEntryList();

    QCOMPARE(list.count(), result);
    for (int i = 0; i < list.count(); i++)
    {
        BlockEntry info = list.at(i);
        QCOMPARE(info.type, typeResult);
        QCOMPARE(info.length, dataLen);
        QCOMPARE(info.extraData, extraResult);
    }
}

void TestStorageFile::testModifyBlockData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<QByteArray>("modifyData");
    QTest::addColumn<QByteArray>("result");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << QByteArray("test")
            << QByteArray();

    QTest::newRow("default constructor reload")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << QByteArray("test_ modi")
            << QByteArray("test_ modi");

    QTest::newRow("param constructor")
            << PARAM_CONSTRUCTOR
            << QByteArray("modigf tes")
            << QByteArray("modigf tes");

    QTest::newRow("default constructor reload, modify data longer")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << QByteArray("testmodifydata")
            << QByteArray("testmodify");

    QTest::newRow("param constructor, modify data shorter")
            << PARAM_CONSTRUCTOR
            << QByteArray("test")
            << QByteArray("test");
}

void TestStorageFile::testModifyBlockData()
{
    QFETCH(int, init);
    QFETCH(QByteArray, modifyData);
    QFETCH(QByteArray, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }
    QByteArray blockData("block data");
    m_StorageFile->appendBlockData(m_StorageFile->fileType() , blockData.data(), blockData.count());
    m_StorageFile->modifyBlockData(0, modifyData.data(), blockData.count());

    char readData[100] = {0};
    m_StorageFile->readBlockData(0, readData, blockData.count());
    QByteArray readDataStr(readData);

    QCOMPARE(readDataStr, result);
}

void TestStorageFile::testRemove_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << false;

    QTest::newRow("default constructor reload")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << false;

    QTest::newRow("param constructor")
            << PARAM_CONSTRUCTOR
            << false;
}

void TestStorageFile::testRemove()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    QFile storageFile(m_File->fileName() + ".seq");
    QFile backupFile(m_File->fileName() + ".seq.bak");
    m_StorageFile->remove(m_StorageFile);

    QCOMPARE(storageFile.exists(), result);
    QCOMPARE(backupFile.exists(), result);
}

void TestStorageFile::testRename_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor")
            << DEFAULT_CONSTRUCTOR
            << false;

    QTest::newRow("default constructor reload")
            << DEFAULT_CONSTRUCTOR_RELOAD
            << true;

    QTest::newRow("param constructor")
            << PARAM_CONSTRUCTOR
            << true;
}

void TestStorageFile::testRename()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == DEFAULT_CONSTRUCTOR)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == DEFAULT_CONSTRUCTOR_RELOAD)
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
