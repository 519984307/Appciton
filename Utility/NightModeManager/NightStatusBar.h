/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/20
 **/

#pragma once
#include "IWidget.h"

class NightStatusBarPrivate;

enum nightMode
{
    NIGHT_MODE_OFF = 0,
    NIGHT_MODE_ON,
    NIGHT_MODE_NR
};

class NightStatusBar : public IWidget
{
public:
    static NightStatusBar &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new NightStatusBar();
        }
        return *_selfObj;
    }
    static NightStatusBar *_selfObj;
    ~NightStatusBar();

public:
    /**
     * @brief changeIcon    改变图标
     * @param status
     */
    void changeIcon(nightMode status);

private:
    NightStatusBar();
    NightStatusBarPrivate *const d_ptr;
};

#define nightStatusBar (NightStatusBar::construction())
