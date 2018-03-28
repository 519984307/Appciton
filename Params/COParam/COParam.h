#pragma once
#include "Param.h"
#include "COProviderIFace.h"

class COTrendWidget;
class COMenu;
class COParam : public Param
{
public:
    static COParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new COParam();
        }
        return *_selfObj;
    }
    ~COParam();

public:
    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 获取子参数值。
    virtual short getSubParamValue(SubParamID id);

    // 设置数据提供对象。
    void setProvider(COProviderIFace *provider);

    // 设置界面对象
    void setCOTrendWidget(COTrendWidget *trendWidget);

public:// Data sent by host
    // C.O.系数
    void setCORatio(unsigned short coRatio);

    // set source of injection temp.
    void setTempSource(COTiMode source, unsigned short temp = 0);

    // set injection volumn.
    void setInjectionVolumn(unsigned char volumn);

    // measure control start stop  and interrupt.
    void measureCtrl(COInstCtl sta);

public:// Data sent by module
    // C.O. and C.I. data content.
    void measureResultCO(unsigned short coData, unsigned short ciData);

    // temp blood data content.
    void realTimeTBData(unsigned short tbData);

public:
    // get C.O. data.
    unsigned short getCOData(void);

    // get C.I. data.
    unsigned short getCIData(void);

    // get TB data.
    unsigned short getTBData(void);


private:
    COParam();
    static COParam *_selfObj;

    COProviderIFace *_provider;

    COTrendWidget *_trendWidget;

    unsigned short _coData;
    unsigned short _ciData;
    unsigned short _tbData;

};
#define coParam (COParam::construction())
