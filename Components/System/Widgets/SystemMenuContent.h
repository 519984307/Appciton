/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#pragma once
#include "MenuContent.h"

class SystemMenuContentPrivate;
class SystemMenuContent : public MenuContent
{
    Q_OBJECT
public:
    SystemMenuContent();
    ~SystemMenuContent();

    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);

private:
    SystemMenuContentPrivate * const d_ptr;
};
