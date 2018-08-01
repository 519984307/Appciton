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
#include "MenuContent.h"

class SupervisorTimeMenuContentPrivate;
class SupervisorTimeMenuContent : public MenuContent
{
    Q_OBJECT
public:
    SupervisorTimeMenuContent();
    ~SupervisorTimeMenuContent();

    void readyShow();

    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);
    void onSpinBoxValueChanged(int value, int scale);

private:
    SupervisorTimeMenuContentPrivate * const d_ptr;
};
