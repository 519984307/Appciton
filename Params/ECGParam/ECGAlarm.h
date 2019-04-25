/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/12/5
 **/

#pragma once
#include "AlarmParamIFace.h"
#include "ECGSymbol.h"

// 超限报警实现。
class ECGLimitAlarm : public AlarmLimitIFace
{
public:
    ECGLimitAlarm();

public:
    // 报警源的名字。
    virtual QString getAlarmSourceName(void);

    // 获取对应的参数ID。
    virtual ParamID getParamID(void)
    {
        return PARAM_ECG;
    }

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

    // 构造与析构。
    virtual ~ECGLimitAlarm();
};

// OneShor报警实现。
class ECGOneShotAlarm : public AlarmOneShotIFace
{
public:
    ECGOneShotAlarm();

public:
    // 报警源的名字。
    virtual QString getAlarmSourceName(void);

    // 获取对应的参数ID。
    virtual ParamID getParamID(void)
    {
        return PARAM_ECG;
    }

    // 是否发生报警
    virtual bool isAlarmed(int id);

    // 报警源的个数。
    virtual int getAlarmSourceNR(void);

    // 获取报警对应的波形ID，该波形将被存储。
    virtual WaveformID getWaveformID(int id);

    // 获取id对应的参数ID。
    virtual SubParamID getSubParamID(int id);

    // 生理参数报警级别。
    virtual AlarmPriority getAlarmPriority(int id);

    //产生报警通知
    virtual void notifyAlarm(int /*id*/, bool /*isAlarm*/);

    // 该报警是否为生命报警，技术报警和生理/命报警分开存放。
    virtual AlarmType getAlarmType(int id);

    // 将报警ID转换成字串。
    virtual const char *toString(int id);

    // 报警使能
    virtual bool isAlarmEnable(int id);

    // remove alarm message after latch
    virtual bool isRemoveAfterLatch(int id);

    virtual bool isRemoveLightAfterConfirm(int id);

    // 是否需要删除报警
    virtual bool isNeedRemove(int /*id*/);

    // 构造与析构。
    virtual ~ECGOneShotAlarm();

private:
    bool _isPhyAlarm;
};
