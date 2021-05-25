/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/22
 **/

#pragma once
#include "AlarmParamIFace.h"
#include "CO2Symbol.h"

// 超限报警实现。
class CO2LimitAlarm : public AlarmLimitIFace
{
public:
    CO2LimitAlarm();

public:
    // 报警源的名字。
    virtual QString getAlarmSourceName(void);

    // 获取对应的参数ID。
    virtual ParamID getParamID(void) { return PARAM_CO2; }

    // 报警源的个数。
    virtual int getAlarmSourceNR(void);

    // 获取报警对应的波形ID，该波形将被存储。
    virtual WaveformID getWaveformID(int id);

    // 获取id对应的参数ID。
    virtual SubParamID getSubParamID(int id);

    // 生理参数报警级别。
    virtual AlarmPriority getAlarmPriority(int id);

    // 获取指定的生理参数测量数据。
    virtual int getValue(int id);

    // 生理参数报警使能。
    virtual bool isAlarmEnable(int id);

    // 生理参数报警上限。
    virtual int getUpper(int id);

    // 生理参数报警下限。
    virtual int getLower(int id);

    // 生理参数比较
    virtual int getCompare(int value, int id);

    // 将报警ID转换成字串。
    virtual const char *toString(int id);

    //报警通知
    virtual void notifyAlarm(int id, bool flag);

    // 构造与析构。
    virtual ~CO2LimitAlarm();

private:
    bool _isEtco2Alarm;
    bool _isFico2Alarm;
    bool _isAwrrAlarm;
};

// OneShor报警实现。
class CO2OneShotAlarm : public AlarmOneShotIFace
{
public:
    CO2OneShotAlarm();

public:
    // 报警源的名字。
    virtual QString getAlarmSourceName(void);

    // 获取对应的参数ID。
    virtual ParamID getParamID(void) { return PARAM_CO2; }

    // 报警源的个数。
    virtual int getAlarmSourceNR(void);

    // 获取报警对应的波形ID，该波形将被存储。
    virtual WaveformID getWaveformID(int id);

    // 获取id对应的参数ID。
    virtual SubParamID getSubParamID(int id);

    // 生理参数报警级别。
    virtual AlarmPriority getAlarmPriority(int id);

    // 该报警是否为生命报警，技术报警和生理/命报警分开存放。
    virtual AlarmType getAlarmType(int id);

    // notify alarm
    virtual void notifyAlarm(int id, bool isAlarm);

    // 将报警ID转换成字串。
    virtual const char *toString(int id);

    // 报警使能
    virtual bool isAlarmEnable(int id);

    // 是否发生报警
    virtual bool isAlarmed(int id);

    // remove alarm message after latch
    virtual bool isRemoveAfterLatch(int id);

    // 构造与析构。
    virtual ~CO2OneShotAlarm();
};
