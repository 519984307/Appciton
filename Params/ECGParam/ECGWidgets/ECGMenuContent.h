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

    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);
    void arrhythmiaBtnReleased(void);
    void selfLearnBtnReleased(void);

private:
    ECGMenuContentPrivate * const d_ptr;
};
