/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/18
 **/

#pragma once
#include <QTest>

class TestAlarmStateMachine : public QObject
{
    Q_OBJECT
public:
    TestAlarmStateMachine(){}
    ~TestAlarmStateMachine(){}

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testStart_data();
    void testStart();
    void testSwitchState_data();
    void testSwitchState();
    void testHandAlarmEvent_data();
    void testHandAlarmEvent();
};
