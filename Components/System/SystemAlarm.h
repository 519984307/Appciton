#pragma once
#include "AlarmParamIFace.h"

enum SystemOneShotAlarm
{
    SOME_LIMIT_ALARM_DISABLED,

    POWERUP_PANEL_SYNC_PRESSED,
    POWERUP_PANEL_RECORD_PRESSED,
    SYSTEM_ONE_SHOT_ALARM_COMMUNICATION_STOP,
    SYSTEM_ONE_SHOT_ALARM_NR
};

class SystemAlarm : public AlarmOneShotIFace
{
public:
    static SystemAlarm &Construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SystemAlarm();
        }

        return *_selfObj;
    }

    static SystemAlarm *_selfObj;

    ~SystemAlarm();

public:
    // 报警源的名字。
    QString getAlarmSourceName(void);

    // 获取对应的参数ID。
    virtual ParamID getParamID(void) {return PARAM_NONE;}

    // 报警源的个数。
    virtual int getAlarmSourceNR(void) {return SYSTEM_ONE_SHOT_ALARM_NR;}

    // 获取报警对应的波形ID，该波形将被存储。
    virtual WaveformID getWaveformID(int /*id*/) {return WAVE_NONE;}

    // 生理参数报警级别。
    virtual AlarmPriority getAlarmPriority(int id);

    // 报警条件是否满足，满足则返回true。
    virtual bool isAlarmed(int id);

    // 该报警是否为生命报警，技术报警和生理/命报警分开存放。
    AlarmType getAlarmType(int id);

    // 将报警ID转换成字串。
    const char *toString(int id);

private:
    SystemAlarm();
};
#define systemAlarm (SystemAlarm::Construction())

