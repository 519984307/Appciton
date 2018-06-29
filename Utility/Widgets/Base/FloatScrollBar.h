/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/6/29
 **/

#pragma once
#include <QWidget>

class QBasicTimer;
class FloatScrollBar : public QWidget
{
    Q_OBJECT
public:
    explicit FloatScrollBar(QWidget *parent = NULL);
    ~FloatScrollBar();

    /* reimplement */
    void setVisible(bool visiable);

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);
    /* reimplement */
    void timerEvent(QTimerEvent *ev);
    /* reimplement */
    void showEvent(QShowEvent *ev);
    /* reimplement */
    void moveEvent(QMoveEvent *ev);

private:
    QBasicTimer *_timer;  // hide widget countdown timer
};
