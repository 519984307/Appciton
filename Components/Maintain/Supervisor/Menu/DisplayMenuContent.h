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

class DisplayMenuContentPrivate;
class DisplayMenuContent : public MenuContent
{
    Q_OBJECT
public:
    DisplayMenuContent();
    ~DisplayMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);

private:
    DisplayMenuContentPrivate * const d_ptr;
};
