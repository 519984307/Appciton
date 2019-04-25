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
#include "COSymbol.h"

// overrun alarm.
class COLimitAlarm : public AlarmLimitIFace
{
public:
    COLimitAlarm();

public:
    virtual QString getAlarmSourceName(void);

    virtual ParamID getParamID(void) { return PARAM_CO; }

    // alarm source number.
    virtual int getAlarmSourceNR(void);

    // alarm id corresponding to waveform ID
    virtual WaveformID getWaveformID(int id);

    // alarm id corresponding to param ID
    virtual SubParamID getSubParamID(int id);

    // alarm id corresponding to alarm priority
    virtual AlarmPriority getAlarmPriority(int id);

    virtual int getValue(int id);

    // alarm enable
    virtual bool isAlarmEnable(int id);

    // alarm upper limit
    virtual int getUpper(int id);

    // alarm lower limit
    virtual int getLower(int id);

    virtual int getCompare(int value, int id);

    // alarm id convert string.
    virtual const char *toString(int id);

    // alarm notify
    virtual void notifyAlarm(int id, bool flag);

    // constructor and destructor.
    virtual ~COLimitAlarm();
};

// one shot alarm
class COOneShotAlarm : public AlarmOneShotIFace
{
public:
    COOneShotAlarm();

public:
    virtual QString getAlarmSourceName(void);

    virtual ParamID getParamID(void) { return PARAM_CO; }

    virtual int getAlarmSourceNR(void);

    virtual WaveformID getWaveformID(int id);

    virtual SubParamID getSubParamID(int id);

    virtual AlarmPriority getAlarmPriority(int id);

    virtual AlarmType getAlarmType(int id);

    virtual bool isAlarmed(int id);

    virtual void notifyAlarm(int id, bool isAlarm);

    virtual const char *toString(int id);

    virtual bool isAlarmEnable(int id);

    virtual bool isRemoveAfterLatch(int id);

    // constructor and destructor.
    virtual ~COOneShotAlarm();
};
