#pragma once
#include "AlarmParamIFace.h"
#include "COSymbol.h"

// overrun alarm.
class COLimitAlarm : public AlarmLimitIFace
{
public:
    static COLimitAlarm &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new COLimitAlarm();
        }
        return *_selfObj;
    }
    static COLimitAlarm *_selfObj;

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
private:
    COLimitAlarm();


};
#define coLimitAlarm (COLimitAlarm::construction())

// one shot alarm
class COOneShotAlarm : public AlarmOneShotIFace
{
public:
    static COOneShotAlarm &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new COOneShotAlarm();
        }
        return *_selfObj;
    }
    static COOneShotAlarm *_selfObj;

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

private:
    COOneShotAlarm();
};
#define coOneShotAlarm (COOneShotAlarm::construction())
