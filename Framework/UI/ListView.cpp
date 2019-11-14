/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blemd.cn>, 2018/7/18
 **/

#include "ListView.h"
#include <QKeyEvent>
#include <QFocusEvent>
#include "ThemeManager.h"
#include <QMap>

class ListsViewPrivate
{
public:
    explicit ListsViewPrivate(ListView *const q_ptr)
        : q_ptr(q_ptr),
          isMultiSelectMode(false),
          clickable(true),
          isDrawIcon(true)
    {}

    int findCheckedRow() const;

    void clearCheckedRow(int row);

    void clearAllCheckedRows();

    ListView *const q_ptr;

    bool isMultiSelectMode;
    bool clickable;

    QMap <int, bool> rowsSelectedMap;

    bool isDrawIcon;
};

int ListsViewPrivate::findCheckedRow() const
{
    QAbstractItemModel *m = q_ptr->model();

    if (m)
    {
        int rows = m->rowCount();

        for (int i = 0; i < rows; i++)
        {
            QModelIndex index = m->index(i, 0);
            QVariant value = m->data(index, Qt::DecorationRole);
            if (!qvariant_cast<QIcon>(value).isNull())
            {
                return i;
            }
        }
    }

    return -1;
}

void ListsViewPrivate::clearCheckedRow(int row)
{
    if (row < 0 || !q_ptr->model())
    {
        return;
    }

    QModelIndex index = q_ptr->model()->index(row, 0);
    q_ptr->model()->setData(index, QVariant(QIcon()), Qt::DecorationRole);
}

void ListsViewPrivate::clearAllCheckedRows()
{
    if (!q_ptr->model())
    {
        return;
    }

    for (int i = 0; i < q_ptr->model()->rowCount(); i++)
    {
        QModelIndex index = q_ptr->model()->index(i, 0);
        q_ptr->model()->setData(index, QVariant(QIcon()), Qt::DecorationRole);
        // clear all selected rows map
        rowsSelectedMap[index.row()] = false;
    }
}

ListView::ListView(QWidget *parent)
    : QListView(parent),
      d_ptr(new ListsViewPrivate(this))
{
    setFrameStyle(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFocusPolicy(Qt::NoFocus);

    setSpacing(4);

    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(onRowClicked(QModelIndex)));
}

ListView::~ListView()
{
    delete d_ptr;
}

int ListView::curCheckedRow() const
{
    return d_ptr->findCheckedRow();
}

void ListView::clearCheckRow()
{
    d_ptr->clearCheckedRow(d_ptr->findCheckedRow());
}

void ListView::clearAllCheckedRows()
{
    d_ptr->clearAllCheckedRows();
}

void ListView::setRowsSelectMode(bool isMultiMode)
{
    Q_UNUSED(isMultiMode)
    d_ptr->isMultiSelectMode = isMultiMode;
}

QMap<int, bool> ListView::getRowsSelectMap() const
{
    return d_ptr->rowsSelectedMap;
}

void ListView::setModel(QAbstractItemModel *model)
{
    if (model)
    {
        setFocusPolicy(Qt::StrongFocus);
    }
    QListView::setModel(model);
}

void ListView::setDrawIcon(bool isDrawIcon)
{
    d_ptr->isDrawIcon = isDrawIcon;
}

void ListView::setClickable(bool clickable)
{
    d_ptr->clickable = clickable;
}

void ListView::keyPressEvent(QKeyEvent *ev)
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
        QListView::keyPressEvent(ev);
        break;
    }
}

