#pragma once
#include "AlarmParamIFace.h"
#include "AGSymbol.h"

// overrun alarm.
class AGLimitAlarm : public AlarmLimitIFace
{
public:
    static AGLimitAlarm &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new AGLimitAlarm();
        }
        return *_selfObj;
    }
    static AGLimitAlarm *_selfObj;

public:
    virtual QString getAlarmSourceName(void);

    virtual ParamID getParamID(void) { return PARAM_AG; }

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
    virtual ~AGLimitAlarm();
private:
    AGLimitAlarm();


};
#define agLimitAlarm (AGLimitAlarm::construction())

// one shot alarm
class AGOneShotAlarm : public AlarmOneShotIFace
{
public:
    static AGOneShotAlarm &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new AGOneShotAlarm();
        }
        return *_selfObj;
    }
    static AGOneShotAlarm *_selfObj;

public:
    virtual QString getAlarmSourceName(void);

    virtual ParamID getParamID(void) { return PARAM_AG; }

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
    virtual ~AGOneShotAlarm();

private:
    AGOneShotAlarm();
};
#define agOneShotAlarm (AGOneShotAlarm::construction())
