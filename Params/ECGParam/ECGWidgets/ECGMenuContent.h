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
#include "Framework/UI/MenuContent.h"
#include "SoundManager.h"

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

    void hideEvent(QHideEvent *e);

    void timerEvent(QTimerEvent *e);

    /* reimplement */
    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);

#ifndef HIDE_ECG_ARRHYTHMIA_FUNCTION
    /**
     * @brief arrhythmiaBtnReleased
     */
    void arrhythmiaBtnReleased(void);
#endif
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

    /**
     * @brief onPopupListItemFocusChanged
     * @param volume
     */
    void onPopupListItemFocusChanged(int volume);

private:
    ECGMenuContentPrivate * const d_ptr;
};
