/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/28
 **/

#pragma once
#include "Window.h"

class ScreenLayoutWindowPrivate;
class ScreenLayoutWindow : public Window
{
    Q_OBJECT
public:
    /**
     * @brief getInstance get the window instance
     * @return
     */
    static ScreenLayoutWindow *getInstance();
    ~ScreenLayoutWindow();

private:
    ScreenLayoutWindow();
    ScreenLayoutWindowPrivate * const d_ptr;
};
