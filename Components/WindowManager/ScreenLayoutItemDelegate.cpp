/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#include "ScreenLayoutItemDelegate.h"

class ScreenLayoutItemDelegatePrivate
{
public:
    explicit ScreenLayoutItemDelegatePrivate(ScreenLayoutItemDelegate * const q_ptr)
        :q_ptr(q_ptr),
          curEditingModel(NULL)
    {
    }

    ScreenLayoutItemDelegate * const q_ptr;
    QAbstractItemModel *curEditingModel;
    QModelIndex curPaintingIndex;
    QModelIndex curEditingIndex;
};

ScreenLayoutItemDelegate::ScreenLayoutItemDelegate(QObject *parent)
    :QItemDelegate(parent), d_ptr(new ScreenLayoutItemDelegatePrivate(this))
{
}

ScreenLayoutItemDelegate::~ScreenLayoutItemDelegate()
{
    delete d_ptr;
}

void ScreenLayoutItemDelegate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(rect)
}

void ScreenLayoutItemDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const
{
}
