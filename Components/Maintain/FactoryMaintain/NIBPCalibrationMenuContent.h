/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/

#pragma once

#include "MenuContent.h"


class NIBPCalibrationMenuContent : public MenuContent
{
    Q_OBJECT
public:
    NIBPCalibrationMenuContent();
    ~NIBPCalibrationMenuContent() {}
protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
private slots:
    /**
     * @brief onBtnSlot  按钮槽函数
     */
    void onBtnSlot();
};
