/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/2/28
 **/

#pragma once
#include "MockAlarmIndicator.h"
#include <QTest>

class TestAlarmNormalState : public QObject
{
    Q_OBJECT
public:
    TestAlarmNormalState();
    ~TestAlarmNormalState();

private Q_SLOTS:
    void testEnter();
    void testHandAlarmEvent_data();
    void testHandAlarmEvent();
};
