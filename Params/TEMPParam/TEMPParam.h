#pragma once
#include "Param.h"
#include "TEMPSymbol.h"

class TEMPTrendWidget;
class TEMPWaveWidget;
class TEMPProviderIFace;
class TEMPParam: public Param
{
public:
    static TEMPParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TEMPParam();
        }
        return *_selfObj;
    }
    static TEMPParam *_selfObj;
    ~TEMPParam();

public:
    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);

    // 获取子参数值
    virtual short getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(TEMPProviderIFace *provider);

    // 模块复位
    void reset();

    // 模块错误,模块禁用
    void setErrorDisable(void);
    bool getErrorDisable(void) { return _isTEMPDisable;}

    // 通信中断
    bool getDisconnected(void);

    // 设置服务模式升级数据提供对象。
    void setServiceProvider(TEMPProviderIFace *provider);

    // 设置界面对象。
    void setTrendWidget(TEMPTrendWidget *trendWidget);

    // 设置/获取TEMP的值。
    void setTEMP(short t1, short t2, short td);
    void getTEMP(short &t1, short &t2, short &td);

    // 设置OneShot报警。
    void setOneShotAlarm(TEMPOneShotType t, bool f);

    // 获取趋势界面。
    virtual void getTrendWindow(QString &trendWin);

    // 超限报警通知
    void noticeLimitAlarm(int id, bool flag);

    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    //接收定标数据
    void sendCalibrateData(int channel, int value);

    //接收定标数据
    void getCalibrateData(unsigned char *packet);

public:
    // 设置/获取单位。
    void setUnit(UnitType u);
    UnitType getUnit(void);

private:
    TEMPParam();

    TEMPProviderIFace *_provider;
    TEMPTrendWidget *_trendWidget;

    short _t1Value;
    short _t2Value;
    short _tdValue;

    int _calibrateChannel;
    int _calibrateValue;

    bool _isTEMPDisable;
};
#define tempParam (TEMPParam::construction())
