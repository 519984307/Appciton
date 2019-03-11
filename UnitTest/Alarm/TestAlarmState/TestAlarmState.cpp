/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/2/28
 **/


#include <QString>
#include <QtTest>
#include "TestAlarmState.h"
#include <QVariant>

Q_DECLARE_METATYPE(ALarmStateType)
Q_DECLARE_METATYPE(Factory*)

TestAlarmState::TestAlarmState() :
    factoryNormal(NULL),
    factoryPause(NULL),
    factoryReset(NULL),
    factoryAbnormal(NULL)
{
}

void TestAlarmState::initTestCase()
{
    factoryNormal = new FactoryNormal();
    factoryPause = new FactoryPause();
    factoryReset = new FactoryReset();
    factoryAbnormal = new FactoryAbnormal();
}

void TestAlarmState::cleanupTestCase()
{
    delete factoryNormal;
    delete factoryPause;
    delete factoryReset;
    delete factoryAbnormal;
}

void TestAlarmState::testType_data()
{
    QTest::addColumn<Factory *>("factory");
    QTest::addColumn<ALarmStateType>("reslut");

    QTest::newRow("normalAlarm") << static_cast<Factory*>(factoryNormal) << ALARM_NORMAL_STATE;
    QTest::newRow("pauseAlarm") << static_cast<Factory*>(factoryPause) << ALARM_PAUSE_STATE;
    QTest::newRow("resetAlarm") << static_cast<Factory*>(factoryReset) << ALARM_RESET_STATE;
    QTest::newRow("noneAlarm") << static_cast<Factory*>(factoryAbnormal) << ALARM_STATE_NONE;
}

void TestAlarmState::testType()
{
    QFETCH(Factory *, factory);
    QFETCH(ALarmStateType, reslut);
    AlarmState *curState = factory->create();
    QCOMPARE(curState->type(), reslut);
    delete curState;
}

void TestAlarmState::testBeginTimer_data()
{
    QTest::addColumn<Factory *>("factory");
    QTest::addColumn<int>("interval");
    QTest::addColumn<bool>("timeIdIsVaild");

    QTest::newRow("normalAlarm0") << static_cast<Factory*>(factoryNormal)
                                  << 1000
                                  << true;
    QTest::newRow("normalAlarm1") << static_cast<Factory*>(factoryNormal)
                                  << 500
                                  << true;
    QTest::newRow("normalAlarm2") << static_cast<Factory*>(factoryNormal)
                                  << 0
                                  << true;

    QTest::newRow("pauseAlarm0") << static_cast<Factory*>(factoryPause)
                                 << 1000
                                 << true;
    QTest::newRow("pauseAlarm1") << static_cast<Factory*>(factoryPause)
                                 << 500
                                 << true;
    QTest::newRow("pauseAlarm2") << static_cast<Factory*>(factoryPause)
                                 << 0
                                 << true;

    QTest::newRow("resetAlarm0") << static_cast<Factory*>(factoryReset)
                                 << 1000
                                 << true;
    QTest::newRow("resetAlarm1") << static_cast<Factory*>(factoryReset)
                                 << 500
                                 << true;
    QTest::newRow("resetAlarm2") << static_cast<Factory*>(factoryReset)
                                 << 0
                                 << true;

    QTest::newRow("noneAlarm0") << static_cast<Factory*>(factoryAbnormal)
                                << 1000
                                << true;
    QTest::newRow("noneAlarm1") << static_cast<Factory*>(factoryAbnormal)
                                << 500
                                << true;
    QTest::newRow("noneAlarm2") << static_cast<Factory*>(factoryAbnormal)
                                << 0
                                << true;
}

void TestAlarmState::testBeginTimer()
{
    QFETCH(Factory *, factory);
    QFETCH(int, interval);
    QFETCH(bool, timeIdIsVaild);

    AlarmState *curState = factory->create();
    curState->beginTimer(interval);
    bool result =  curState->getTimerID();
    QCOMPARE(result, timeIdIsVaild);
    curState->endTimer();
    delete curState;
}

void TestAlarmState::testEndTimer_data()
{
    QTest::addColumn<Factory *>("factory");
    QTest::addColumn<int>("reslut");

    QTest::newRow("normalAlarm") << static_cast<Factory*>(factoryNormal) << 0;
    QTest::newRow("pauseAlarm") << static_cast<Factory*>(factoryPause) << 0;
    QTest::newRow("resetAlarm") << static_cast<Factory*>(factoryReset) << 0;
    QTest::newRow("noneAlarm") << static_cast<Factory*>(factoryAbnormal) << 0;
}

void TestAlarmState::testEndTimer()
{
    QFETCH(Factory *, factory);
    QFETCH(int, reslut);
    AlarmState *state = factory->create();
    state->beginTimer(1000);
    state->endTimer();
    QCOMPARE(state->getTimerID() , reslut);
    delete state;
}

QTEST_MAIN(TestAlarmState)
