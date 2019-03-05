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
    void testReservedSize_data();
    void testReservedSize();
    void testReload_data();
    void testReload();
    void testGetBlockNR_data();
    void testGetBlockNR();
    void testAdditionalData_data();
    void testAdditionalData();
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
