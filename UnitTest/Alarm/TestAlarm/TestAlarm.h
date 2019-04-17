/**
 ** This file is part of the unittest project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/4/2
 **/



#pragma once
#include <QTest>
#include "AlarmParamIFace.h"

class TestAlarmPrivate;
class TestAlarm : public QObject
{
    Q_OBJECT
public:
    TestAlarm();
    ~TestAlarm();
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();
    void testUpdateMuteKeyStatus_data();
    void testUpdateMuteKeyStatus();
    void testUpdateResetKeyStatus_data();
    void testUpdateResetKeyStatus();
    void testAddAlarmStatus_data();
    void testAddAlarmStatus();
    void testGetAlarmLimitIFace_data();
    void testGetAlarmLimitIFace();
    void testGetAlarmOneShotIFace_data();
    void testGetAlarmOneShotIFace();

    void testGetPhyAlarmMessage_data();
    void testGetPhyAlarmMessage();
    void testGetAlarmMessage_data();
    void testGetAlarmMessage();
    void testMainRunLimitSource_data();
    void testMainRunLimitSource();
    void testMainRunOneshotSource_data();
    void testMainRunOneshotSource();

private:
    TestAlarmPrivate *const d_ptr;
};

#define ALARM_COUNT 4
class LimitAlarm : public AlarmLimitIFace
{
public:
    LimitAlarm()
        : subParamId(SUB_PARAM_NONE),
          paramId(PARAM_NONE)
    {}
    ~LimitAlarm() {}

    // 报警源的名字。
    virtual QString getAlarmSourceName(void)
    {
        return QString("AlarmLimitSourceName");
    }

    // 获取对应的参数ID。
    virtual void setParamID(ParamID id)
    {
        paramId = id;
    }

    virtual ParamID getParamID(void)
    {
        return paramId;
    }

    virtual void setSubParamID(SubParamID id)
    {
        subParamId = id;
    }

    virtual SubParamID getSubParamID(int id)
    {
        Q_UNUSED(id)
        return subParamId;
    }

    // 报警源的个数。
    virtual int getAlarmSourceNR(void)
    {
        return ALARM_COUNT;
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
        switch (id)
        {
        case 0:
            return ALARM_PRIO_PROMPT;
        case 1:
            return ALARM_PRIO_LOW;
        case 2:
            return ALARM_PRIO_MED;
        default:
            return ALARM_PRIO_HIGH;
        }
    }

    virtual int getValue(int id)
    {
        Q_UNUSED(id)
        return 0;
    }

    // 生理参数报警使能。
    virtual bool isAlarmEnable(int id)
    {
        switch (id)
        {
        case 0:
        case 1:
        case 2:
            return true;
        default:
            return false;
        }
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
        switch (id)
        {
        case 0:
            return 0;
        case 1:
            return -1;
        case 2:
            return 1;
        default:
            break;
        }
        return 0;
    }

    // 将报警ID转换成字串。
    virtual const char *toString(int id)
    {
        Q_UNUSED(id)
        switch (id)
        {
        case 0:
            return "AlarmLimit0";
        default:
            return "AlarmLimit1";
        }
    }

private:
    SubParamID subParamId;
    ParamID paramId;
};

static bool firstIdAlarm = true;
static bool fourthIdAlarm = false;
static bool thirdIdRemove = true;
static bool firstIdEnable = false;
class OneShotAlarm : public AlarmOneShotIFace
{
public:
    OneShotAlarm() :
        paramId(PARAM_NONE)
    {}
    virtual const char *toString(int id)
    {
        Q_UNUSED(id)
        return "AlarmOneShot";
    }

    virtual QString getAlarmSourceName(void)
    {
        return QString("AlarmOneShotSourceName");
    }

    void setParamID(ParamID id)
    {
        paramId = id;
    }

    virtual ParamID getParamID(void)
    {
        return paramId;
    }

    virtual int getAlarmSourceNR(void)
    {
        return ALARM_COUNT;
    }

    virtual WaveformID getWaveformID(int id)
    {
        Q_UNUSED(id)
        return WAVE_NONE;
    }

    // 生理参数报警级别。
    virtual AlarmPriority getAlarmPriority(int id)
    {
        switch (id)
        {
        case 0:
            return ALARM_PRIO_PROMPT;
        case 1:
            return ALARM_PRIO_LOW;
        case 2:
            return ALARM_PRIO_MED;
        default:
            return ALARM_PRIO_HIGH;
        }
    }

    virtual AlarmType getAlarmType(int id)
    {
        switch (id)
        {
        case 0:
        case 1:
        case 2:
            return ALARM_TYPE_TECH;
        case 3:
            return ALARM_TYPE_PHY;
        default:
            return ALARM_TYPE_LIFE;
        }
    }

    virtual bool isNeedRemove(int id)
    {
        switch (id)
        {
        case 0:
        case 1:
        case 3:
            return false;
        default:
            return thirdIdRemove;
        }
    }

    // 生理参数报警使能。
    virtual bool isAlarmEnable(int id)
    {
        switch (id)
        {
        case 0:
            return firstIdEnable;
        case 2:
        case 3:
            return true;
        default:
            return false;
        }
    }

    // 是否发生报警
    virtual bool isAlarmed(int id)
    {
        switch (id)
        {
        case 0:
            return firstIdAlarm;
        case 1:
        case 2:
            return true;
        default:
            return fourthIdAlarm;
        }
    }

private:
    ParamID paramId;
};
