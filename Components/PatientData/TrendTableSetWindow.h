/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/25
 **/

#pragma once
#include "Framework/UI/Dialog.h"

class TrendTableSetWindowPrivate;
class TrendTableSetWindow : public Dialog
{
    Q_OBJECT
public:
    static TrendTableSetWindow &construction()
    {
        if (NULL == selfObj)
        {
            selfObj = new TrendTableSetWindow();
        }
        return *selfObj;
    }
    ~TrendTableSetWindow();

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void timeIntervalReleased(int);
    void trendGroupReleased(int);

private:
    TrendTableSetWindow();
    static TrendTableSetWindow *selfObj;

    QScopedPointer<TrendTableSetWindowPrivate> d_ptr;
};
#define trendTableSetWindow (TrendTableSetWindow::construction())
#define deleteTrendTableSetWindow() (delete TrendTableSetWindow::_selfObj)

