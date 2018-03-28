#pragma once
#include "Param.h"
#include "CPRDefine.h"
#include "PDParamDefine.h" // todo 临时处理。

class CPRProviderIFace;
class CPRWidget;
class CPRParam: public Param
{
public:
    static CPRParam &Construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new CPRParam();
        }
        return *_selfObj;
    }
    static CPRParam *_selfObj;
    ~CPRParam();

public:
    // 设置数据提供对象。
    void setProvider(CPRProviderIFace *provider);

    // 设置界面对象。
    void setWidget(CPRWidget *widget);

public:
    void updateWaveform(short waveform[], int len);
    void updateCompressionRate(short rate);
    void updateVoiceFeedback(CPRVoiceFeedback feedback);

public:
    // 设置/获取使能状态。
    void setEnable(CPREnable onoff);
    CPREnable getEnable(void);

    // 设置/获取声音使能状态。
    void setVoicePrompt(CPRVoicePrompt onoff);
    CPRVoicePrompt getVoicePrompt(void);

    // 设置/获取节拍器使能状态。
    void setMetronome(CPRMetronome onoff);
    CPRMetronome getMetronome(void);

    // 设置/获取节拍器节律。
    void setMetronomeRate(int rate);
    int getMetronomeRate(void);

    // 设置/获取节律高限。
    void setHighRateLimit(int limit);
    int getHighRateLimit(void);

    // 设置/获取节律高限。
    void setLowRateLimit(int limit);
    int getLowRateLimit(void);

    // 设置/获取目标按压深度。
    void setMinimumDepth(int depth);
    int getMiminumDepth(void);

    // 设置/获取单位。
    void setUnit(UnitType u);
    UnitType getUnit(void);

private:
    CPRParam();

    CPRWidget *_cprWidget;
    CPRProviderIFace *_provider;
};
#define cprParam (CPRParam::Construction())
