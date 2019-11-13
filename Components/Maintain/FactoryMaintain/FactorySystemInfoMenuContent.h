/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/23
 **/
#pragma once
#include "Framework/UI/MenuContent.h"

class FactorySystemInfoMenuContentPrivate;
class FactorySystemInfoMenuContent : public MenuContent
{
    Q_OBJECT
public:
    FactorySystemInfoMenuContent();
    ~FactorySystemInfoMenuContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();

private slots:
    /**
     * @brief onBtnReleasedChanged
     */
    void onBtnReleasedChanged();

private:
    FactorySystemInfoMenuContentPrivate *const d_ptr;
};


