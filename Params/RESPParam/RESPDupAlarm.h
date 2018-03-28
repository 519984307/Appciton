#pragma once
#include "AlarmParamIFace.h"
#include "RESPSymbol.h"

// 超限报警实现。
class RESPDupLimitAlarm : public AlarmLimitIFace
{
public:
    static RESPDupLimitAlarm &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new RESPDupLimitAlarm();
        }
        return *_selfObj;
    }
    static RESPDupLimitAlarm *_selfObj;

public:
    // 报警源的名字。
    virtual QString getAlarmSourceName(void);

    // 获取对应的参数ID。
    virtual ParamID getParamID(void);

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

    virtual void notifyAlarm(int id, bool isAlarm);

    // 构造与析构。
    RESPDupLimitAlarm();
    virtual ~RESPDupLimitAlarm();

private:
    bool _isRRAlarm;
    bool _isBRAlarm;
};
#define respDupLimitAlarm (RESPDupLimitAlarm::construction())

