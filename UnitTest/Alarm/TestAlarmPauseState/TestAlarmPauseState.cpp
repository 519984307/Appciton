/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/15
 **/

#include "TestAlarmPauseState.h"
#include "AlarmPauseState.h"
#include "MockAlarmIndicator.h"
#include "MockAlarmStateMachine.h"
#include "MockLightManager.h"
#include "IConfig.h"
#include <QVariant>

using ::testing::Eq;
using ::testing::Mock;
using ::testing::_;
using ::testing::Return;

Q_DECLARE_METATYPE(AlarmStateEvent)

class TestAlarmPauseStatePrivate
{
public:
    TestAlarmPauseStatePrivate(){}
    ~TestAlarmPauseStatePrivate(){}
    AlarmPauseState pauseState;
};

TestAlarmPauseState::TestAlarmPauseState()
    : d_ptr(new TestAlarmPauseStatePrivate)
{
}

void TestAlarmPauseState::initTestCase()
{
}

void TestAlarmPauseState::cleanupTestCase()
{
}

void TestAlarmPauseState::testEnter_data()
{
    QTest::addColumn<int>("index");
    QTest::addColumn<int>("pauseTime");

    QTest::newRow("1min") << 0 << 60;
    QTest::newRow("2min") << 1 << 120;
    QTest::newRow("3min") << 2 << 180;
    QTest::newRow("5min") << 3 << 300;
    QTest::newRow("10min") << 4 << 600;
    QTest::newRow("15min") << 5 << 900;
    QTest::newRow("NR") << 6 << 120;
    QTest::newRow("invaild value") << -1 << 120;
}

void TestAlarmPauseState::testEnter()
{
    QFETCH(int, index);
    QFETCH(int, pauseTime);

    systemConfig.setNumValue("Alarms|AlarmPauseTime", index);

    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    EXPECT_CALL(mockAlarmIndicator, setAlarmStatus(Eq(ALARM_STATUS_PAUSE)));
    EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(static_cast<AlarmPauseTime>(pauseTime)));
    EXPECT_CALL(mockAlarmIndicator, removeAllAlarmResetStatus());

    MockLightManager mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);
    /* the alarm pause light not longer turn on in alarm pause state */
    EXPECT_CALL(mockLightManager, enableKeyboardBacklight(Eq(false)));

    d_ptr->pauseState.enter();
    QCOMPARE(d_ptr->pauseState.type(), ALARM_PAUSE_STATE);
    QCOMPARE(d_ptr->pauseState.getTimerID() != 0, true);

    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockLightManager));
}

void TestAlarmPauseState::testExit()
{
    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(_));
    d_ptr->pauseState.exit();
    QCOMPARE(d_ptr->pauseState.getTimerID(), 0);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
}

void TestAlarmPauseState::testHandAlarmEvent_data()
{
    QTest::addColumn<AlarmStateEvent>("event");

    QTest::newRow("mute") << ALARM_STATE_EVENT_MUTE_BTN_PRESSED;
    QTest::newRow("all_phy_alarm_latch") << ALARM_STATE_EVENT_ALL_PHY_ALARM_LATCHED;
    QTest::newRow("no_pause_phy_alarm") << ALARM_STATE_EVENT_NO_PAUSED_PHY_ALARM;
    QTest::newRow("reset") << ALARM_STATE_EVENT_RESET_BTN_PRESSED;
    QTest::newRow("new_phy_alarm") << ALARM_STATE_EVENT_NEW_PHY_ALARM;
    QTest::newRow("new_tech_alarm") << ALARM_STATE_EVENT_NEW_TECH_ALARM;
    QTest::newRow("no_acknowledg") << ALARM_STATE_EVENT_NO_ACKNOWLEDG_ALARM;
    QTest::newRow("NR") << ALARM_STATE_EVENT_NR;
}

void TestAlarmPauseState::testHandAlarmEvent()
{
    QFETCH(AlarmStateEvent, event);

    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    MockAlarmStateMachine mockAlarmStateMachine;
    AlarmStateMachineInterface::registerAlarmStateMachine(&mockAlarmStateMachine);
    if (event == ALARM_STATE_EVENT_MUTE_BTN_PRESSED)
    {
        EXPECT_CALL(mockAlarmIndicator, phyAlarmPauseStatusHandle());
        EXPECT_CALL(mockAlarmStateMachine, switchState(Eq(ALARM_NORMAL_STATE)));
    }
    else if (event == ALARM_STATE_EVENT_RESET_BTN_PRESSED)
    {
        EXPECT_CALL(mockAlarmIndicator, clearAlarmPause());
        EXPECT_CALL(mockAlarmIndicator, phyAlarmResetStatusHandle());
        EXPECT_CALL(mockAlarmIndicator, techAlarmResetStatusHandle());
        EXPECT_CALL(mockAlarmIndicator, getAlarmCount()).WillRepeatedly(Return(0));
    }
    else if (event == ALARM_STATE_EVENT_NEW_PHY_ALARM || event == ALARM_STATE_EVENT_NEW_TECH_ALARM)
    {
        EXPECT_CALL(mockAlarmIndicator, clearAlarmPause());
        EXPECT_CALL(mockAlarmStateMachine, switchState(Eq(ALARM_NORMAL_STATE)));
    }

    d_ptr->pauseState.handAlarmEvent(event, NULL, 0);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}
