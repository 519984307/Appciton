/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/15
 **/

#pragma once
#include <QTest>

class TestAlarmPauseStatePrivate;
class TestAlarmPauseState : public QObject
{
    Q_OBJECT
public:
    TestAlarmPauseState();
    ~TestAlarmPauseState(){}

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testEnter_data();
    void testEnter();
    void testExit();
    void testHandAlarmEvent_data();
    void testHandAlarmEvent();

private:
    TestAlarmPauseStatePrivate * const d_ptr;
};
