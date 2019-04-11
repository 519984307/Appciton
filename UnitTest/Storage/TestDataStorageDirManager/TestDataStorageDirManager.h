/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/20
 **/

#pragma once
#include <QString>
#include <QtTest>
#include "MockTimeManager.h"
#include "AlarmParamIFace.h"

class TestDataStorageDirManager : public QObject
{
    Q_OBJECT

public:
    TestDataStorageDirManager();

private slots:
    void init();
    void cleanup();
    void testGetCurFolder();
    void testGetRescueEvent();
    void testGetRescueDataDir();
    void testIsCurRescueFolderFull_data();
    void testIsCurRescueFolderFull();
    void testDeleteData_data();
    void testDeleteData();
    void testCleanCurData();
};

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
        return 1;
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
        Q_UNUSED(id)
        return false;
    }

    // 生理参数报警使能。
    virtual bool isAlarmEnable(int id)
    {
        Q_UNUSED(id)
        return false;
    }

    // 是否发生报警
    virtual bool isAlarmed(int id)
    {
        Q_UNUSED(id)
        return false;
    }

    // 获取id对应的子参数ID。
    virtual SubParamID getSubParamID(int id)
    {
        Q_UNUSED(id)
        return SUB_PARAM_NONE;
    }

private:
    ParamID paramId;
};
