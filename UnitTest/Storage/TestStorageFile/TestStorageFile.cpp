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

    QTest::newRow("default constructor") << 0 << true;
    QTest::newRow("default constructor reload") << 1 << true;
    QTest::newRow("param constructor") << 2 << true;
}

void TestStorageFile::testFileType()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
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
    QTest::addColumn<bool>("addData");
    QTest::addColumn<QString>("data");
    QTest::addColumn<quint32>("result");

    QTest::newRow("default constructor") << 0 << false << "" << static_cast<quint32>(0);
    QTest::newRow("param constructor add data") << 2 << true << "test this funtion" << static_cast<quint32>(41);
}

void TestStorageFile::testFileSize()
{
    QFETCH(int, init);
    QFETCH(bool, addData);
    QFETCH(QString, data);
    QFETCH(quint32, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
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

    QTest::newRow("default constructor") << 0 << false;
    QTest::newRow("default constructor reload") << 1 << true;
    QTest::newRow("param constructor") << 2 << true;
}

void TestStorageFile::testIsValid()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
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

void TestStorageFile::testReservedSize_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("addData");
    QTest::addColumn<quint32>("reservedSize");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor, 200 size") << 0 << false << static_cast<quint32>(200) << false;
    QTest::newRow("param constructor, 200 size") << 2 << false << static_cast<quint32>(200) << true;
    QTest::newRow("param constructor, 100000 size") << 2 << false << static_cast<quint32>(100000) << true;
    QTest::newRow("param constructor, add data, 200 size") << 2 << true << static_cast<quint32>(200) << false;
}

void TestStorageFile::testReservedSize()
{
    QFETCH(int, init);
    QFETCH(bool, addData);
    QFETCH(quint32, reservedSize);
    QFETCH(bool, result);
    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
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

    QCOMPARE(m_StorageFile->getReservedSize() == reservedSize, result);
}
Q_DECLARE_METATYPE(QIODevice::OpenModeFlag)
void TestStorageFile::testReload_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<QIODevice::OpenModeFlag>("openMode");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor, read only") << 0 << QIODevice::ReadOnly << false;
    QTest::newRow("default constructor, read write") << 0 << QIODevice::ReadWrite << true;
    QTest::newRow("param constructor, read only") << 2 <<  QIODevice::ReadOnly << true;
}

void TestStorageFile::testReload()
{
    QFETCH(int, init);
    QFETCH(QIODevice::OpenModeFlag, openMode);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 2)
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
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor, 10 blocknum") << 0 << 10 << false;
    QTest::newRow("default constructor reload, 10 blocknum") << 1 << 50 << true;
    QTest::newRow("param constructor, 10 blocknum") << 2 << 10 << true;
    QTest::newRow("param constructor, 50 blocknum") << 2 << 50 << true;
}

void TestStorageFile::testGetBlockNR()
{
    QFETCH(int, init);
    QFETCH(int, blockNum);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
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

    int blockNR = m_StorageFile->getBlockNR();
    QCOMPARE(blockNR == blockNum, result);
}

void TestStorageFile::testAdditionalData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<int>("reservedSize");
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor, 100 size") << 0 << 100 << "test read write data" << false;
    QTest::newRow("param constructor, 100 size") << 2 << 100 << "test read write data" << true;
    QTest::newRow("param constructor, 10 size") << 2 << 10 << "test read write data" << false;
}

void TestStorageFile::testAdditionalData()
{
    QFETCH(int, init);
    QFETCH(QString, data);
    QFETCH(int, reservedSize);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    char *readData = new char[100];
    m_StorageFile->setReservedSize(reservedSize);
    m_StorageFile->writeAdditionalData(data.toLatin1().data(), data.count());
    m_StorageFile->readAdditionalData(readData, data.count());
    QString readDataStr = QString(QLatin1String(readData));
    QCOMPARE(readDataStr == data, result);
}

void TestStorageFile::testGetBlockDataLen_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor") << 0 << "test" << false;
    QTest::newRow("default constructor reload, frist type len") << 1 << "test get blockdata" << true;
    QTest::newRow("param constructor,  second type len") << 2  << "test get blockdata len" << true;
}

void TestStorageFile::testGetBlockDataLen()
{
    QFETCH(int, init);
    QFETCH(QString, data);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->appendBlockData(m_StorageFile->fileType() , data.toLatin1().data(), data.count());

    int len = m_StorageFile->getBlockDataLen(0);
    QCOMPARE(len == data.count(), result);
}

void TestStorageFile::testBlockData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor") << 0 << "test read" << false;
    QTest::newRow("default constructor reload") << 1 << "test read write" << true;
    QTest::newRow("param constructor") << 2 << "test read write data" << true;
}

void TestStorageFile::testBlockData()
{
    QFETCH(int, init);
    QFETCH(QString, data);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->appendBlockData(0x66, data.toLatin1().data(), data.count());

    char readData[100] = {0};
    m_StorageFile->readBlockData(0, readData, data.count());
    QString readDataStr = QString(QLatin1String(readData));
    QCOMPARE(readDataStr == data, result);
}

