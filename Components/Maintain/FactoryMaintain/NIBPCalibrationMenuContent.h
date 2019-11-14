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

#include "Framework/UI/MenuContent.h"

class Button;
class NIBPCalibrationMenuContentPrivate;
class NIBPCalibrationMenuContent : public MenuContent
{
    Q_OBJECT
public:
    NIBPCalibrationMenuContent();
    ~NIBPCalibrationMenuContent();
protected:
    /**
     * @brief readyShow
     */
    void showEvent(QShowEvent* e);
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);

    /**
     * @brief hideEvent
     * @param e
     */
    void hideEvent(QHideEvent *e);

    /**
     * @brief timerEvent
     * @param e
     */
    void timerEvent(QTimerEvent *e);
private slots:
    /**
     * @brief onBtnSlot  按钮槽函数
     */
    void onBtnSlot();

private:
    NIBPCalibrationMenuContentPrivate * const d_ptr;
};
