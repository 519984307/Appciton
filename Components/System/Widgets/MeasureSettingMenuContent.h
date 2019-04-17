/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/18
 **/

#pragma once
#include "MenuContent.h"
class MeasureSettingMenuContentPrivate;
class MeasureSettingMenuContent : public MenuContent
{
    Q_OBJECT
public:
    MeasureSettingMenuContent();
    ~MeasureSettingMenuContent();

protected:
    /* reimplement */
    void layoutExec();

    /* reimplement */
    void readyShow();

private slots:
    /**
     * @brief onBtnReleasd
     */
    void onBtnReleasd(void);

private:
    MeasureSettingMenuContentPrivate *const d_ptr;
};
