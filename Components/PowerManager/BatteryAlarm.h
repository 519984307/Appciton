#pragma once
#include "AlarmParamIFace.h"

// OneShot报警实现。
class BatteryOneShotAlarm : public AlarmOneShotIFace
{
public:
    static BatteryOneShotAlarm &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new BatteryOneShotAlarm();
        }
        return *_selfObj;
    }
    static BatteryOneShotAlarm *_selfObj;

public:
    // 报警源的名字。
    virtual QString getAlarmSourceName(void);

    // 获取对应的参数ID。
    virtual ParamID getParamID(void) { return PARAM_NONE; }

    // 报警源的个数。
    virtual int getAlarmSourceNR(void);

    // 获取报警对应的波形ID，该波形将被存储。
    virtual WaveformID getWaveformID(int id);

    // 生理参数报警级别。
    virtual AlarmPriority getAlarmPriority(int id);

    // 该报警是否为生命报警，技术报警和生理/命报警分开存放。
    virtual AlarmType getAlarmType(int id);

    // 将报警ID转换成字串。
    virtual const char *toString(int id);

    // 构造与析构。
    virtual ~BatteryOneShotAlarm();

private:
    BatteryOneShotAlarm();
};
#define batteryOneShotAlarm (BatteryOneShotAlarm::construction())
