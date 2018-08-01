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
