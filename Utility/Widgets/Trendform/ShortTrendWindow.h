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
#include "Framework/UI/Dialog.h"

class ShortTrendWindowPrivate;
class ShortTrendContainer;
class ShortTrendWindow : public Dialog
{
    Q_OBJECT
public:
    explicit ShortTrendWindow(ShortTrendContainer *const trendContainer);
    ~ShortTrendWindow();

protected:
    /*reimplement*/
    void showEvent(QShowEvent *e);

private slots:
    void onBtnReleased();

private:
    ShortTrendWindowPrivate *const d_ptr;
};
