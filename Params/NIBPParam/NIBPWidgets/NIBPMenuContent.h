/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma  once

#include "MenuContent.h"

class NIBPMenuContentPrivate;
class NIBPMenuContent : public MenuContent
{
    Q_OBJECT
public:
    NIBPMenuContent();
    ~NIBPMenuContent();

protected:
    /**
     * @brief readyShow  //reimplment load settings
     */
    virtual void readyShow();
    /**
     * @brief layoutExec  //reimplment play layout
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
    void onBtnReleasedChanged(void);
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

    /**
     * @brief onSpinBoxReleased
     */
    void onSpinBoxReleased(int value);

    /**
     * @brief onCboItemFocusChanged combox item focus changed
     * @param index
     */
    void onCboItemFocusChanged(int index);

    /**
     * @brief onStatBtnStateChanged stat button state changed
     * @param true: start stat  false: stop stat
     */
    void onStatBtnStateChanged(bool);
private:
    NIBPMenuContentPrivate * const d_ptr;
};
