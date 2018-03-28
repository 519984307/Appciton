#pragma once
#include "AlarmParamIFace.h"
#include "PrintDefine.h"

// OneShot报警实现。
class PrintOneShotAlarm : public AlarmOneShotIFace
{
public:
    static PrintOneShotAlarm &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PrintOneShotAlarm();
        }
        return *_selfObj;
    }
    static PrintOneShotAlarm *_selfObj;

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

    // acknowledge后是否移除
    virtual bool isRemoveAfterLatch(int id);

    // 构造与析构。
    virtual ~PrintOneShotAlarm();

private:
    PrintOneShotAlarm();
};
#define printOneShotAlarm (PrintOneShotAlarm::construction())
