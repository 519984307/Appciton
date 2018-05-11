#pragma once
#include "AGDefine.h"
#include "AlarmDefine.h"
#include <QString>

// 定义IBP数据提供对象需要实现的接口方法。
class AGProviderIFace
{
public:
    // set work mode.
    virtual void setWorkMode(AGWorkMode) {}

    // set no-breaths timeout.
    virtual void setApneaTimeout(ApneaAlarmTime) {}

    // set primary agent ID.
    virtual void setPrimaryID(AGAnaestheticType) {}

    // set O2 compensation.
    virtual void setO2Compensation(int) {}

    // set N2O compensation.
    virtual void setN2OCompensation(int) {}

    // zero calibration.
    virtual void zeroCalibration(void)  {}

    // get base line.
    virtual int getN2OBaseLine(void) = 0;
    virtual int getAA1BaseLine(void) = 0;
    virtual int getAA2BaseLine(void) = 0;
    virtual int getO2BaseLine(void) = 0;

    // get max waveform value.
    virtual int getN2OMaxWaveform(void) = 0;
    virtual int getAA1MaxWaveform(void) = 0;
    virtual int getAA2MaxWaveform(void) = 0;
    virtual int getO2MaxWaveform(void) = 0;

public:
    AGProviderIFace() {  }
    virtual ~AGProviderIFace() { }
};
