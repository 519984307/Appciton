/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/7
 **/

#include "EventTrendItemDelegate.h"
#include "QStyleOptionViewItem"
#include "FontManager.h"
#include <QPainter>

#define MARGIN 4

#define ALARM_ITEM_BG_COLOR (Qt::white)

EventTrendItemDelegate::EventTrendItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void EventTrendItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    bool alarm = index.data(TrendAlarmRole).toBool();

    QColor textColor = opt.palette.color(QPalette::Text);


    // fill background if alarm
    if (alarm)
    {
        painter->fillRect(opt.rect, ALARM_ITEM_BG_COLOR);
    }

    painter->setPen(textColor);

    QFont font;
    QVariant value;
    QRect textRect = opt.rect.adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN);

    // draw value text
    QString text = index.data(ValueTextRole).toString();
    value = index.data(ValueFontRole);
    if (value.isValid())
    {
        font = qvariant_cast<QFont>(value);
    }
    else
    {
        value  = opt.font;
    }
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignRight, text);

    // draw title
    value = index.data(TitleFontRole);
    if (value.isValid())
    {
        font = qvariant_cast<QFont>(value);
    }
    else
    {
        font = opt.font;
    }
    text = index.data(TitleTextRole).toString();
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignTop | Qt::AlignLeft, text);

    textRect.adjust(0, fontManager.textHeightInPixels(font), 0, 0);
    value = index.data(UnitFontRole);
    if (value.isValid())
    {
        font = qvariant_cast<QFont>(value);
    }
    else
    {
        font = opt.font;
    }
    text = index.data(UnitTextRole).toString();
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignTop | Qt::AlignLeft, text);
}

QSize EventTrendItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid())
    {
        return qvariant_cast<QSize>(value);
    }

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    QFont valueFont = opt.font;
    QFont titleFont = opt.font;
    QFont unitFont = opt.font;

    value = (index.data(ValueFontRole));
    if (value.isValid())
    {
        valueFont = qvariant_cast<QFont>(value);
    }

    value = (index.data(TitleFontRole));
    if (value.isValid())
    {
        titleFont = qvariant_cast<QFont>(value);
    }

    value = (index.data(UnitFontRole));
    if (value.isValid())
    {
        unitFont = qvariant_cast<QFont>(value);
    }

    int height = fontManager.textHeightInPixels(valueFont);
    int titleHeight = fontManager.textHeightInPixels(titleFont);
    int unitHeight = fontManager.textHeightInPixels((unitFont));

    if (height < titleHeight + unitHeight)
    {
        height = titleHeight + unitHeight;
    }

//    height += 2 * MARGIN;
    height += 12 * MARGIN;

    QString valueText = index.data(ValueTextRole).toString();
    QString unitText = index.data(UnitTextRole).toString();
    QString titleText = index.data(TitleTextRole).toString();

    int unitTextWidth  = fontManager.textWidthInPixels(unitText, unitFont);
    int titleTextWidth = fontManager.textWidthInPixels(titleText, titleFont);

    int width = unitTextWidth > titleTextWidth ? unitTextWidth : titleTextWidth;

    width +=  fontManager.textWidthInPixels(valueText, valueFont) + 2 * MARGIN;

    return QSize(width, height);
}
