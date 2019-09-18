/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/14
 **/

#pragma once
#include "MenuContent.h"

class O2MenuContentPrivate;
class O2MenuContent : public MenuContent
{
    Q_OBJECT
public:
    O2MenuContent();
    ~O2MenuContent();

    /**
     * @brief layoutExec
     */
    virtual void layoutExec();

    /**
     * @brief readyShow
     */
    virtual void readyShow();

private slots:
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

private:
    O2MenuContentPrivate *const d_ptr;
};
