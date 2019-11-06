/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/3/7
 **/
#include <QString>
#include <QtTest>
#include "Framework/ErrorLog/ErrorLog.h"
#include "mythread.h"
class TestErrorLogTest : public QObject
{
    Q_OBJECT

public:
    TestErrorLogTest();
    ~TestErrorLogTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testTypeCount();
    void testCount_data();
    void testCount();
    void testGetLog();
    void testAppendList_data();
    void testAppendList();
    void testGetLatestLog();
    void testGetSummary();
    void testRun();
};
