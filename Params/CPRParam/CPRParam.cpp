#include "CPRParam.h"
#include "IConfig.h"
#include "CPRProviderIFace.h"
#include "CPRWidget.h"

CPRParam *CPRParam::_selfObj = NULL;

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void CPRParam::setProvider(CPRProviderIFace *provider)
{
    _provider = provider;

    // 初始化Provider。
    // CPR部分。
    _provider->setMinimumDepth(getMiminumDepth());
    _provider->setMetronomeRate(getMetronomeRate());
    _provider->setPressRateRange(getLowRateLimit(), getHighRateLimit());
    _provider->enableCPR(getEnable());
    _provider->enableMetronome(getMetronome());
}

/**************************************************************************************************
 * 设置界面对象。
 *************************************************************************************************/
void CPRParam::setWidget(CPRWidget *widget)
{
    _cprWidget = widget;
}

/**************************************************************************************************
 * 更新CPR波形数据。
 *************************************************************************************************/
void CPRParam::updateWaveform(short waveform[], int len)
{
    _cprWidget->updataWaveform(waveform, len);
}

/**************************************************************************************************
 * 更新CPR按压节律。
 *************************************************************************************************/
void CPRParam::updateCompressionRate(short rate)
{
    _cprWidget->updateCompressionRate(rate);
}

/**************************************************************************************************
 * 更新CPR语音提示。
 *************************************************************************************************/
void CPRParam::updateVoiceFeedback(CPRVoiceFeedback /*feedback*/)
{
    // todo.
}

/**************************************************************************************************
 * 设置使能状态。
 *************************************************************************************************/
void CPRParam::setEnable(CPREnable onoff)
{
    primaryConfig.setNumValue("CPR|Options|Enable", (int)onoff);
    _provider->enableCPR(onoff);
}

/**************************************************************************************************
 * 获取使能状态。
 *************************************************************************************************/
CPREnable CPRParam::getEnable(void)
{
    int onoff = CPR_ENABLE_OFF;
    primaryConfig.getNumValue("CPR|Options|Enable", onoff);
    return (CPREnable)onoff;
}


/**************************************************************************************************
 * 设置声音使能状态。
 *************************************************************************************************/
void CPRParam::setVoicePrompt(CPRVoicePrompt onoff)
{
    primaryConfig.setNumValue("CPR|Options|VoicePrompt", (int)onoff);
}

/**************************************************************************************************
 * 获取声音使能状态。
 *************************************************************************************************/
CPRVoicePrompt CPRParam::getVoicePrompt(void)
{
    int onoff = CPR_VOCIE_PROMPT_OFF;
    primaryConfig.getNumValue("CPR|Options|VoicePrompt", onoff);
    return (CPRVoicePrompt)onoff;
}

/**************************************************************************************************
 * 设置节拍器使能状。
 *************************************************************************************************/
void CPRParam::setMetronome(CPRMetronome onoff)
{
    primaryConfig.setNumValue("CPR|Options|Metronome", (int)onoff);
    _provider->enableMetronome(onoff);
}

/**************************************************************************************************
 * 获取节拍器使能状态。
 *************************************************************************************************/
CPRMetronome CPRParam::getMetronome(void)
{
    int onoff = CPR_METRONOME_OFF;
    primaryConfig.getNumValue("CPR|Options|Metronome", onoff);
    return (CPRMetronome)onoff;
}

/**************************************************************************************************
 * 获取节拍器节律。
 *************************************************************************************************/
void CPRParam::setMetronomeRate(int rate)
{
    primaryConfig.setNumValue("CPR|Options|MetronomeRate", rate);
    _provider->setMetronomeRate(rate);
}

/**************************************************************************************************
 * 获取节拍器节律。
 *************************************************************************************************/
int CPRParam::getMetronomeRate(void)
{
    int rate = 100;
    primaryConfig.getNumValue("CPR|Options|MetronomeRate", rate);
    return rate;
}

/**************************************************************************************************
 * 设置节律高限。
 *************************************************************************************************/
void CPRParam::setHighRateLimit(int limit)
{
    primaryConfig.setNumValue("CPR|Options|HighRateLimit", limit);
    _provider->setPressRateRange(getLowRateLimit(), limit);
}

/**************************************************************************************************
 * 获取节律高限。
 *************************************************************************************************/
int CPRParam::getHighRateLimit(void)
{
    int limit = 120;
    primaryConfig.getNumValue("CPR|Options|HighRateLimit", limit);
    return limit;
}

/**************************************************************************************************
 * 设置节律低限。
 *************************************************************************************************/
void CPRParam::setLowRateLimit(int limit)
{
    primaryConfig.setNumValue("CPR|Options|LowRateLimit", limit);
    _provider->setPressRateRange(limit, getHighRateLimit());
}

/**************************************************************************************************
 * 获取节律低限。
 *************************************************************************************************/
int CPRParam::getLowRateLimit(void)
{
    int limit = 80;
    primaryConfig.getNumValue("CPR|Options|LowRateLimit", limit);
    return limit;
}

/**************************************************************************************************
 * 设置目标按压深度。
 *************************************************************************************************/
void CPRParam::setMinimumDepth(int depth)
{
    primaryConfig.setNumValue("CPR|Options|MinimumDepth", (int)depth);
    _provider->setMinimumDepth(depth);
}

/**************************************************************************************************
 * 获取目标按压深度。
 *************************************************************************************************/
int CPRParam::getMiminumDepth(void)
{
    int depth = 2000;
    primaryConfig.getNumValue("CPR|Options|MinimumDepth", depth);
    return depth;
}

/**************************************************************************************************
 * 设置单位。
 *************************************************************************************************/
void CPRParam::setUnit(UnitType u)
{
    primaryConfig.setNumValue("CPR|Options|Unit", (int)u);
}

/**************************************************************************************************
 * 获取单位。
 *************************************************************************************************/
UnitType CPRParam::getUnit(void)
{
    int u = UNIT_MV;
    primaryConfig.getNumValue("CPR|Options|Unit", u);
    return (UnitType)u;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CPRParam::CPRParam() : Param(PARAM_CPR)
{
    _cprWidget = NULL;
    _provider = NULL;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CPRParam::~CPRParam()
{

}
