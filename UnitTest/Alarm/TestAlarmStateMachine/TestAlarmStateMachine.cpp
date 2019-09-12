/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/18
 **/

#include "TestAlarmStateMachine.h"
#include "IConfig.h"
#include "MockAlarmIndicator.h"
#include "MockLightManager.h"
#include "MockAlarmStateMachine.h"
#include <QVariant>
#include "MockAlarmState.h"
#include "AlarmStateMachine.h"

using ::testing::Eq;
using ::testing::Mock;
using ::testing::_;
using ::testing::Invoke;

Q_DECLARE_METATYPE(AlarmStatus)
Q_DECLARE_METATYPE(AlarmStateEvent)
Q_DECLARE_METATYPE(ALarmStateType)

void TestAlarmStateMachine::initTestCase()
{
    alarmStateMachine.getInstance();
    systemConfig.load(":/System.Original.xml");
}

void TestAlarmStateMachine::cleanupTestCase()
{
}

void TestAlarmStateMachine::testStart_data()
{
    QTest::addColumn<int>("alarmState");
    QTest::addColumn<AlarmStatus>("alarmStatus");
    QTest::addColumn<bool>("enableAlarmAudioMuteFlag");

    QTest::newRow("normal") << static_cast<int>(ALARM_NORMAL_STATE) << ALARM_STATUS_NORMAL << false;
    QTest::newRow("pause") << static_cast<int>(ALARM_PAUSE_STATE) << ALARM_STATUS_PAUSE << false;
    QTest::newRow("reset") << static_cast<int>(ALARM_RESET_STATE) << ALARM_STATUS_RESET << false;
    QTest::newRow("abnormal") << -1 << ALARM_STATUS_NORMAL << false;
    QTest::newRow("beyond") << 6 << ALARM_STATUS_NORMAL << false;
}

void TestAlarmStateMachine::testStart()
{
    QFETCH(int, alarmState);
    QFETCH(AlarmStatus, alarmStatus);
    QFETCH(bool, enableAlarmAudioMuteFlag);

    systemConfig.setNumValue("PrimaryCfg|Alarm|AlarmStatus", alarmState);
    systemConfig.saveToDisk();

    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    MockLightManager mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);
    if (alarmState > ALARM_STATE_NONE && alarmState <= ALARM_RESET_STATE)
    {
        EXPECT_CALL(mockAlarmIndicator, setAlarmStatus(Eq(alarmStatus)));
        /* don't turn on pause light any more, the light is useless */
        EXPECT_CALL(mockLightManager, enableAlarmAudioMute(enableAlarmAudioMuteFlag));
    }
    if (alarmState == ALARM_NORMAL_STATE)
    {
        // enter normal state
         EXPECT_CALL(mockAlarmIndicator, updateAlarmAudioState());
         EXPECT_CALL(mockAlarmIndicator, clearAlarmPause);
    }
    else if (alarmState == 2)
    {
        /* will not del phy alarm when pause */
        EXPECT_CALL(mockAlarmIndicator, delAllPhyAlarm()).Times(0);
        EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(_));
    }

    alarmStateMachine.start();
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockLightManager));
}

void TestAlarmStateMachine::testSwitchState_data()
{
    QTest::addColumn<ALarmStateType>("type");

    QTest::newRow("normalState") << ALARM_NORMAL_STATE;
    QTest::newRow("pauseState") << ALARM_PAUSE_STATE;
    QTest::newRow("resetState") << ALARM_RESET_STATE;
}

void TestAlarmStateMachine::testSwitchState()
{
    QFETCH(ALarmStateType, type);
    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    MockLightManager mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);

    EXPECT_CALL(mockAlarmIndicator, setAlarmStatus(static_cast<AlarmStatus>(type - 1)));
    EXPECT_CALL(mockLightManager, enableAlarmAudioMute(_));

    if (type == ALARM_NORMAL_STATE)
    {
        // 正常报警状态处理事件的期望
        EXPECT_CALL(mockAlarmIndicator, updateAlarmAudioState());
        EXPECT_CALL(mockAlarmIndicator, clearAlarmPause);
    }
    else if (type == ALARM_PAUSE_STATE)
    {
        /* will not delete phy alarm when alarm pause */
        EXPECT_CALL(mockAlarmIndicator, delAllPhyAlarm()).Times(0);
        EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(_));
    }
    else if (type == ALARM_RESET_STATE)
    {
        // 复位报警状态处理事件的期望
        EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(_));
    }

    alarmStateMachine.switchState(type);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockLightManager));
}

void TestAlarmStateMachine::testHandAlarmEvent_data()
{
    QTest::addColumn<ALarmStateType>("type");

    QTest::newRow("normalState") << ALARM_NORMAL_STATE;
    QTest::newRow("pauseState") << ALARM_PAUSE_STATE;
    QTest::newRow("resetState") << ALARM_RESET_STATE;
}

void TestAlarmStateMachine::testHandAlarmEvent()
{
    QFETCH(ALarmStateType, type);
    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    MockLightManager mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);
    MockAlarmStateMachine mockAlarmStateMachine;
    AlarmStateMachineInterface::registerAlarmStateMachine(& mockAlarmStateMachine);
    EXPECT_CALL(mockAlarmIndicator, setAlarmStatus(_));
    EXPECT_CALL(mockLightManager, enableAlarmAudioMute(_));
    if (type != ALARM_NORMAL_STATE)
    {
        // 非正常报警状态处理事件的期望
        EXPECT_CALL(mockAlarmIndicator, phyAlarmPauseStatusHandle());
    }
    EXPECT_CALL(mockAlarmStateMachine, switchState(_));
    if (type == ALARM_NORMAL_STATE)
    {
        // 正常报警状态处理事件的期望
        EXPECT_CALL(mockAlarmIndicator, updateAlarmAudioState());
        EXPECT_CALL(mockAlarmIndicator, clearAlarmPause);
    }
    else if (type == ALARM_PAUSE_STATE)
    {
        /* will not delete phy alarm when pause */
        EXPECT_CALL(mockAlarmIndicator, delAllPhyAlarm()).Times(0);
        EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(_));
    }
    else if (type == ALARM_RESET_STATE)
    {
        // 复位报警状态处理事件的期望
        EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(_));
        EXPECT_CALL(mockAlarmIndicator, removeAllAlarmResetStatus());
    }

    alarmStateMachine.switchState(type);
    alarmStateMachine.handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_PRESSED, NULL, 0);
    alarmStateMachine.handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_RELEASED, NULL, 0);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockLightManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}
