/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/16
 **/

#pragma once
#include "BatteryDefine.h"

class TestBatteryTimePrivate;
class QTime;
class TestBatteryTime
{
public:
    static TestBatteryTime &getInstance();
    virtual ~TestBatteryTime();

    void Record(BatteryPowerLevel const &level, unsigned int const &ad, const QTime &time);

    void open();
private:
    TestBatteryTime();
    TestBatteryTime(const TestBatteryTime &handle);
    TestBatteryTimePrivate* const d_ptr;
};

#define testBatteryTime (TestBatteryTime::getInstance())
