/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/20
 **/

#include "TestDataStorageDirManager.h"
#include "DataStorageDirManager.h"
#include "MockTimeManager.h"
#include "MockSystemAlarm.h"
#include "AlarmSourceManager.h"
#include "SystemAlarm.h"
#include <unistd.h>
#include <sys/types.h>

using ::testing::_;
using ::testing::Return;

TestDataStorageDirManager::TestDataStorageDirManager()
{
    QString newdir = QString("/run/user/%1/data").arg(getuid());
    QDir d(newdir);
    if (!d.exists())
    {
        d.mkdir(newdir);
    }
    DataStorageDirManagerInterface::setDataStorageDir(newdir);
}

void TestDataStorageDirManager::init()
{
}

void TestDataStorageDirManager::cleanup()
{
    dataStorageDirManager.deleteAllData();
}

void TestDataStorageDirManager::testGetCurFolder()
{
    MockTimeManager mockTimeManager;
    TimeManagerInterface::registerTimeManager(&mockTimeManager);
    EXPECT_CALL(mockTimeManager, getPowerOnSession).Times(1).WillOnce(testing::Return(POWER_ON_SESSION_NEW));
    EXPECT_CALL(mockTimeManager, getStartTime).WillRepeatedly(testing::Return(0));
    dataStorageDirManager.createDir();
    QDir dir;
    bool exist = dir.exists(dataStorageDirManager.getCurFolder());
    QCOMPARE(exist, true);
}

void TestDataStorageDirManager::testGetRescueEvent()
{
    MockTimeManager mockTimeManager;
    TimeManagerInterface::registerTimeManager(&mockTimeManager);
    EXPECT_CALL(mockTimeManager, getPowerOnSession).Times(1).WillOnce(testing::Return(POWER_ON_SESSION_NEW));
    EXPECT_CALL(mockTimeManager, getStartTime).WillRepeatedly(testing::Return(0));
    dataStorageDirManager.createDir();
    QStringList timeList;
    dataStorageDirManager.getRescueEvent(timeList);
    QVERIFY(timeList.count() != 0);
}

void TestDataStorageDirManager::testGetRescueDataDir()
{
    MockTimeManager mockTimeManager;
    TimeManagerInterface::registerTimeManager(&mockTimeManager);
    EXPECT_CALL(mockTimeManager, getPowerOnSession).Times(1).WillOnce(testing::Return(POWER_ON_SESSION_NEW));
    EXPECT_CALL(mockTimeManager, getStartTime).WillRepeatedly(testing::Return(0));
    dataStorageDirManager.createDir();
    dataStorageDirManager.createDir();
    QString path = dataStorageDirManager.getRescueDataDir(0);
    QVERIFY(path != QString());
}

void TestDataStorageDirManager::testIsCurRescueFolderFull_data()
{
    QTest::addColumn<int>("folderSize");
    QTest::addColumn<bool>("result");

    QTest::newRow("0 size")
            << 0
            << false;
    QTest::newRow("max size")
            << SIGNAL_RESCUE_MAX_DATA_SIZE
            << false;
    QTest::newRow("full folder")
            << SIGNAL_RESCUE_MAX_DATA_SIZE + 1
            << true;
}

void TestDataStorageDirManager::testIsCurRescueFolderFull()
{
    QFETCH(int, folderSize);
    QFETCH(bool, result);

    MockTimeManager mockTimeManager;
    TimeManagerInterface::registerTimeManager(&mockTimeManager);
    EXPECT_CALL(mockTimeManager, getPowerOnSession).Times(1).WillOnce(testing::Return(POWER_ON_SESSION_NEW));
    EXPECT_CALL(mockTimeManager, getStartTime).WillRepeatedly(testing::Return(0));
    dataStorageDirManager.createDir();
    MockSystemAlarm systemAlarm;
    alarmSourceManager.registerOneShotAlarmSource(&systemAlarm, ONESHOT_ALARMSOURCE_SYSTEM);
    dataStorageDirManager.addDataSize(folderSize);
    QCOMPARE(dataStorageDirManager.isCurRescueFolderFull(), result);
    QCOMPARE(systemAlarm.isAlarmed(STORAGE_SPACE_FULL), false);
    QCOMPARE(systemAlarm.isAlarmed(STORAGE_SPACE_FULL), result);
}

void TestDataStorageDirManager::testDeleteData_data()
{
    QTest::addColumn<bool>("isCurFolder");
    QTest::addColumn<int>("dirNum");

    QTest::newRow("delete current folder")
            << true
            << 2;
    QTest::newRow("delete index 0")
            << false
            << 1;
}

void TestDataStorageDirManager::testDeleteData()
{
    QFETCH(bool, isCurFolder);
    QFETCH(int, dirNum);

    MockTimeManager mockTimeManager;
    TimeManagerInterface::registerTimeManager(&mockTimeManager);
    EXPECT_CALL(mockTimeManager, getPowerOnSession).Times(1).WillOnce(testing::Return(POWER_ON_SESSION_NEW));
    EXPECT_CALL(mockTimeManager, getStartTime).WillRepeatedly(testing::Return(0));
    dataStorageDirManager.createDir();
    if (isCurFolder)
    {
        QString path = dataStorageDirManager.getCurFolder();
        dataStorageDirManager.deleteData(path);
    }
    else
    {
        dataStorageDirManager.deleteData(0);
    }
    QCOMPARE(dataStorageDirManager.getDirNum(), dirNum);
}

void TestDataStorageDirManager::testCleanCurData()
{
    MockTimeManager mockTimeManager;
    TimeManagerInterface::registerTimeManager(&mockTimeManager);
    EXPECT_CALL(mockTimeManager, getPowerOnSession).Times(1).WillOnce(testing::Return(POWER_ON_SESSION_NEW));
    EXPECT_CALL(mockTimeManager, getStartTime).WillRepeatedly(testing::Return(0));
    dataStorageDirManager.createDir();
    QFile file(dataStorageDirManager.getCurFolder() + "/test.txt");
    if (!file.open(QIODevice::ReadWrite))
    {
        qDebug() << "creat test file fail!";
    }
    QCOMPARE(file.exists(), true);
    dataStorageDirManager.cleanCurData();
    QCOMPARE(file.exists(), false);
}
