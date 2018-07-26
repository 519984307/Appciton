/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/

#define pragma once

#include "MenuContent.h"

class SoftWareVersionContentPrivate;
class SoftWareVersionContent : public MenuContent
{
    Q_OBJECT
public:
    SoftWareVersionContent();
    ~SoftWareVersionContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();

private:
    SoftWareVersionContentPrivate *const d_ptr;
};
