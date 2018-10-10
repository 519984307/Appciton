/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/9
 **/

#pragma once
#include "Window.h"

class MiniTrendWindowPrivate;
class ShortTrendContainer;
class MiniTrendWindow : public Window
{
    Q_OBJECT
public:
    explicit MiniTrendWindow(ShortTrendContainer * const trendContainer);
    ~MiniTrendWindow();

protected:
    /*reimplement*/
    void showEvent(QShowEvent *e);

private slots:
    void onBtnReleased();

private:
    MiniTrendWindowPrivate *const d_ptr;
};
