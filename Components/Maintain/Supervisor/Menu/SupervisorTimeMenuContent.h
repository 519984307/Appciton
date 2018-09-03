/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/18
 **/

#pragma once
#include "Window.h"

class SupervisorTimeMenuContentPrivate;
class SupervisorTimeMenuContent : public Window
{
    Q_OBJECT
public:
    SupervisorTimeMenuContent();
    ~SupervisorTimeMenuContent();

    /**
     * @brief readyShow
     */
    void readyShow();
    /**
     * @brief layoutExec
     */
    void layoutExec();

private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onSpinBoxValueChanged
     * @param value
     * @param scale
     */
    void onSpinBoxValueChanged(int value, int scale);

private:
    SupervisorTimeMenuContentPrivate * const d_ptr;
};
