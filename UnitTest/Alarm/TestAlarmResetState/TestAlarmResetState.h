/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/14
 **/

#pragma once
#include <QTest>

class TestAlarmResetState : public QObject
{
    Q_OBJECT
public:
    TestAlarmResetState(){}
    ~TestAlarmResetState(){}

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testEnter();
    void testHandAlarmEvent_data();
    void testHandAlarmEvent();
};
