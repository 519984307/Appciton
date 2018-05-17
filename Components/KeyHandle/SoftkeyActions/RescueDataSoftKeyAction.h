#pragma once
#include "SoftkeyActionBase.h"

class RescueDataSoftKeyAction : public SoftkeyActionBase
{
public:
    static void trendReview(bool isPressed);
    static void summaryReview(bool isPressed);
    static void clearData(bool isPressed);
    static void exportData(bool isPressed);
    static void exit(bool isPressed);
    static void eventReview(bool isPressed);
    static void oxyCRGEventReview(bool isPressed);

    // 获取动作描述总个数。
    virtual int getActionDescNR(void);

    // 获取动作描述句柄。
    virtual KeyActionDesc *getActionDesc(int index);

    RescueDataSoftKeyAction();
    virtual ~RescueDataSoftKeyAction();
};

