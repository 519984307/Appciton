/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/25
 **/

#pragma once
#include "Window.h"

class BigFontLayoutWindowPrivate;
class BigFontLayoutWindow :public Window
{
    Q_OBJECT
public:
    static BigFontLayoutWindow &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new BigFontLayoutWindow();
        }
        return *_selfObj;
    }
    static BigFontLayoutWindow *_selfObj;
    ~BigFontLayoutWindow();

private slots:
    void onButtonClicked();
private:
    BigFontLayoutWindow();
    BigFontLayoutWindowPrivate *const d_ptr;
};

#define bigFontLayoutWindow (BigFontLayoutWindow::construction())
