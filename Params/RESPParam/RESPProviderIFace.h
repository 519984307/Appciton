#pragma once
#include "RESPDefine.h"
#include "AlarmDefine.h"

// 定义ECG数据提供对象需要实现的接口方法。
class RESPProviderIFace
{
public:
    // 最大数值
    virtual int maxRESPWaveValue(void) = 0;

    // 最小值
    virtual int minRESPWaveValue(void) = 0;

    // 获取波形的采样速度。
    virtual int getRESPWaveformSample(void) = 0;

    // 获取波形基线
    virtual int getRESPBaseLine(void) = 0;

    // 关闭窒息处理。
    virtual void disableApnea(void) = 0;

    // 设置窒息时间。
    virtual void setApneaTime(ApneaAlarmTime t) = 0;

    // 获取波形的采样速度。
    virtual void setWaveformZoom(RESPZoom zoom) = 0;

    // 设置计算导联
    virtual void setRESPCalcLead(RESPLead lead) = 0;

    // 启用RESP
    virtual void enableRESPCalc(bool enable) = 0;

    /**
     * @brief getRespModuleType get the module type
     * @return module type
     */
    virtual RESPModuleType getRespModuleType() const = 0;

    // 析构。
    virtual ~RESPProviderIFace() { }
};
