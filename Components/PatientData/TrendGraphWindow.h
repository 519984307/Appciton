/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/27
 **/

#pragma once
#include "Window.h"

class TrendGraphWindowPrivate;
class TrendGraphWindow : public Window
{
    Q_OBJECT
public:
    static TrendGraphWindow &construction()
    {
        if (NULL == selfObj)
        {
            selfObj = new TrendGraphWindow();
        }
        return *selfObj;
    }
    ~TrendGraphWindow();

private slots:
    void onButtonReleased(void);

private:
    static TrendGraphWindow *selfObj;
    TrendGraphWindow();
    QScopedPointer<TrendGraphWindowPrivate> d_ptr;
};
#define trendGraphWindow (TrendGraphWindow::construction())
#define deleteTrendGraphWindow() (delete TrendGraphWindow::selfObj)
