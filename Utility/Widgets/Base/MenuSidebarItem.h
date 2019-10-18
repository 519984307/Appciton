/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/6
 **/

#pragma once
#include<QAbstractButton>

class MenuSidebarItem : public QAbstractButton
{
    Q_OBJECT
public:
    explicit MenuSidebarItem(QWidget *parent = NULL);

    QSize sizeHint() const;


signals:
    void focusChanged(bool in, Qt::FocusReason reason);

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);

    /* reimplement */
    void nextCheckState();

    /* reimplement */
    void focusInEvent(QFocusEvent *ev);

    /* reimplement */
    void focusOutEvent(QFocusEvent *ev);

    /* reimplement */
    void keyPressEvent(QKeyEvent *ev);

    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);

    void mousePressEvent(QMouseEvent *e);
};
