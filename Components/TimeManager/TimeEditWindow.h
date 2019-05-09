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
#include "Dialog.h"

class TimeEditWindowPrivate;
class TimeEditWindow : public Dialog
{
    Q_OBJECT
public:
    TimeEditWindow();
    ~TimeEditWindow();

    /**
     * @brief readyShow
     */
    void readyShow();
    /**
     * @brief layoutExec
     */
    void layoutExec();

protected:
    void hideEvent(QHideEvent *ev);

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
    TimeEditWindowPrivate * const d_ptr;
};
