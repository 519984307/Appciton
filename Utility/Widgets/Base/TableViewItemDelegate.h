/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/11
 **/

#pragma once

#include <QItemDelegate>

class TableViewItemDelegatePrivate;
class TableViewItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TableViewItemDelegate(QObject *parent = NULL);
    ~TableViewItemDelegate();

    /* reimplement */
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    /* reimplement */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    /* reimplement */
    void drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, Qt::CheckState state) const;
    /* reimplement */
    void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
    /* reimplement */
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const;
    /* reimplement */
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    TableViewItemDelegatePrivate *const d_ptr;
};
