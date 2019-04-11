/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/13
 **/

#pragma once
#include "AlarmParamIFace.h"

// 超限报警实现
class O2LimitAlarm : public AlarmLimitIFace
{
public:
    O2LimitAlarm();

public:
    // 报警源名字
    virtual QString getAlarmSourceName();

    // 获取对应的参数ID
    virtual ParamID getParamID(){return PARAM_O2;}

    // 报警源个数
    virtual int getAlarmSourceNR();

    // 获取报警对应的波形ID，该波形将被存储。
    virtual WaveformID getWaveformID(int id);

    // 获取报警ID对应的参数ID
    virtual SubParamID getSubParamID(int /*id*/){return SUB_PARAM_O2;}

    // 报警ID对应的报警级别
    virtual AlarmPriority getAlarmPriority(int id);

    // 报警ID对应的生理测量参数
    virtual int getValue(int id);

    // 生理参数报警使能
    virtual bool isAlarmEnable(int id);

    // 生理参数报警上限
    virtual int getUpper(int id);

    // 生理参数报警下限
    virtual int getLower(int id);

    // 生理参数比较
    virtual int getCompare(int value, int id);

    // 将报警ID转换成字串
    virtual const char *toString(int id);

    // 超限报警通知
    virtual void notifyAlarm(int id, bool flag);

private:
    // 构造与析构
    virtual ~O2LimitAlarm();

    bool _isO2Alarm;
};

// OneShot 报警实现
class O2OneShotAlarm : public AlarmOneShotIFace
{
public:
    O2OneShotAlarm();

public:
    // 报警源名字
    virtual QString getAlarmSourceName();

    // 获取对应参数ID
    virtual ParamID getParamID(){return PARAM_O2;}

    // 报警源的个数
    virtual int getAlarmSourceNR();

    // 获取报警对应的波形ID，该波形将被存储。
    virtual WaveformID getWaveformID(int id);

    // 报警ID对应的报警级别
    virtual AlarmPriority getAlarmPriority(int id);

    // 报警类型
    virtual AlarmType getAlarmType(int id);

    // 将报警ID转换成字串
    virtual const char *toString(int id);

    // 报警栓锁后移除报警信息
    virtual bool isRemoveAfterLatch(int id);

    // 报警ID是否报警
    virtual bool isAlarmed(int id);

private:
    virtual ~O2OneShotAlarm();
};
