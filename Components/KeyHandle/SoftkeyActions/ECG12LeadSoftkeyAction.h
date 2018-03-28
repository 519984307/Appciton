#pragma once
#include "SoftkeyActionBase.h"

class ECG12LeadSoftkeyAction : public SoftkeyActionBase
{
public:
    static void ecg12LeaddiagECG(bool isPressed);
    static void ecg12LeadCapture(bool isPressed);
    static void ecg12LeadReview(bool isPressed);
    static void ecg12LeadExit(bool isPressed);

    // 获取动作描述总个数。
    virtual int getActionDescNR(void);

    // 获取动作描述句柄。
    virtual KeyActionDesc *getActionDesc(int index);

    ECG12LeadSoftkeyAction();
    virtual ~ECG12LeadSoftkeyAction();
};
