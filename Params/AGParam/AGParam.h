#pragma once
#include "Param.h"
#include "ParamInfo.h"
#include "AGDefine.h"
#include "AGProviderIFace.h"
#include "AlarmDefine.h"
#include "PhaseinProvider.h"

class AGWaveWidget;
class AGTrendWidget;
class AGParam: public Param
{
    Q_OBJECT

public:
    static AGParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new AGParam();
        }
        return *_selfObj;
    }
    ~AGParam();

public:
    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);

    // 获取可得的波形控件集。
    virtual void getAvailableWaveforms(QStringList &waveforms,
            QStringList &waveformShowName, int);

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 获取子参数值。
    virtual short getSubParamValue(SubParamID id);

    // check whether the sub param is avaliabe
    virtual bool isSubParamAvaliable(SubParamID id);

    // 设置数据提供对象。
    void setProvider(AGProviderIFace *provider);

    // 设置界面对象
    void setTrendWidget(AGTrendWidget *trendWidget, AGTypeGas gasType);
    void setWaveWidget(AGWaveWidget *waveWidget, AGTypeGas gasType);

    // 设置OneShot报警
    void setOneShotAlarm(AGOneShotType t, bool status);

public:
    // 设置/获取窒息时间。
    void setApneaTime(ApneaAlarmTime t);
    ApneaAlarmTime getApneaTime(void);

    // 处理模块的状态与主机状态同步。
    void verifyApneanTime(ApneaAlarmTime time);
    void verifyWorkMode(AGWorkMode mode);

    // 设置波形值。
    void addWaveformData(short wave, bool invalid, AGTypeGas gasType);

    // 设置/获取波形放大标尺。
    void setDisplayZoom(AGDisplayZoom zoom);
    AGDisplayZoom getDisplayZoom(void);

    // 设置获取et值
    unsigned char getEtData(AGTypeGas gasType);
    void setEtData(unsigned char etValue, AGTypeGas gasType);

    // 设置获取fi值
    unsigned char getFiData(AGTypeGas gasType);
    void setFiData(unsigned char fiValue, AGTypeGas gasType);

    // 设置主麻醉的麻醉剂类型
    void setAnestheticType(AGAnaestheticType primary, AGAnaestheticType secondary);

    // 设置波形速度
    void setSweepSpeed(AGSweepSpeed speed);

    // 获取AG模块状态信息
    void AGModuleStatus(AGProviderStatus status);

    // demo文件的生成
    bool getDemoWaveformFile(const char *buf, int len, AGTypeGas);
private:
    AGParam();
    static AGParam *_selfObj;
    void _setWaveformSpeed(AGSweepSpeed speed);
    void _setWaveformZoom(AGDisplayZoom zoom);

    AGProviderIFace *_provider;

    AGWaveWidget *_waveWidgetN2O;
    AGWaveWidget *_waveWidgetAA1;
    AGWaveWidget *_waveWidgetAA2;
    AGWaveWidget *_waveWidgetO2;

    AGTrendWidget *_trendWidgetN2O;
    AGTrendWidget *_trendWidgetAA1;
    AGTrendWidget *_trendWidgetAA2;
    AGTrendWidget *_trendWidgetO2;

    AGParamInfo _etn2o;
    AGParamInfo _fin2o;
    AGParamInfo _etaa1;
    AGParamInfo _fiaa1;
    AGParamInfo _etaa2;
    AGParamInfo _fiaa2;
    AGParamInfo _eto2;
    AGParamInfo _fio2;

    AGProviderStatus _config;

    char *_n2oBuf;

    int n2o;
};
#define agParam (AGParam::construction())
