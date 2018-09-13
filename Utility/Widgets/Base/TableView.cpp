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
#include <QFocusEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QTimer>

class TableViewPrivate
{
public:
    explicit TableViewPrivate(TableView *const q_ptr)
        : q_ptr(q_ptr),
          mouseClickRow(-1),
          mouseSelectRowChanged(false)
    {}

    // focus the editable item in the row
    void focusTheEditableItem(int row);

    TableView *const q_ptr;
    int mouseClickRow;
    QModelIndex lastIndex;  // the last index that has been mouse press or key press
    bool mouseSelectRowChanged;
};

void TableViewPrivate::focusTheEditableItem(int row)
{
    int columnCount = q_ptr->model()->columnCount();
    for (int i = 0; i < columnCount; ++i)
    {
        QModelIndex index = q_ptr->model()->index(row, i);
        if (index.isValid() && (q_ptr->model()->flags(index) & Qt::ItemIsEditable))
        {
            q_ptr->setCurrentIndex(index);
            lastIndex = index;
            break;
        }
    }
}

TableView::TableView(QWidget *parent)
    : QTableView(parent), d_ptr(new TableViewPrivate(this))
{
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAutoScroll(false);
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    setTabKeyNavigation(false);
    setEditTriggers(QAbstractItemView::SelectedClicked);
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
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        QModelIndex index = currentIndex();
        if (index.isValid() && (model()->flags(index) & Qt::ItemIsEditable))
        {
            d_ptr->lastIndex = index;
            edit(index, EditKeyPressed, ev);
        }
    }
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
        if (selectionBehavior() == QAbstractItemView::SelectRows)
        {
            int column = index.column();
            if (column > 0)
            {
                column -= 1;
                QModelIndex itemIndex = model()->index(index.row(), column);
                Qt::ItemFlags itemFlags = model()->flags(itemIndex);
                if (itemFlags & Qt::ItemIsEditable)
                {
                    setCurrentIndex(itemIndex);
                    break;
                }
            }
            int nextRow = index.row() - 1;
            QModelIndex nextIndex = model()->index(nextRow, index.column());
            if (index.isValid())
            {
                QRect r = visualRect(nextIndex);
                if (rect().contains(r))
                {
                    selectRow(nextRow);
                    emit selectRowChanged(nextRow);
                    break;
                }
            }
        }
        else if (selectionBehavior() == QAbstractItemView::SelectItems)
        {
            QAbstractItemModel *m = model();
            if (m)
            {
                int newRow = index.row();
                int newColumn = index.column();
                QModelIndex newIndex;
                if ( newColumn > 0)
                {
                    newColumn -= 1;
                    newIndex = m->index(newRow, newColumn);
                    setCurrentIndex(newIndex);
                    break;
                }
                else if (newRow > 0)
                {
                    newColumn = m->columnCount() - 1;
                    newRow -= 1;
                    newIndex = m->index(newRow, newColumn);
                    setCurrentIndex(newIndex);
                    break;
                }
            }
        }

        focusPreviousChild();
    }
    break;
    case Qt::Key_Right:
    case Qt::Key_Down:
    {
        QModelIndex index = currentIndex();
        if (selectionBehavior() == QAbstractItemView::SelectRows)
        {
            int column = index.column();
            if (column < model()->columnCount() - 1)
            {
                column += 1;
                QModelIndex itemIndex = model()->index(index.row(), column);
                Qt::ItemFlags itemFlags = model()->flags(itemIndex);
                if (itemFlags & Qt::ItemIsEditable)
                {
                    setCurrentIndex(itemIndex);
                    break;
                }
            }
            int nextRow = index.row() + 1;
            int lastRow = rowAt(viewport()->height() - 1);
            if (lastRow == -1)
            {
                // items are not filled the viewport
                lastRow = model()->rowCount() - 1;
            }
            if (nextRow <= lastRow)
            {
                QModelIndex nextIndex = model()->index(nextRow, index.column());
                QRect r = visualRect(nextIndex);
                if (rect().contains(r))
                {
                    selectRow(nextRow);
                    emit selectRowChanged(nextRow);
                    break;
                }
            }
        }
        else if (selectionBehavior() == QAbstractItemView::SelectItems)
        {
            QAbstractItemModel *m = model();
            if (m)
            {
                int newRow = index.row();
                int newColumn = index.column();
                QModelIndex newIndex;
                if ( newColumn < m->columnCount() - 1)
                {
                    newColumn += 1;
                    newIndex = m->index(newRow, newColumn);
                    setCurrentIndex(newIndex);
                    break;
                }
                else if (newRow < m->rowCount() - 1)
                {
                    newColumn = 0;
                    newRow += 1;
                    newIndex = m->index(newRow, newColumn);
                    setCurrentIndex(newIndex);
                    break;
                }
            }
        }
        focusNextChild();
    }
    break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        QModelIndex index = currentIndex();
        if (index.isValid())
        {
            emit rowClicked(index.row());
            d_ptr->focusTheEditableItem(index.row());
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
            if (!(model()->flags(index) & Qt::ItemIsSelectable))
            {
                // do nothing if the item  is unselectable
                ev->accept();
                return;
            }

            if (d_ptr->mouseClickRow != index.row())
            {
                d_ptr->mouseSelectRowChanged = true;
            }
            d_ptr->mouseClickRow = index.row();
            d_ptr->lastIndex = index;
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
            if (!(model()->flags(index) & Qt::ItemIsSelectable))
            {
                // do nothing if the item  is unselectable
                ev->accept();
                return;
            }

            if (index.row() == d_ptr->mouseClickRow)
            {
                if (d_ptr->mouseSelectRowChanged)
                {
                    emit selectRowChanged(d_ptr->mouseClickRow);
                    d_ptr->mouseSelectRowChanged = false;
                }
                emit rowClicked(d_ptr->mouseClickRow);
                if (!(model()->flags(index) & Qt::ItemIsEditable))
                {
                    d_ptr->focusTheEditableItem(d_ptr->mouseClickRow);
                }
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
        if (this->selectionBehavior() == QAbstractItemView::SelectRows)
        {
            int row = rowAt(1);
            if (row >= 0)
            {
                selectRow(row);
            }
        }
        else if (this->selectionBehavior() == QAbstractItemView::SelectItems)
        {
            QModelIndex index = currentIndex();
            if (!index.isValid())
            {
                index = model()->index(0, 0);
            }
            setCurrentIndex(index);
        }
    }
    else if (ev->reason() == Qt::BacktabFocusReason)
    {
        if (this->selectionBehavior() == QAbstractItemView::SelectRows)
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
        else if (this->selectionBehavior() == QAbstractItemView::SelectItems)
        {
            QModelIndex index = currentIndex();
            if (!index.isValid())
            {
                index = model()->index(model()->rowCount() - 1, model()->columnCount() - 1);
            }
            setCurrentIndex(index);
        }
    }
    else if (ev->reason() == Qt::PopupFocusReason || ev->reason() == Qt::MouseFocusReason)
    {
        setCurrentIndex(d_ptr->lastIndex);
    }
}

void TableView::focusOutEvent(QFocusEvent *ev)
{
    clearSelection();
    setCurrentIndex(QModelIndex());

    if (ev->reason() == Qt::MouseFocusReason
            || ev->reason() == Qt::TabFocusReason
            || ev->reason() == Qt::BacktabFocusReason)
    {
        QTimer::singleShot(0, this, SLOT(checkAfterFocusOut()));
    }
    QTableView::focusOutEvent(ev);
}

void TableView::checkAfterFocusOut()
{
    if (!hasFocus() && !QApplication::activePopupWidget())
    {
        // don't have focus and not any popup widget (popup widget might be the editor)
        emit selectRowChanged(-1);
    }
}
