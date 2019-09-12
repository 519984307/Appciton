/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/3/7
 **/
#include "TestErrorLog.h"
#include <unistd.h>
#include <sys/types.h>


TestErrorLogTest::TestErrorLogTest()
{
    QString newdir = QString("/run/user/%1/data").arg(getuid());
    QDir d(newdir);
    if (!d.exists())
    {
        d.mkdir(newdir);
    }
    errorLog.setStorageDir(newdir);
    errorLog.clear();
}

TestErrorLogTest::~TestErrorLogTest()
{
    errorLog.clear();
}

void TestErrorLogTest::initTestCase()
{
}

void TestErrorLogTest::cleanupTestCase()
{
}

void TestErrorLogTest::testCount_data()
{
    QTest::addColumn<int>("num");
    QTest::addColumn<int>("result");
    QTest::newRow("inRange") << 500  << 500;
    QTest::newRow("MaxRange") << 1000 << 1000;
    QTest::newRow("outOfRange") << 1001 << 1000;
}

void TestErrorLogTest::testCount()
{
    QFETCH(int, num);
    QFETCH(int, result);

    QString log("ErrorLog test");
    QString str;
    errorLog.clear();
    for (int i = 0; i < num; i++)
    {
        ErrorLogItem *item = new ErrorLogItem();
        str = "Test " + QString::number(i);
        item->setName(str);
        item->setLog(log);
        item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
        item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        errorLog.append(item);
    }
    errorLog.run();
    QCOMPARE(errorLog.count(), result);
}

void TestErrorLogTest::testGetLog()
{
    QString name("Test ErrorLog GetLog");
    QString log("Test ErrorLog GetLog");
    errorLog.clear();
    if (errorLog.count() == 0)
    {
        ErrorLogItem *item = new ErrorLogItem();
        item->setName(name);
        item->setLog(log);
        item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
        item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        errorLog.append(item);
        errorLog.run();

        int count = errorLog.count();
        ErrorLogItem* logitem = reinterpret_cast<ErrorLogItem*>(errorLog.getLog(count - 1));
        QCOMPARE(name, logitem->name());
    }
    else
    {
        QVERIFY2(false, "Test fail");
    }
}

void TestErrorLogTest::testAppendList_data()
{
    QTest::addColumn<int>("num");
    QTest::addColumn<int>("result");
    QTest::newRow("inRange") << 500 << 500;
    QTest::newRow("MaxRange") << 1000 << 1000;
    QTest::newRow("outOfRange") << 1001<< 1000;
}

void TestErrorLogTest::testAppendList()
{
    QFETCH(int, num);
    QFETCH(int, result);

    errorLog.clear();
    QCOMPARE(errorLog.count(), 0);
    QString name = "Test Error Log append";
    QString strname;
    QList<ErrorLogItemBase *> items;
    for (int i = 0; i < num; i++)
    {
        ErrorLogItem *item = new ErrorLogItem();
        strname = name + QString::number(i);
        item->setName(strname);
        item->setLog("Test Error Log append ");
        item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
        item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        items.append(reinterpret_cast<ErrorLogItemBase *>(item));
    }
    errorLog.append(items);
    errorLog.run();
    QCOMPARE(errorLog.count(), result);
}

void TestErrorLogTest::testGetLatestLog()
{
    errorLog.clear();
    QString name("Test ErrorLog GetLatestLog");
    QString log("Test ErrorLog GetLateestLog");
    ErrorLogItem *item = new ErrorLogItem();
    item->setName(name);
    item->setLog(log);
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(item);
    errorLog.run();

    QCOMPARE(errorLog.getLatestLog(ErrorLogItem::Type)->name(), name);
}

void TestErrorLogTest::testGetSummary()
{
    errorLog.clear();
    QString name("Test ErrorLog GetSummary");
    QString log("Test ErrorLog GetSummary");
    ErrorLogItem *item = new ErrorLogItem();
    item->setName(name);
    item->setLog(log);
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(item);
    errorLog.run();
    ErrorLog::Summary summary = errorLog.getSummary();
    QVERIFY2(summary.NumOfErrors == 1, "Test getSummary Failure");
}

void TestErrorLogTest::testRun()
{
    errorLog.clear();
    QString name("Test ErrorLog Run");
    QString log("Test ErrorLog Run");
    ErrorLogItem *item = new ErrorLogItem();
    item->setName(name);
    item->setLog(log);
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(item);
    errorLog.run();

    QCOMPARE(errorLog.count(), 1);
}

void TestErrorLogTest::testTypeCount()
{
    errorLog.clear();
    QString name("Test ErrorLog TypeCount");
    QString log("Test ErrorLog TypeCount");
    ErrorLogItem *criticalItem = new CriticalFaultLogItem();
    criticalItem->setName(name + "1");
    criticalItem->setLog(log);
    criticalItem->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    criticalItem->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    criticalItem->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(criticalItem);
    errorLog.run();
    QCOMPARE(errorLog.getTypeCount(CriticalFaultLogItem::Type), 1);

    ErrorLogItem *CrashItem = new CrashLogItem();
    CrashItem->setName(name + "2");
    CrashItem->setLog(log);
    CrashItem->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    CrashItem->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    CrashItem->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(CrashItem);
    errorLog.run();

    QCOMPARE(errorLog.getTypeCount(CrashLogItem::Type), 1);

    QCOMPARE(errorLog.count(), 2);
}

//  QTEST_APPLESS_MAIN(TestErrorLogTest)

//  #include "TestErrorLog.moc"
