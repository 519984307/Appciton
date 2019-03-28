/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/3/25
 **/
#pragma once

#include <QString>
#include <QtTest>

class TestNIBPMonitorStandbyStatePrivate;
class TestNIBPMonitorStandbyState : public QObject
{
    Q_OBJECT

public:
    TestNIBPMonitorStandbyState();
    ~TestNIBPMonitorStandbyState();

private Q_SLOTS:
    void testEnter_data();
    void testEnter();
    void testHandleNIBPEvent_data();
    void testHandleNIBPEvent();
private:
    TestNIBPMonitorStandbyStatePrivate* const d_ptr;
};

