#pragma once
#include "SoftkeyActionBase.h"

class CalculateSoftkeyAction : public SoftkeyActionBase
{
public:
    static void doseCalculation(bool isPressed);
    static void Hemodynamic(bool isPressed);
    static void exit(bool isPressed);

    // 获取动作描述总个数。
    virtual int getActionDescNR(void);

    // 获取动作描述句柄。
    virtual KeyActionDesc *getActionDesc(int index);

    CalculateSoftkeyAction();
    virtual ~CalculateSoftkeyAction();
};

