#pragma once
#include "SoftkeyActionBase.h"

class IMessageBox;
class DelRescueDataSoftkeyAction : public SoftkeyActionBase
{
public:
    static void delAll(bool isPressed);
    static void delCase(bool isPressed);
    static void exit(bool isPressed);

    // 获取动作描述总个数。
    virtual int getActionDescNR(void);

    // 获取动作描述句柄。
    virtual KeyActionDesc *getActionDesc(int index);

    DelRescueDataSoftkeyAction();
    virtual ~DelRescueDataSoftkeyAction();

private:
    static IMessageBox *_messageBox;
};


