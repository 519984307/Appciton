/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/10
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class AGMenuContentPrivate;
class AGMenuContent : public MenuContent
{
    Q_OBJECT
public:
    AGMenuContent();
    ~AGMenuContent();

protected:
    /* reimplement */
    virtual void readyShow();

    /* reimplement */
    virtual void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);
private:
    AGMenuContentPrivate * const d_ptr;
};
