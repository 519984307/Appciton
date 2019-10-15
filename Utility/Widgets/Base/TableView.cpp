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
#include <QDebug>

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

    /**
     * @brief findNextPreviousSelectableItem find the next or previous selectable item
     * @param curIndex the current item index
     * @param next  true for next, false for previous
     * @return the next or previous item index, it will return invalid index if the not found any item
     */
    QModelIndex findNextPreviousSelectableItem(QModelIndex curIndex, bool next)
    {
        QModelIndex index;
        if (curIndex.isValid())
        {
            const QAbstractItemModel *m = curIndex.model();
            int rowCount = m->rowCount();
            int columnCount =  m->columnCount();
            int itemCount = rowCount * columnCount;
            int pos = curIndex.row() * columnCount + curIndex.column();
            int downRange = 0;
            int upRange = itemCount;

            int downX = q_ptr->columnAt(1) + 1;
            int downY = q_ptr->rowAt(1) + 1;
            int upX = q_ptr->columnAt(q_ptr->viewport()->width() - 1) + 1;
            int upY = q_ptr->rowAt(q_ptr->viewport()->height() - 1) + 1;
            downRange = (downY - 1) * upX + downX - 1;
            if (q_ptr->hasNextPage())
            {
                upRange = (upY - 1) * upX + upX - 1;
            }

            int delta = next ? 1 : -1;

            pos += delta;
            while (pos >= downRange && pos <= upRange)
            {
                curIndex = m->index(pos / columnCount, pos % columnCount);
                if (m->flags(curIndex) & Qt::ItemIsSelectable)
                {
                    index = curIndex;
                    break;
                }
                pos += delta;
            }
        }
        return index;
    }

    /**
     * @brief findFirstLastSelectableItem find the first or the last
     * @model the model
     * @param first true for first, false for last
     * @return the first or last selectable item' index, return invalid index if not found
     */
    QModelIndex findFirstLastSelectableItem(QAbstractItemModel *model, bool first)
    {
        QModelIndex index;
        if (first)
        {
            index = model->index(q_ptr->rowAt(1),
                                 q_ptr->columnAt(1));
        }
        else
        {

            index = model->index(q_ptr->rowAt(q_ptr->viewport()->height() - 1),
                                 q_ptr->columnAt(q_ptr->viewport()->width() - 1));
            if (!index.isValid())
            {
                // if not find the item, focus the last item.
                index = model->index(model->rowCount() - 1, model->columnCount() - 1);
            }
        }

        if (!(model->flags(index) & Qt::ItemIsSelectable))
        {
            index = findNextPreviousSelectableItem(index, first);
        }
        return index;
    }

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
        if (index.isValid())  // 加入model逻辑索引的有效性判断
        {
            scrollTo(index, QAbstractItemView::PositionAtTop);
        }
        else
        {
            scrollToBottom();  // 发现无效逻辑索引时，翻到尾页
        }
    }
}

void TableView::scrollToPreviousPage()
{
    int row = rowAt(-1);
    if (row >= 0)
    {
        QModelIndex index = model()->index(row - 1, 0);
        if (index.isValid())  // 加入model逻辑索引的有效性判断
        {
            scrollTo(index, QAbstractItemView::PositionAtBottom);
        }
        else
        {
            scrollToTop();  // 发现无效逻辑索引时，翻到首页
        }
    }
}

void TableView::scrollToAssignedPage(int row)
{
    int eachPageRowCount = rowAt(viewport()->height() - 1) - rowAt(1) + 1;
    int assignedPage = row / eachPageRowCount;
    int curPage = rowAt(0) / eachPageRowCount;
    if (curPage == assignedPage)
    {
        // 若指定页面在当前页，则直接返回
        return;
    }

    QModelIndex index = model()->index(assignedPage * eachPageRowCount - 1, 0);
    scrollTo(index, QAbstractItemView::PositionAtTop);
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

void TableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);

    // load the item's row span
    int row = model->rowCount();
    int column = model->columnCount();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            QSize span = model->span(model->index(i, j));
            if (span.isValid())
            {
                setSpan(i, j, span.height(), span.width());
            }
        }
    }
    connect(model, SIGNAL(modelReset()), this, SLOT(onModelReset()));
}

