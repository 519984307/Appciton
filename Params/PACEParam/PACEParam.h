#pragma once
#include "Param.h"
#include "PACEDefine.h"

class PACEWidget;
class PACEProviderIFace;
class PACEParam: public Param
{
public:
    static PACEParam &Construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PACEParam();
        }
        return *_selfObj;
    }
    static PACEParam *_selfObj;
    ~PACEParam();

public:
    void pulseDelivered(void);

public:
    // 设置数据提供对象。
    void setProvider(PACEProviderIFace *provider);

    // 设置界面对象。
    void setWidget(PACEWidget *widget);

public:
    // 设置/获取工作模式。
    void setMode(PACEMode);
    PACEMode getMode(void);

    // 设置/获取输出频率。
    void setRate(int rate);
    int getRate(void);

    // 设置/获取输出电流。
    void setCurrent(int current);
    int getCurrent(void);

private:
    PACEParam();

    PACEProviderIFace *_provider;
    PACEWidget *_paceWidget;
};
#define paceParam (PACEParam::Construction())
