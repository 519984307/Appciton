/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/11/5
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class FactoryVersionInfoPrivate;
class FactoryVersionInfo : public MenuContent
{
    Q_OBJECT

public:
    FactoryVersionInfo();
    ~FactoryVersionInfo();

protected:
    virtual void readyShow();
    virtual void layoutExec();

private:
    FactoryVersionInfoPrivate *const d_ptr;
};
