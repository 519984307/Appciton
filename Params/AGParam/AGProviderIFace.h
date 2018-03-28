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

public:
    AGProviderIFace() {  }
    virtual ~AGProviderIFace() { }
};
