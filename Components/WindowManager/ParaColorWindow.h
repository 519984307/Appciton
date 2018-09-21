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
#include "Window.h"

class ParaColorWindowPrivate;
class ParaColorWindow : public Window
{
    Q_OBJECT
public:
    ParaColorWindow();
    ~ParaColorWindow();

protected:
    void showEvent(QShowEvent *ev);

    /* reimplement */
    void layoutExec();

    void hideEvent(QHideEvent *ev);

private slots:
    void onComboBoxIndexChanged(int index);

private:
    ParaColorWindowPrivate * const d_ptr;
};
