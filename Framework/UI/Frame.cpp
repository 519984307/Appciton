/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/2
 **/



#include "Frame.h"
#include "FrameItem.h"
#include <QBoxLayout>
#include <QList>

class FramePrivate
{
public:
    FramePrivate(): layout(NULL) {}
    QBoxLayout *layout;
    QList<FrameItem *> items;
};

Frame::Frame(QWidget *parent)
    : QWidget(parent), d_ptr(new FramePrivate())
{
    setAutoFillBackground(false);
    d_ptr->layout = new QVBoxLayout(this);
    d_ptr->layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->layout->setSpacing(0);
    d_ptr->layout->addStretch(1);
    setContentsMargins(8, 8, 8, 8);
}

Frame::~Frame()
{
}

bool Frame::addItem(FrameItem *item)
{
    if (item == NULL || d_ptr->items.contains(item))
    {
        return false;
    }

    item->setParent(this);
    d_ptr->items.append(item);
    d_ptr->layout->insertWidget(d_ptr->items.count()-1, item);
    return true;
}

int Frame::itemCount() const
{
    return d_ptr->items.count();
}

int Frame::indexOf(FrameItem *item) const
{
    return d_ptr->items.indexOf(item);
}