void TestStorageFile::testGetBlockType_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<quint32>("type");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor") << 0 << static_cast<quint32>(0xff) << false;
    QTest::newRow("default constructor reload") << 1 << static_cast<quint32>(0) << true;
    QTest::newRow("param constructor") << 2 << static_cast<quint32>(0x66) << true;
}

void TestStorageFile::testGetBlockType()
{
    QFETCH(int, init);
    QFETCH(quint32, type);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
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

    QCOMPARE(m_StorageFile->getBlockType(0) == type, result);
}

void TestStorageFile::testGetBlockInfo_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<quint32>("type");
    QTest::addColumn<QString>("data");
    QTest::addColumn<quint32>("extraData");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor") << 0 << static_cast<quint32>(0x32) << "test" << static_cast<quint32>(48) << false;
    QTest::newRow("default constructor reload") << 1 << static_cast<quint32>(0x42) << "test get block" << static_cast<quint32>(36) << true;
    QTest::newRow("param constructor") << 2 << static_cast<quint32>(0x52) << "test get block info" << static_cast<quint32>(95) << true;
}

void TestStorageFile::testGetBlockInfo()
{
    QFETCH(int, init);
    QFETCH(quint32, type);
    QFETCH(QString, data);
    QFETCH(quint32, extraData);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->appendBlockData(type, data.toLatin1().data(), data.count(), extraData);

    BlockEntry info;
    m_StorageFile->getBlockInfo(0, info);
    QCOMPARE(info.type == type, result);
    QCOMPARE(info.length == static_cast<quint32>(data.count()), result);
    QCOMPARE(info.extraData == extraData, result);
}

void TestStorageFile::testGetBlockEntryList_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<quint32>("type");
    QTest::addColumn<QString>("data");
    QTest::addColumn<quint32>("extraData");
    QTest::addColumn<int>("blockNum");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor") << 0 << static_cast<quint32>(0x32) << "test" << static_cast<quint32>(48) << 10 << false;
    QTest::newRow("default constructor reload") << 1 << static_cast<quint32>(0x42) << "test get block" << static_cast<quint32>(36) << 20 << true;
    QTest::newRow("param constructor") << 2 << static_cast<quint32>(0x52) << "test get block info" << static_cast<quint32>(95) << 5 << true;
}

void TestStorageFile::testGetBlockEntryList()
{
    QFETCH(int, init);
    QFETCH(quint32, type);
    QFETCH(QString, data);
    QFETCH(quint32, extraData);
    QFETCH(int, blockNum);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
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
        m_StorageFile->appendBlockData(type, data.toLatin1().data(), data.count(), extraData);
    }

    QList<BlockEntry> list;
    m_StorageFile->getBlockEntryList(list);

    QCOMPARE(list.count() == blockNum, result);
    for (int i = 0; i < list.count(); i++)
    {
        BlockEntry info = list.at(i);
        QCOMPARE(info.type == type, result);
        QCOMPARE(info.length == static_cast<quint32>(data.count()), result);
        QCOMPARE(info.extraData == extraData, result);
    }
}

void TestStorageFile::testModifyBlockData_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<QString>("modifyData");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor") << 0 << "test" << false;
    QTest::newRow("default constructor reload") << 1 << "test_ modi" << true;
    QTest::newRow("param constructor") << 2 << "modigf tes" << true;
}

void TestStorageFile::testModifyBlockData()
{
    QFETCH(int, init);
    QFETCH(QString, modifyData);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }
    QString blockData = "block data";
    m_StorageFile->appendBlockData(m_StorageFile->fileType() , blockData.toLatin1().data(), blockData.count());
    m_StorageFile->modifyBlockData(0, modifyData.toLatin1().data(), blockData.count());

    char readData[100] = {0};
    m_StorageFile->readBlockData(0, readData, blockData.count());
    QString readDataStr = QString(QLatin1String(readData));

    QCOMPARE(readDataStr == modifyData, result);
}

void TestStorageFile::testRemove_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor") << 0 << true;
    QTest::newRow("default constructor reload") << 1 << true;
    QTest::newRow("param constructor") << 2 << true;
}

void TestStorageFile::testRemove()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
    {
        m_StorageFile = new StorageFile();
        m_StorageFile->reload(m_File->fileName(), QIODevice::ReadWrite);
    }
    else
    {
        m_StorageFile = new StorageFile(m_File->fileName(), QIODevice::ReadWrite);
    }

    m_StorageFile->remove(m_StorageFile);

    QCOMPARE(!m_StorageFile->storageFileExist(), result);
    QCOMPARE(!m_StorageFile->backupFileExist(), result);
}

void TestStorageFile::testRename_data()
{
    QTest::addColumn<int>("init");
    QTest::addColumn<bool>("result");

    QTest::newRow("default constructor") << 0 << false;
    QTest::newRow("default constructor reload") << 1 << true;
    QTest::newRow("param constructor") << 2 << true;
}

void TestStorageFile::testRename()
{
    QFETCH(int, init);
    QFETCH(bool, result);

    if (init == 0)
    {
        m_StorageFile = new StorageFile();
    }
    else if (init == 1)
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
