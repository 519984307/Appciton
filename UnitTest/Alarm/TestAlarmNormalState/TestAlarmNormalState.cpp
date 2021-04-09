/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/1
 **/


#include "TestAlarmNormalState.h"
#include "AlarmNormalState.h"
#include "MockAlarmIndicator.h"
#include "MockAlarmStateMachine.h"
#include "MockLightManager.h"

using ::testing::Eq;
using ::testing::Return;
using ::testing::Mock;
using ::testing::AnyNumber;
using ::testing::_;

Q_DECLARE_METATYPE(AlarmStateEvent)
Q_DECLARE_METATYPE(ALarmStateType)

TestAlarmNormalState::TestAlarmNormalState()
{
}

TestAlarmNormalState::~TestAlarmNormalState()
{
}

void TestAlarmNormalState::testEnter()
{
    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    EXPECT_CALL(mockAlarmIndicator, setAlarmStatus(Eq(ALARM_STATUS_NORMAL)));
    EXPECT_CALL(mockAlarmIndicator, clearAlarmPause());
    EXPECT_CALL(mockAlarmIndicator, updateAlarmAudioState());

    MockLightManager mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);
    EXPECT_CALL(mockLightManager, enableKeyboardBacklight(_));
    AlarmNormalState normalState;
    normalState.enter();
    QCOMPARE(normalState.type(), ALARM_NORMAL_STATE);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
}

void TestAlarmNormalState::testHandAlarmEvent_data()
{
    QTest::addColumn<AlarmStateEvent>("event");
    QTest::addColumn<ALarmStateType>("state");
    QTest::addColumn<int>("alarmCount");

    QTest::newRow("reset btn pressed") << ALARM_STATE_EVENT_RESET_BTN_PRESSED
                                       << ALARM_RESET_STATE
                                       << 0;
    QTest::newRow("reset btn pressed") << ALARM_STATE_EVENT_RESET_BTN_PRESSED
                                       << ALARM_RESET_STATE
                                       << 10;
    QTest::newRow("mute btn pressed") << ALARM_STATE_EVENT_MUTE_BTN_PRESSED
                                      << ALARM_PAUSE_STATE
                                      << 0;
    QTest::newRow("mute btn pressed") << ALARM_STATE_EVENT_MUTE_BTN_PRESSED
                                      << ALARM_PAUSE_STATE
                                      << 10;
}

void TestAlarmNormalState::testHandAlarmEvent()
{
    QFETCH(AlarmStateEvent, event);
    QFETCH(ALarmStateType, state);
    QFETCH(int, alarmCount);
    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    EXPECT_CALL(mockAlarmIndicator, delLatchPhyAlarm()).Times(AnyNumber());
    EXPECT_CALL(mockAlarmIndicator, clearAlarmPause()).Times(AnyNumber());
    EXPECT_CALL(mockAlarmIndicator, phyAlarmResetStatusHandle()).Times(AnyNumber());
    EXPECT_CALL(mockAlarmIndicator, techAlarmResetStatusHandle()).Times(AnyNumber());
    EXPECT_CALL(mockAlarmIndicator, getAlarmCount()).Times(AnyNumber()).WillOnce(testing::Return(alarmCount));
    EXPECT_CALL(mockAlarmIndicator, phyAlarmPauseStatusHandle()).Times(AnyNumber());

    MockAlarmStateMachine mockAlarmStateMachine;
    MockAlarmStateMachine::registerAlarmStateMachine(&mockAlarmStateMachine);
    EXPECT_CALL(mockAlarmStateMachine, switchState(Eq(state))).Times(AnyNumber());
    EXPECT_CALL(mockAlarmStateMachine, isEnableAlarmAudioOff()).Times(AnyNumber()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockAlarmStateMachine, isEnableAlarmOff()).Times(AnyNumber()).WillOnce(testing::Return(true));

    AlarmNormalState normalState;
    normalState.handAlarmEvent(event, 0, 0);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}
