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

void errorLogThreadEntry()
{
    errorLog.run();
}

TestErrorLogTest::TestErrorLogTest():item(NULL), _myThread(NULL)
{
    errorLog.clear();
    _myThread = new MyThread(errorLogThreadEntry);
    _myThread->start();
}

TestErrorLogTest::~TestErrorLogTest()
{
    errorLog.clear();
    delete _myThread;
    delete item;
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
    QTest::addColumn<int>("time");
    QTest::addColumn<int>("result");
    QTest::newRow("inRange") << 500 << 75000 << 500;
    QTest::newRow("MaxRange") << 1000 << 150000 << 1000;
    QTest::newRow("outOfRange") << 1001 << 150000 << 1000;
}

void TestErrorLogTest::testCount()
{
    QFETCH(int, num);
    QFETCH(int, time);
    QFETCH(int, result);

    QString name("Test ErrorLog Count");
    QString log("Test ErrorLog Count");
    QString str;
    errorLog.clear();
    QList<ErrorLogItemBase *> items;
    for (int i = 0; i < num; i++)
    {
    item = new ErrorLogItem();
    str = name + QString::number(i);
    item->setName(str);
    item->setLog(log);
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
    errorLog.append(item);
    }
    Util::waitInEventLoop(time);
    QCOMPARE(errorLog.count(), result);
}

void TestErrorLogTest::testGetLog()
{
    QString name("Test ErrorLog GetLog");
    QString log("Test ErrorLog GetLog");
    errorLog.clear();
    if (errorLog.count() == 0)
    {
        item = new ErrorLogItem();
        item->setName(name);
        item->setLog(log);
        item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
        item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        errorLog.append(item);
        Util::waitInEventLoop(1000);

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
    QTest::addColumn<int>("time");
    QTest::addColumn<int>("result");
    QTest::newRow("inRange") << 500 << 75000 << 500;
    QTest::newRow("MaxRange") << 1000 << 150000 << 1000;
    QTest::newRow("outOfRange") << 1001 << 150000 << 1000;
}

void TestErrorLogTest::testAppendList()
{
    QFETCH(int, num);
    QFETCH(int, time);
    QFETCH(int, result);

    errorLog.clear();
    QCOMPARE(errorLog.count(), 0);
    QString name = "Test Error Log append";
    QString strname;
    QList<ErrorLogItemBase *> items;
    for (int i = 0; i < num; i++)
    {
    item = new ErrorLogItem();
    strname = name + QString::number(i);
    item->setName(strname);
    item->setLog("Test Error Log append ");
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
    items.append(reinterpret_cast<ErrorLogItemBase *>(item));
    }
    errorLog.append(items);
    Util::waitInEventLoop(time);
    QCOMPARE(errorLog.count(), result);
}

void TestErrorLogTest::testGetLatestLog()
{
    errorLog.clear();
    QString name("Test ErrorLog GetLatestLog");
    QString log("Test ErrorLog GetLateestLog");
    item = new ErrorLogItem();
    item->setName(name);
    item->setLog(log);
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(item);
    Util::waitInEventLoop(1000);

    QCOMPARE(errorLog.getLatestLog(ErrorLogItem::Type)->name(), name);
}

void TestErrorLogTest::testGetSummary()
{
    errorLog.clear();
    QString name("Test ErrorLog GetSummary");
    QString log("Test ErrorLog GetSummary");
    item = new ErrorLogItem();
    item->setName(name);
    item->setLog(log);
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(item);
    Util::waitInEventLoop(1000);
    ErrorLog::Summary summary = errorLog.getSummary();
    QVERIFY2(summary.NumOfErrors == 1, "Test getSummary Failure");
}

void TestErrorLogTest::testRun()
{
    errorLog.clear();
    _myThread->stop();
    QString name("Test ErrorLog Run");
    QString log("Test ErrorLog Run");
    item = new ErrorLogItem();
    item->setName(name);
    item->setLog(log);
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(item);
    Util::waitInEventLoop(1000);

    QCOMPARE(errorLog.count(), 0);
}

void TestErrorLogTest::testTypeCount()
{
    errorLog.clear();
    QString name("Test ErrorLog TypeCount");
    QString log("Test ErrorLog TypeCount");
    ErrorLogItem *criticalItem = new CriticalFaultLogItem();
    criticalItem->setName(name);
    criticalItem->setLog(log);
    criticalItem->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    criticalItem->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    criticalItem->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(criticalItem);
    Util::waitInEventLoop(2000);
    QCOMPARE(errorLog.getTypeCount(CriticalFaultLogItem::Type), 1);

    ErrorLogItem *CrashItem = new CrashLogItem();
    CrashItem->setName(name);
    CrashItem->setLog(log);
    CrashItem->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    CrashItem->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    CrashItem->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

    errorLog.append(CrashItem);
    Util::waitInEventLoop(2000);

    QCOMPARE(errorLog.getTypeCount(CrashLogItem::Type), 1);

    QCOMPARE(errorLog.count(), 2);
}

//  QTEST_APPLESS_MAIN(TestErrorLogTest)

//  #include "TestErrorLog.moc"
