/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#pragma once
#include <QItemDelegate>

class ScreenLayoutItemDelegatePrivate;
class ScreenLayoutItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ScreenLayoutItemDelegate(QObject *parent = NULL);
    ~ScreenLayoutItemDelegate();

protected:
    /* reimplement */
    void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;

    /* reimplement */
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const;

private:
    ScreenLayoutItemDelegatePrivate * const d_ptr;
};
