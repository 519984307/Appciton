/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/5
 **/
#pragma once
#include "AlarmState.h"
#include "AlarmNormalState.h"
#include "AlarmPauseState.h"
#include "AlarmResetState.h"

class Factory
{
public:
    Factory(){}
    virtual AlarmState *create()
    {
        return new AlarmState();
    }
    virtual ~Factory(){}
};

class FactoryNormal : public Factory
{
public:
    FactoryNormal() : Factory(){}
    virtual AlarmState *create()
    {
        return new AlarmNormalState();
    }
    virtual ~FactoryNormal(){}
};

class FactoryPause  : public Factory
{
public:
    FactoryPause() : Factory(){}
    virtual AlarmState *create()
    {
        return new AlarmPauseState();
    }
    virtual ~FactoryPause(){}
};

class FactoryReset  : public Factory
{
public:
    FactoryReset() : Factory(){}
    virtual AlarmState *create()
    {
        return new AlarmResetState();
    }
    virtual ~FactoryReset(){}
};

class FactoryAbnormal : public Factory
{
public:
    FactoryAbnormal() : Factory(){}
    virtual AlarmState *create()
    {
        return new AlarmState();
    }
    virtual ~FactoryAbnormal(){}
};

class TestAlarmState : public QObject
{
    Q_OBJECT

public:
    TestAlarmState();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testType_data();
    void testType();
    void testBeginTimer_data();
    void testBeginTimer();
    void testEndTimer_data();
    void testEndTimer();

private:
    FactoryNormal *factoryNormal;
    FactoryPause *factoryPause;
    FactoryReset *factoryReset;
    FactoryAbnormal *factoryAbnormal;
};
