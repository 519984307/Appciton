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
#include "AlarmState.h"

class TestAlarmState : public QObject
{
    Q_OBJECT

public:
    TestAlarmState();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testType_data();
    void testType();
    void testBeginTimer_data();
    void testBeginTimer();
    void testEndTimer();
};

TestAlarmState::TestAlarmState()
{
}

void TestAlarmState::initTestCase()
{
}

void TestAlarmState::cleanupTestCase()
{
}

void TestAlarmState::testType_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<int>("reslut");

    QTest::newRow("type1") <<static_cast<int>(ALARM_STATE_NONE)
                           <<static_cast<int>(ALARM_STATE_NONE);

    QTest::newRow("type2") <<static_cast<int>(ALARM_NORMAL_STATE)
                           <<static_cast<int>(ALARM_NORMAL_STATE);

    QTest::newRow("type3") <<static_cast<int>(ALARM_PAUSE_STATE)
                           <<static_cast<int>(ALARM_PAUSE_STATE);

    QTest::newRow("type4") <<static_cast<int>(ALARM_AUDIO_OFF_STATE)
                           <<static_cast<int>(ALARM_AUDIO_OFF_STATE);

    QTest::newRow("type5") <<static_cast<int>(ALARM_OFF_STATE)
                           <<static_cast<int>(ALARM_OFF_STATE);

    QTest::newRow("type6") <<static_cast<int>(ALARM_RESET_STATE)
                           <<static_cast<int>(ALARM_RESET_STATE);
}

void TestAlarmState::testType()
{
    QFETCH(int, type);
    QFETCH(int, reslut);

    AlarmState state(static_cast<ALarmStateType>(type));
    QCOMPARE(static_cast<int>(state.type()), reslut);
}

void TestAlarmState::testBeginTimer_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<int>("interval");
    QTest::addColumn<bool>("timeIdIsVaild");

    QTest::newRow("test1") << static_cast<int>(ALARM_NORMAL_STATE) << 1000 << true;
    QTest::newRow("test2") << static_cast<int>(ALARM_NORMAL_STATE) << 50 << true;
    QTest::newRow("test3") << static_cast<int>(ALARM_NORMAL_STATE) << 0 << true;
    QTest::newRow("test4") << static_cast<int>(ALARM_PAUSE_STATE) << 1000 <<true;
    QTest::newRow("test5") << static_cast<int>(ALARM_PAUSE_STATE) << 50 <<true;
    QTest::newRow("test6") << static_cast<int>(ALARM_PAUSE_STATE) << 0 <<true;
    QTest::newRow("test7") << static_cast<int>(ALARM_RESET_STATE) <<1000 <<true;
    QTest::newRow("test8") << static_cast<int>(ALARM_RESET_STATE) <<50 <<true;
    QTest::newRow("test9") << static_cast<int>(ALARM_RESET_STATE) <<0 <<true;
    QTest::newRow("test10") << static_cast<int>(ALARM_AUDIO_OFF_STATE) << 1000 << true;
    QTest::newRow("test11") << static_cast<int>(ALARM_AUDIO_OFF_STATE) << 50 << true;
    QTest::newRow("test12") << static_cast<int>(ALARM_AUDIO_OFF_STATE) << 0 << true;
    QTest::newRow("test13") << static_cast<int>(ALARM_OFF_STATE) << 1000 << true;
    QTest::newRow("test14") << static_cast<int>(ALARM_OFF_STATE) << 50 << true;
    QTest::newRow("test15") << static_cast<int>(ALARM_OFF_STATE) << 0 << true;
    QTest::newRow("test13") << static_cast<int>(ALARM_STATE_NONE) << 1000 << true;
    QTest::newRow("test14") << static_cast<int>(ALARM_STATE_NONE) << 50 << true;
    QTest::newRow("test15") << static_cast<int>(ALARM_STATE_NONE) << 0 << true;
}

void TestAlarmState::testBeginTimer()
{
    QFETCH(int, type);
    QFETCH(int, interval);
    QFETCH(bool, timeIdIsVaild);
    AlarmState state(static_cast<ALarmStateType>(type));
    state.beginTimer(interval);
    bool result = state.getTimerID();
    QCOMPARE(result, timeIdIsVaild);
    state.killTimer(state.getTimerID());
}

void TestAlarmState::testEndTimer()
{
    AlarmState state;
    state.beginTimer(1000);
    state.endTimer();
    bool result = state.getTimerID();
    QCOMPARE(result , false);
}

QTEST_MAIN(TestAlarmState)

#include "tst_testalarmstate.moc"
