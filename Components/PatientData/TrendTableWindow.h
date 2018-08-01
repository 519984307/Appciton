/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/20
 **/

#pragma once
#include "Window.h"
#include "IBPDefine.h"
#include "TrendDataDefine.h"

class TrendTableWindowPrivate;
class TrendTableWindow : public Window
{
    Q_OBJECT
public:
    static TrendTableWindow &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new TrendTableWindow();
        }
        return *_selfObj;
    }
    ~TrendTableWindow();

public:
    void setTimeInterval(int t);

    void setTrendGroup(int g);

    void setHistoryDataPath(QString path);

    void setHistoryData(bool flag);

    void printTrendData(unsigned startTime, unsigned endTime);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void leftReleased(void);
    void rightReleased(void);
    void upReleased(void);
    void downReleased(void);
    void leftMoveEvent(void);
    void rightMoveEvent(void);
    void printWidgetRelease(void);
    void trendDataSetReleased(void);

private:
    TrendTableWindow();
    static TrendTableWindow *_selfObj;

    QScopedPointer<TrendTableWindowPrivate> d_ptr;
};
#define trendTableWindow (TrendTableWindow::construction())
#define deleteTrendTableWindow() (delete TrendTableWindow::_selfObj)

