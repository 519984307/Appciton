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
#include <QScrollArea>

class ScrollAreaPrivate;
class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit ScrollArea(QWidget *parent = NULL);
    ~ScrollArea();

protected:
    /* reimplement */
    void resizeEvent(QResizeEvent *ev);
    /* reimplement */
    void showEvent(QShowEvent *ev);
    /* reimplement */
    void scrollContentsBy(int dx, int dy);

    ScrollArea(ScrollAreaPrivate *d, QWidget *parent);

    QScopedPointer<ScrollAreaPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(ScrollArea)
};
