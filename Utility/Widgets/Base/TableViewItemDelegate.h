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

class TableViewItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TableViewItemDelegate(QObject *parent = NULL);

protected:
    void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
};
