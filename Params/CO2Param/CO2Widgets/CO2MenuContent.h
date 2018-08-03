/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once

#include "MenuContent.h"

class CO2MenuContentPrivate;
class CO2MenuContent: public MenuContent
{
    Q_OBJECT
public:
    static CO2MenuContent *getInstance();
    ~CO2MenuContent();
    /**
     * @brief readyShow
     */
    virtual void readyShow();
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();
private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onBtnReleasedChanged
     */
    void onBtnReleasedChanged();
private:
    CO2MenuContentPrivate * const d_ptr;
    CO2MenuContent();
};
