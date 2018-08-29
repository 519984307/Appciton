/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/11
 **/

#pragma once
#include "MenuContent.h"

class SPO2MenuContentPrivate;
class SPO2MenuContent : public MenuContent
{
    Q_OBJECT
public:
    SPO2MenuContent();
    ~SPO2MenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

private:
    SPO2MenuContentPrivate * const d_ptr;
};
