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

class ListsViewPrivate
{
public:
    ListsViewPrivate() {}
};

ListView::ListView(QWidget *parent)
    : QListView(parent),
      d_ptr(new ListsViewPrivate())
{
}

ListView::~ListView()
{
    delete d_ptr;
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
