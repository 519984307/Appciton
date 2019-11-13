/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/11
 **/

#include "TableHeaderView.h"
#include <QPainter>

TableHeaderView::TableHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    // set to as minimum as possible to make the model sizehint role works
    setMinimumSectionSize(10);
    setClickable(false);
    setMovable(false);
}

void TableHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (!rect.isValid())
    {
        return;
    }

    QAbstractItemModel *m = model();
    if (!m)
    {
        return;
    }

    QBrush bgBrush(Qt::white);
    QBrush fgBrush(Qt::black);

    QStyleOptionHeader opt;
    initStyleOption(&opt);

    QVariant textAlignment = m->headerData(logicalIndex, orientation(), Qt::TextAlignmentRole);

    int alignment = Qt::Alignment(textAlignment.isValid()
                                  ? Qt::Alignment(textAlignment.toInt())
                                  : Qt::AlignCenter);

    QString text  = m->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();

    if (orientation() == Qt::Horizontal)
    {
        text = opt.fontMetrics.elidedText(text, Qt::ElideRight, rect.width() - 4);
    }

    QVariant foregroundBrush = m->headerData(logicalIndex, orientation(), Qt::ForegroundRole);
    if (foregroundBrush.canConvert<QBrush>())
    {
        fgBrush = qvariant_cast<QBrush>(foregroundBrush);
    }

    QVariant backgroundBrush = m->headerData(logicalIndex, orientation(), Qt::BackgroundRole);
    if (backgroundBrush.canConvert<QBrush>())
    {
        bgBrush = qvariant_cast<QBrush>(backgroundBrush);
    }

    painter->fillRect(rect, bgBrush);

    painter->setPen(fgBrush.color());
    painter->drawText(rect.adjusted(4, 4, -4, -4), alignment, text);
}

