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

class ListsViewPrivate
{
public:
    explicit ListsViewPrivate(ListView *const q_ptr)
        : q_ptr(q_ptr)
    {}

    int findCheckedRow() const;

    void clearCheckedRow(int row);

    ListView *const q_ptr;
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

void ListView::setModel(QAbstractItemModel *model)
{
    if (model && model->rowCount())
    {
        setFocusPolicy(Qt::StrongFocus);
    }
    QListView::setModel(model);
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
        if (index.isValid())
        {
            QVariant value = model()->data(index, Qt::DecorationRole);
            QIcon icon = qvariant_cast<QIcon>(value);
            if (!icon.isNull())
            {
                model()->setData(index, QVariant(QIcon()), Qt::DecorationRole);
            }
            else
            {
                // clear the row that already checked
                clearCheckRow();
                QIcon icon = themeManger.getIcon(ThemeManager::IconChecked);
                // set current checked row
                model()->setData(index, icon, Qt::DecorationRole);
            }
        }
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

void ListView::onRowClicked(QModelIndex index)
{
    if (index.isValid())
    {
        QVariant value = model()->data(index, Qt::DecorationRole);
        QIcon icon = qvariant_cast<QIcon>(value);
        if (!icon.isNull())
        {
            model()->setData(index, QVariant(QIcon()), Qt::DecorationRole);
        }
        else
        {
            // clear the row that already checked
            clearCheckRow();
            QIcon icon = themeManger.getIcon(ThemeManager::IconChecked);
            // set current checked row
            model()->setData(index, icon, Qt::DecorationRole);
        }
    }
}
