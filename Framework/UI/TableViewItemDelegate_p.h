/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/3
 **/

#pragma once

#include "TableViewItemDelegate.h"

class QTableView;
class TableViewItemDelegatePrivate
{
public:
    explicit TableViewItemDelegatePrivate(TableViewItemDelegate *const q_ptr)
        : q_ptr(q_ptr),
          curEditingModel(NULL)
    {
    }

    virtual ~TableViewItemDelegatePrivate() {}

    const QWidget *widget(const QStyleOptionViewItem &option) const;

    virtual bool showEditor(const QTableView *view, QAbstractItemModel *model, QModelIndex index);

    TableViewItemDelegate *const q_ptr;
    QModelIndex curPaintingIndex;   // record current painting item's index
    QModelIndex curEditingIndex;    // record current painting item's index
    QAbstractItemModel *curEditingModel;  // current editing model
    Qt::CheckState checkState;  // record current item's check state
    QPalette pal;   // palette to used when draw check state
};
