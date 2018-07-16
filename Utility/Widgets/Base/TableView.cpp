/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#include "TableView.h"
#include <QKeyEvent>
#include <QDebug>
#include <QFocusEvent>
#include <QMouseEvent>

class TableViewPrivate
{
public:
    explicit TableViewPrivate(TableView *const q_ptr)
        : q_ptr(q_ptr),
          mouseClickRow(-1)
    {}

    TableView *const q_ptr;
    int mouseClickRow;
};

TableView::TableView(QWidget *parent)
    : QTableView(parent), d_ptr(new TableViewPrivate(this))
{
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAutoScroll(false);
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    setTabKeyNavigation(false);
}

TableView::~TableView()
{
    delete d_ptr;
}

QSize TableView::sizeHint() const
{
    return QTableView::sizeHint();
}

void TableView::scrollToNextPage()
{
    int row = rowAt(viewport()->height() + 1);
    if (row >= 0)
    {
        QModelIndex index = model()->index(row, 0);
        scrollTo(index, QAbstractItemView::PositionAtTop);
    }
}

void TableView::scrollToPreviousPage()
{
    int row = rowAt(-1);
    if (row >= 0)
    {
        QModelIndex index = model()->index(row, 0);
        scrollTo(index, QAbstractItemView::PositionAtBottom);
    }
}

bool TableView::hasPreivousPage()
{
    int row  = rowAt(-1);
    return row >= 0;
}

bool TableView::hasNextPage()
{
    int row = rowAt(viewport()->height() + 1);
    return row >= 0;
}

// void TableView::mouseMoveEvent(QMouseEvent *ev)
// {
//     Q_UNUSED(ev)
//     // do nothing, disable the drag behavior
// }

void TableView::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        break;
    default:
        QTableView::keyPressEvent(ev);
        break;
    }
}

void TableView::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
    {
        QModelIndex index = currentIndex();
        int nextRow = index.row() - 1;
        QModelIndex nextIndex = model()->index(nextRow, index.column());
        if (index.isValid())
        {
            QRect r = visualRect(nextIndex);
            if (rect().contains(r))
            {
                selectRow(nextRow);
                break;
            }
        }
        focusPreviousChild();
    }
    break;
    case Qt::Key_Right:
    case Qt::Key_Down:
    {
        QModelIndex index = currentIndex();
        int nextRow = index.row() + 1;
        int lastRow = rowAt(viewport()->height() - 1);
        if (nextRow <= lastRow)
        {
            QModelIndex nextIndex = model()->index(nextRow, index.column());
            QRect r = visualRect(nextIndex);
            if (rect().contains(r))
            {
                selectRow(nextRow);
                break;
            }
        }
        focusNextChild();
    }
    break;
    case Qt::Key_Return:
    case Qt::Key_End:
    {
        QModelIndex index = currentIndex();
        if (index.isValid())
        {
            emit rowClicked(index.row());
        }
    }
    break;
    default:
        QTableView::keyReleaseEvent(ev);
        break;
    }
}

void TableView::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() && Qt::LeftButton)
    {
        QModelIndex index = indexAt(ev->pos());
        if (index.isValid())
        {
            d_ptr->mouseClickRow = index.row();
        }
    }
    QTableView::mousePressEvent(ev);
}

void TableView::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() && Qt::LeftButton)
    {
        QModelIndex index = indexAt(ev->pos());
        if (index.isValid())
        {
            if (index.row() == d_ptr->mouseClickRow)
            {
                emit rowClicked(d_ptr->mouseClickRow);
                d_ptr->mouseClickRow = -1;
            }
        }
    }
    QTableView::mouseReleaseEvent(ev);
}

void TableView::focusInEvent(QFocusEvent *ev)
{
    if (ev->reason() == Qt::TabFocusReason || ev->reason() == Qt::ActiveWindowFocusReason)
    {
        int row = rowAt(1);
        if (row >= 0)
        {
            selectRow(row);
        }
    }
    else if (ev->reason() == Qt::BacktabFocusReason)
    {
        int row = rowAt(viewport()->rect().height() - 1);
        if (row >= 0)
        {
            selectRow(row);
        }
        else if (model() && model()->rowCount() > 0)
        {
            selectRow(model()->rowCount() - 1);
        }
    }
}

void TableView::focusOutEvent(QFocusEvent *ev)
{
    clearSelection();
    setCurrentIndex(QModelIndex());
    QTableView::focusOutEvent(ev);
}
