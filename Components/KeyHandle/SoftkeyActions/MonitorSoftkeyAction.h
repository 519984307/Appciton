#pragma once
#include "SoftkeyActionBase.h"

class MonitorSoftkeyAction : public SoftkeyActionBase
{
public:
    // 获取动作描述总个数。
    virtual int getActionDescNR(void);

    // 获取动作描述句柄。
    virtual KeyActionDesc *getActionDesc(int index);

    MonitorSoftkeyAction();
    virtual ~MonitorSoftkeyAction();
};
