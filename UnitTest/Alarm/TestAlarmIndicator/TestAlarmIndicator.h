/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/19
 **/

#pragma once
#include <QTest>
#include "AlarmParamIFace.h"

class TestAlarmIndicatorPrivate;
class TestAlarmIndicator : public QObject
{
    Q_OBJECT
public:
    TestAlarmIndicator();
    ~TestAlarmIndicator(){}

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();

    void testHandleAlarmInfo_data();
    void testHandleAlarmInfo();
    void testUpdateAlarmInfo_data();
    void testUpdateAlarmInfo();

    void testLatchAlarmInfo_data();
    void testLatchAlarmInfo();
    void testUpdateLatchAlarmInfo_data();
    void testUpdateLatchAlarmInfo();

    void testPhyAlarmPauseStatusHandle_data();
    void testPhyAlarmPauseStatusHandle();

    void testUpdataAlarmPriority_data();
    void testUpdataAlarmPriority();

    void testResetStatusHandle_data();
    void testResetStatusHandle();

    void testPublishAlarmHasNoAlarm_data();
    void testPublishAlarmHasNoAlarm();
    void testPublishAlarmHasPhyAlarm_data();
    void testPublishAlarmHasPhyAlarm();
    void testPublishAlarmHasTechAlarm_data();
    void testPublishAlarmHasTechAlarm();

private:
    TestAlarmIndicatorPrivate * const d_ptr;
};

class LimitAlarm : public AlarmLimitIFace
{
public:
    LimitAlarm(){}
    ~LimitAlarm(){}

    // 报警源的名字。
    virtual QString getAlarmSourceName(void)
    {
        return QString("AlarmLimitSourceName");
    }

    // 获取对应的参数ID。
    virtual ParamID getParamID(void)
    {
        return PARAM_NONE;
    }

    // 报警源的个数。
    virtual int getAlarmSourceNR(void)
    {
        return 0;
    }

    // 获取报警对应的波形ID，该波形将被存储。
    virtual WaveformID getWaveformID(int id)
    {
        Q_UNUSED(id)
        return WAVE_NONE;
    }

    // 生理参数报警级别。
    virtual AlarmPriority getAlarmPriority(int id)
    {
        Q_UNUSED(id)
        return ALARM_PRIO_HIGH;
    }

    virtual int getValue(int id)
    {
        Q_UNUSED(id)
        return 0;
    }

    // 生理参数报警使能。
    virtual bool isAlarmEnable(int id)
    {
        Q_UNUSED(id)
        return false;
    }

    // 生理参数报警上限。
    virtual int getUpper(int id)
    {
        Q_UNUSED(id)
        return 100;
    }

    // 生理参数报警下限。
    virtual int getLower(int id)
    {
        Q_UNUSED(id)
        return 0;
    }

    // 报警参数值比较
    virtual int getCompare(int value, int id)
    {
        Q_UNUSED(value)
        Q_UNUSED(id)
        return 0;
    }

    // 将报警ID转换成字串。
    virtual const char *toString(int id)
    {
        Q_UNUSED(id)
        return "AlarmLimit";
    }

    virtual SubParamID getSubParamID(int id)
    {
        Q_UNUSED(id)
        return SUB_PARAM_NONE;
    }
};

class OneShotAlarm : public AlarmOneShotIFace
{
public:
    virtual const char *toString(int id)
    {
        Q_UNUSED(id)
        return "AlarmOneShot";
    }

    virtual QString getAlarmSourceName(void)
    {
        return QString("AlarmOneShotSourceName");
    }

    virtual ParamID getParamID(void)
    {
        return PARAM_NONE;
    }

    virtual int getAlarmSourceNR(void)
    {
        return 0;
    }

    virtual WaveformID getWaveformID(int id)
    {
        Q_UNUSED(id)
        return WAVE_NONE;
    }

    // 生理参数报警级别。
    virtual AlarmPriority getAlarmPriority(int id)
    {
        Q_UNUSED(id)
        return ALARM_PRIO_HIGH;
    }
};
