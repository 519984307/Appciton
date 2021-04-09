/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/14
 **/

#include "TestAlarmResetState.h"
#include "AlarmResetState.h"
#include "MockAlarmIndicator.h"
#include "MockAlarmStateMachine.h"
#include <QVariant>

using ::testing::Eq;
using ::testing::_;
using ::testing::Mock;
using ::testing::AnyNumber;

Q_DECLARE_METATYPE(AlarmStateEvent)
Q_DECLARE_METATYPE(ALarmStateType)

void TestAlarmResetState::initTestCase()
{
}

void TestAlarmResetState::cleanupTestCase()
{
}

void TestAlarmResetState::testEnter()
{
    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    EXPECT_CALL(mockAlarmIndicator, setAlarmStatus(Eq(ALARM_STATUS_RESET)));

    AlarmResetState resetState;
    resetState.enter();
    QCOMPARE(resetState.type(), ALARM_RESET_STATE);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
}

void TestAlarmResetState::testHandAlarmEvent_data()
{
    QTest::addColumn<AlarmStateEvent>("event");
    QTest::addColumn<ALarmStateType>("state");

    QTest::newRow("reset btn pressed") << ALARM_STATE_EVENT_RESET_BTN_PRESSED
                                       << ALARM_STATE_NONE;

    QTest::newRow("mute btn pressed") << ALARM_STATE_EVENT_MUTE_BTN_PRESSED
                                      << ALARM_PAUSE_STATE;

    QTest::newRow("new phy alarm") << ALARM_STATE_EVENT_NEW_PHY_ALARM
                                      << ALARM_NORMAL_STATE;

    QTest::newRow("new tech alarm") << ALARM_STATE_EVENT_NEW_TECH_ALARM
                                      << ALARM_NORMAL_STATE;
}

void TestAlarmResetState::testHandAlarmEvent()
{
    QFETCH(AlarmStateEvent, event);
    QFETCH(ALarmStateType, state);

    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);

    MockAlarmStateMachine mockAlarmStateMachine;
    MockAlarmStateMachine::registerAlarmStateMachine(&mockAlarmStateMachine);

    if (event != ALARM_STATE_EVENT_RESET_BTN_PRESSED)
    {
        EXPECT_CALL(mockAlarmStateMachine, switchState(Eq(state))).Times(AnyNumber());
    }
    if (event == ALARM_STATE_EVENT_MUTE_BTN_PRESSED)
    {
        EXPECT_CALL(mockAlarmIndicator, phyAlarmPauseStatusHandle()).Times(AnyNumber());
        EXPECT_CALL(mockAlarmIndicator, removeAllAlarmResetStatus()).Times(AnyNumber());
    }

    AlarmResetState resetState;
    resetState.handAlarmEvent(event, 0, 0);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}
