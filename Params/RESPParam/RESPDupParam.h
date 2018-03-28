#pragma once
#include "Param.h"
#include "RESPSymbol.h"

class RESPTrendWidget;
class CO2TrendWidget;
class RESPProviderIFace;
class RESPDupParam: public Param
{
public:
    enum BrSourceType {
        BR_SOURCE_CO2,
        BR_SOURCE_RESP,
    };

    static RESPDupParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new RESPDupParam();
        }
        return *_selfObj;
    }
    static RESPDupParam *_selfObj;

    // 析构。
    virtual ~RESPDupParam();

public:
    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);

    // 获取可得的波形控件集。
    virtual void getAvailableWaveforms(QStringList &waveforms,
            QStringList &waveformShowName, int flag = 0);

    // 获取趋势窗体
    virtual void getTrendWindow(QString &trendWin);

    // 获取子参数值。
    virtual short getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 使能
    virtual bool isEnabled();

    // 设置界面对象。
    void setTrendWidget(RESPTrendWidget *trendWidget);

    // 是否发生报警
    void isAlarm(bool isAlarm, bool isLimit);

    // 设置/获取RR的值。
    void updateRR(short rr);
    void updateBR(short br);
    short getRR(void);

    //get br source type
    BrSourceType getBrSource() const;

private:
    // 构造。
    RESPDupParam();

    RESPTrendWidget *_trendWidget;

    short _rrValue;
    short _brValue;
    bool _isAlarm;
};
#define respDupParam (RESPDupParam::construction())

