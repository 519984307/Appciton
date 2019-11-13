/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/

#pragma once

#include <QAbstractButton>

class PopupListItemPrivate;
class PopupListItem : public QAbstractButton
{
    Q_OBJECT
public:
    explicit PopupListItem(const QString &text, QWidget *parent = NULL);
    ~PopupListItem();

    QSize sizeHint() const;

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *e);

    /* reimplement */
    void nextCheckState();

    /* reimplement */
    void keyPressEvent(QKeyEvent *e);

    /* reimplement */
    void keyReleaseEvent(QKeyEvent *e);

    void mousePressEvent(QMouseEvent *e);

private:
    QScopedPointer<PopupListItemPrivate> d_ptr;
};
