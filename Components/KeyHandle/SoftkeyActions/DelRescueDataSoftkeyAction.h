/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/9
 **/
#pragma once
#include "SoftkeyActionBase.h"

class MessageBox;
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
    static MessageBox *_messageBox;
};


