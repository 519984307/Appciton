/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/24
 **/

#pragma once

#include <QItemDelegate>

class ListViewItemDelegatePrivate;
class ListViewItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ListViewItemDelegate(QObject *parent = NULL);

protected:
    /* reimplement */
    void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;

    /* reimplement */
    void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const;

private:
    ListViewItemDelegatePrivate * const d_ptr;
};
