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

class Supervisor12LMenuContentPrivate;
class Supervisor12LMenuContent : public MenuContent
{
    Q_OBJECT
public:
    Supervisor12LMenuContent();
    ~Supervisor12LMenuContent();

    /* reimplement */
    virtual void readyShow();

    /* reimplement */
    virtual void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);

private:
    Supervisor12LMenuContentPrivate * const d_ptr;
};
