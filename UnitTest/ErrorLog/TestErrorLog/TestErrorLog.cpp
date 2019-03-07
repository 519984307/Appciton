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

/*************************************************************************************************
 * Thread Func
 * **********************************************************************************************/
void errorLogThreadEntry()
{
    errorLog.run();
}

TestErrorLogTest::TestErrorLogTest()
{
    _mythread = new MyThread(errorLogThreadEntry);
    _mythread->start();
}

TestErrorLogTest::~TestErrorLogTest()
{
    _mythread->stop();

    delete _errorLogItem;
}

void TestErrorLogTest::initTestCase()
{
}

void TestErrorLogTest::cleanupTestCase()
{
}

void TestErrorLogTest::testCount()
{
    QString name("Test ErrorLog GetLog");
    QString log("Test ErrorLog GetLog");
    errorLog.clear();
    QCOMPARE(errorLog.count(), 0);

    ErrorLogItem *item = new ErrorLogItem();
    item->setName(name);
    item->setLog(log);
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
    errorLog.append(item);
    Util::waitInEventLoop(2000);

    QCOMPARE(errorLog.count(), 1);
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
        Util::waitInEventLoop(2000);

        int count = errorLog.count();
        ErrorLogItem* logitem = reinterpret_cast<ErrorLogItem*>(errorLog.getLog(count - 1));
        QCOMPARE(name, logitem->name());
    }
    else
    {
        QVERIFY2(false, "Test fail");
    }
}

void TestErrorLogTest::testAppend()
{
    QString name("Test ErrorLog Append");
    QString log("Test ErrorLog Append");
    errorLog.clear();

    ErrorLogItem *item = new ErrorLogItem();
    item->setName(name);
    item->setLog(log);
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
    errorLog.append(item);
    QCOMPARE(errorLog.count(), 0);
}

void TestErrorLogTest::testClear()
{
    QSKIP("implement the test", SkipAll);
}

void TestErrorLogTest::testTypeCount()
{
    errorLog.clear();
    Util::waitInEventLoop(500);
    ErrorLogItem *item = new ErrorLogItem();
    item->setName("Test Error Log getTypeCount");
    item->setLog("Test Error Log getTypeCount");
    item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);

//    errorLog.append(item);
    Util::waitInEventLoop(500);

    QCOMPARE(errorLog.getTypeCount(ErrorLogItem::Type), 1);

//    QVERIFY2(2 == errorLog.getTypeCount(ErrorLogItem::Type) &&
//             0 == errorLog.getTypeCount(CrashLogItem::Type) &&
//             0 == errorLog.getTypeCount(CriticalFaultLogItem::Type), "Test count append");
}




//  QTEST_APPLESS_MAIN(TestErrorLogTest)

//  #include "TestErrorLog.moc"