void ListView::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
    {
        QModelIndex  index = currentIndex();
        if (index.row() <= 0)
        {
            focusPreviousChild();
        }
        else
        {
            QModelIndex newIndex = model()->index(index.row() - 1, index.column());
            setCurrentIndex(newIndex);
        }
    }
    break;
    case Qt::Key_Right:
    case Qt::Key_Down:
    {
        QModelIndex index = currentIndex();
        if (index.row() >= model()->rowCount() - 1)
        {
            focusNextChild();
        }
        else
        {
            QModelIndex newIndex = model()->index(index.row() + 1, index.column());
            setCurrentIndex(newIndex);
        }
    }
    break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        QModelIndex index = currentIndex();
        if (!index.isValid())
        {
            break;
        }
        QVariant value = model()->data(index, Qt::DecorationRole);
        QIcon icon = qvariant_cast<QIcon>(value);
        if (!icon.isNull())
        {
            model()->setData(index, QVariant(QIcon()), Qt::DecorationRole);
            // record selected rows when is on multi select mode
            d_ptr->rowsSelectedMap[index.row()] = false;
            emit enterSignal();
            break;
        }
        if (!d_ptr->isMultiSelectMode)
        {
            // clear the row that already checked
            clearCheckRow();
        }
        else
        {
            // record selected rows when is on multi select mode
            d_ptr->rowsSelectedMap[index.row()] = true;
        }
        icon = themeManager.getIcon(ThemeManager::IconChecked, QSize(24, 24));
        // set current checked row
        if (d_ptr->isDrawIcon)
        {
            model()->setData(index, icon, Qt::DecorationRole);
        }
        else
        {
            model()->setData(index, QVariant(QIcon()), Qt::DecorationRole);
        }
        emit enterSignal();
    }
    break;
    default:
        QListView::keyReleaseEvent(ev);
        break;
    }
}

void ListView::paintEvent(QPaintEvent *ev)
{
    QListView::paintEvent(ev);
}

void ListView::focusInEvent(QFocusEvent *ev)
{
    if (ev->reason() == Qt::TabFocusReason || ev->reason() == Qt::ActiveWindowFocusReason)
    {
        setCurrentIndex(model()->index(0, 0));
    }
    else if (ev->reason() == Qt::BacktabFocusReason)
    {
        setCurrentIndex(model()->index(model()->rowCount() - 1, 0));
    }
}

void ListView::focusOutEvent(QFocusEvent *ev)
{
    Q_UNUSED(ev)
    clearSelection();
}

void ListView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    QModelIndex index = currentIndex();
    if (index.row() >= start && index.row() <= end)
    {
        clearSelection();
    }
    if (end - start + 1 == model()->rowCount())
    {
        setFocusPolicy(Qt::NoFocus);
    }
    QListView::rowsAboutToBeRemoved(parent, start, end);
}

void ListView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QListView::rowsInserted(parent, start, end);
    setFocusPolicy(Qt::StrongFocus);
}

void ListView::hideEvent(QHideEvent *ev)
{
    QListView::hideEvent(ev);
    clearAllCheckedRows();
}

void ListView::onRowClicked(QModelIndex index)
{
    if (!index.isValid() || !d_ptr->clickable)
    {
        return;
    }

    QVariant value = model()->data(index, Qt::DecorationRole);
    QIcon icon = qvariant_cast<QIcon>(value);
    if (!icon.isNull())
    {
        model()->setData(index, QVariant(QIcon()), Qt::DecorationRole);
        // record selected rows when is on multi select mode
        d_ptr->rowsSelectedMap[index.row()] = false;
        emit enterSignal();
        return;
    }
    if (!d_ptr->isMultiSelectMode)
    {
        // clear the row that already checked
        clearCheckRow();
    }
    else
    {
        // record selected rows when is on multi select mode
        d_ptr->rowsSelectedMap[index.row()] = true;
    }
    icon = themeManager.getIcon(ThemeManager::IconChecked, QSize(24, 24));

    // set current checked row

    if (d_ptr->isDrawIcon)
    {
        model()->setData(index, icon, Qt::DecorationRole);
    }
    else
    {
        model()->setData(index, QVariant(QIcon()), Qt::DecorationRole);
    }

    emit enterSignal();
}