void TableView::getPageInfo(int &curPage, int &totalPage)
{
    int eachPageRowCount = rowAt(viewport()->height() - 1) - rowAt(1) + 1;
    if (model()->rowCount() % eachPageRowCount)
    {
        totalPage = model()->rowCount() / eachPageRowCount + 1;
    }
    else
    {
        totalPage = model()->rowCount() / eachPageRowCount;
    }
    curPage = rowAt(1) / eachPageRowCount + 1;
}

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
            if (index.isValid() && nextIndex.data().isValid())
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
            QModelIndex preIndex = d_ptr->findNextPreviousSelectableItem(index, false);
            if (preIndex.isValid() && preIndex.data().isValid())
            {
                setCurrentIndex(preIndex);
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
                if (rect().contains(r) && nextIndex.data().isValid())
                {
                    selectRow(nextRow);
                    emit selectRowChanged(nextRow);
                    break;
                }
            }
        }
        else if (selectionBehavior() == QAbstractItemView::SelectItems)
        {
            QModelIndex nextIndex = d_ptr->findNextPreviousSelectableItem(index, true);
            if (nextIndex.isValid() && nextIndex.data().isValid())
            {
                setCurrentIndex(nextIndex);
                break;
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
            emit itemClicked(index);
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
    if (!d_ptr->findFirstLastSelectableItem(model(), true).isValid())
    {
        // 如果tableview里没有可以聚焦的item，则聚焦下一个控件
        if (ev->reason() == Qt::TabFocusReason)
        {
            focusNextChild();
        }
        else if (ev->reason() == Qt::BacktabFocusReason)
        {
            focusPreviousChild();
        }
        return;
    }
    if (ev->reason() == Qt::TabFocusReason)
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
            setCurrentIndex(d_ptr->findFirstLastSelectableItem(model(), true));
        }
    }
    else if (ev->reason() == Qt::BacktabFocusReason)
    {
        if (this->selectionBehavior() == QAbstractItemView::SelectRows)
        {
            int row = rowAt(viewport()->rect().height() - 1);
            QModelIndex index = model()->index(row, 0);
            if (row >= 0 && index.data().isValid())
            {
                selectRow(row);
            }
            else if (model() && model()->rowCount() > 0)
            {
                int rows = model()->rowCount();
                int realRowCount = 0;
                for (int i = 0; i< rows; i++)
                {
                    QModelIndex focusIndex = model()->index(i, 0);
                    if (focusIndex.data().isValid())
                    {
                        realRowCount++;
                    }
                }
                selectRow(realRowCount - 1);
            }
        }
        else if (this->selectionBehavior() == QAbstractItemView::SelectItems)
        {
            setCurrentIndex(d_ptr->findFirstLastSelectableItem(model(), false));
        }
    }
    else if (ev->reason() == Qt::PopupFocusReason || ev->reason() == Qt::MouseFocusReason
             || ev->reason() == Qt::ActiveWindowFocusReason)
    {
        if (d_ptr->lastIndex.isValid())
        {
            setCurrentIndex(d_ptr->lastIndex);
        }
        else if (!currentIndex().isValid())
        {
            // focus the first item or the first row
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
                setCurrentIndex(d_ptr->findFirstLastSelectableItem(model(), true));
            }
        }
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

void TableView::onSpanChanged(const QModelIndex &index)
{
    QSize span = this->model()->span(index);
    if (span.isValid())
    {
        setSpan(index.row(), index.column(), span.height(), span.width());
    }
}

void TableView::onModelReset()
{
    if (!this->hasFocus())
    {
        return;
    }
    QModelIndex index = indexAt(QPoint(0, 0));
    QVariant var = model()->data(index, Qt::DisplayRole);
    if (var.isValid())
    {
        // model复位后，设置焦点聚焦到第一个index上。
        setCurrentIndex(index);
    }
    else
    {
        // 如果第一个index无效，即认为table view没有数据，则聚焦下一个控件
        focusNextPrevChild(true);
    }
}
