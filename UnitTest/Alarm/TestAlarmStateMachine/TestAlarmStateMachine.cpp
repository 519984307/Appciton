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
}

void TestAlarmStateMachine::cleanupTestCase()
{
}

void TestAlarmStateMachine::testStart_data()
{
    QTest::addColumn<int>("alarmState");
    QTest::addColumn<AlarmStatus>("alarmStatus");
    QTest::addColumn<bool>("enableAlarmAudioMuteFlag");

    QTest::newRow("normal") << 1 << ALARM_STATUS_NORMAL << false;
    QTest::newRow("pause") << 2 << ALARM_STATUS_PAUSE << true;
    QTest::newRow("reset") << 5 << ALARM_STATUS_RESET << false;
    QTest::newRow("abnormal") << -1 << ALARM_STATUS_NORMAL << false;
    QTest::newRow("beyond") << 6 << ALARM_STATUS_NORMAL << false;
}

void TestAlarmStateMachine::testStart()
{
    QFETCH(int, alarmState);
    QFETCH(bool, enableAlarmAudioMuteFlag);
    QFETCH(AlarmStatus, alarmStatus);

    systemConfig.setNumValue("PrimaryCfg|Alarm|AlarmStatus", alarmState);
    systemConfig.saveToDisk();

    MockAlarmIndicator mockAlarmIndicator;
    AlarmIndicatorInterface::registerAlarmIndicator(&mockAlarmIndicator);
    MockLightManager mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);
    if (alarmState > ALARM_STATE_NONE && alarmState <= ALARM_RESET_STATE)
    {
        EXPECT_CALL(mockAlarmIndicator, setAlarmStatus(Eq(alarmStatus)));
        EXPECT_CALL(mockLightManager, enableAlarmAudioMute(enableAlarmAudioMuteFlag));
    }
    if (alarmState == 1)
    {
        // enter normal state
        EXPECT_CALL(mockAlarmIndicator, updateAlarmStateWidget());
    }
    else if (alarmState == 2)
    {
        EXPECT_CALL(mockAlarmIndicator, delAllPhyAlarm());
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
        EXPECT_CALL(mockAlarmIndicator, updateAlarmStateWidget());
    }
    else if (type == ALARM_PAUSE_STATE)
    {
        EXPECT_CALL(mockAlarmIndicator, delAllPhyAlarm());
        EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(_));
    }
    else if (type == ALARM_RESET_STATE)
    {
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
    EXPECT_CALL(mockAlarmIndicator, phyAlarmPauseStatusHandle());
    EXPECT_CALL(mockAlarmStateMachine, switchState(_));
    if (type == ALARM_NORMAL_STATE)
    {
        EXPECT_CALL(mockAlarmIndicator, updateAlarmStateWidget());
    }
    else if (type == ALARM_PAUSE_STATE)
    {
        EXPECT_CALL(mockAlarmIndicator, delAllPhyAlarm());
        EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(_));
    }
    else if (type == ALARM_RESET_STATE)
    {
        EXPECT_CALL(mockAlarmIndicator, updateAlarmPauseTime(_));
    }

    alarmStateMachine.switchState(type);
    alarmStateMachine.handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_PRESSED, NULL, 0);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockLightManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}
