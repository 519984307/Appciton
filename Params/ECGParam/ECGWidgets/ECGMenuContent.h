/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/5
 **/

#pragma once
#include "MenuContent.h"

class ECGMenuContentPrivate;
class ECGMenuContent : public MenuContent
{
    Q_OBJECT
public:
    ECGMenuContent();
    ~ECGMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);
    void arrhythmiaBtnReleased(void);
    void selfLearnBtnReleased(void);
    /**
     * @brief onSTSwitchBtnReleased  ST开关触发槽函数
     */
    void onSTSwitchBtnReleased(void);
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

private:
    ECGMenuContentPrivate * const d_ptr;
};
