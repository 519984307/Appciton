/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/28
 **/

#include "ScreenLayoutWindow.h"

class ScreenLayoutWindowPrivate
{
public:
    ScreenLayoutWindowPrivate() {}
};

ScreenLayoutWindow *ScreenLayoutWindow::getInstance()
{
    static ScreenLayoutWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new ScreenLayoutWindow();
    }
    return instance;
}


ScreenLayoutWindow::ScreenLayoutWindow()
    : Window(), d_ptr(new ScreenLayoutWindowPrivate())
{
}

ScreenLayoutWindow::~ScreenLayoutWindow()
{
    delete d_ptr;
}
