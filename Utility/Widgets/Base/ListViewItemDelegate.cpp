/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/24
 **/

#include "ListViewItemDelegate.h"
#include <QPainter>
#include <QPixmap>

class ListViewItemDelegatePrivate
{
public:
    ListViewItemDelegatePrivate(){}
};

ListViewItemDelegate::ListViewItemDelegate(QObject *parent)
    :QItemDelegate(parent),
      d_ptr(new ListViewItemDelegatePrivate)
{
}

void ListViewItemDelegate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(rect)
    // do nothing
}

void ListViewItemDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const
{
    Q_UNUSED(option)
    if (!pixmap.isNull())
    {
        // premultiplied alpha channel
        QImage tmp(rect.size(), QImage::Format_ARGB32_Premultiplied);
        QPainter drawImagePainter(&tmp);
        drawImagePainter.setCompositionMode(QPainter::CompositionMode_Source);
        drawImagePainter.drawPixmap(0, 0, rect.width(), rect.height(), pixmap);
        drawImagePainter.end();

        painter->drawImage(rect, tmp);
    }
}
