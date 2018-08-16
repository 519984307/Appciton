/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/15
 **/

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
