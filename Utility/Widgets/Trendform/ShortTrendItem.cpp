/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/26
 **/

#include "ShortTrendItem.h"
#include <QPixmap>


class ShortTrendItemPrivate
{
public:
    ShortTrendItemPrivate(const QString &name, ShortTrendItem * const q_ptr)
        : q_ptr(q_ptr), name(name), duration(ShortTrendItem::TREND_DURATION_60M), maxValue(100), minValue(0), scale(1),
          waveRegion(), waveMargin(40, 18, 0, 18),
          updateBGFlag(true), resetPointBufFlag(true)
    {}

    void updateBackground();

    void resetPointBuffer();

    ShortTrendItem * const q_ptr;
    QString name;
    QPixmap background;
    QList<SubParamID> subParams;
    ShortTrendItem::TrendDuration duration;
    int maxValue;
    int minValue;
    int scale;
    QRect waveRegion;
    QMargins waveMargin;
    bool updateBGFlag;
    bool resetPointBufFlag;
};

void ShortTrendItemPrivate::updateBackground()
{
}

void ShortTrendItemPrivate::resetPointBuffer()
{
}

ShortTrendItem::ShortTrendItem(const QString &name, QWidget *parent)
    :QWidget(parent), d_ptr(new ShortTrendItemPrivate(name, this))
{
}

ShortTrendItem::~ShortTrendItem()
{
    delete d_ptr;
}

QString ShortTrendItem::getTrendName() const
{
    return d_ptr->name;
}

void ShortTrendItem::setTrendName(const QString &name)
{
    if (d_ptr->name == name)
    {
        return;
    }

    d_ptr->name = name;
    d_ptr->updateBGFlag = true;
}

QList<SubParamID> ShortTrendItem::getSubParamList() const
{
    return d_ptr->subParams;
}

void ShortTrendItem::setSubParamList(const QList<SubParamID> subParams)
{
    d_ptr->subParams = subParams;
    d_ptr->resetPointBufFlag = true;
}

void ShortTrendItem::setValueRange(short max, short min, short scale)
{
    d_ptr->maxValue = max;
    d_ptr->minValue = min;
    d_ptr->scale = scale;
    d_ptr->updateBGFlag = true;
    d_ptr->resetPointBufFlag = true;
}

void ShortTrendItem::getValueRange(short &max, short &min, short &scale) const
{
    max = d_ptr->maxValue;
    min = d_ptr->minValue;
    scale = d_ptr->scale;
}

void ShortTrendItem::setTrendDuration(ShortTrendItem::TrendDuration duration)
{
    if (d_ptr->duration == duration)
    {
        return;
    }

    d_ptr->duration = duration;
    d_ptr->updateBGFlag = true;
    d_ptr->resetPointBufFlag = true;
}

ShortTrendItem::TrendDuration ShortTrendItem::getTrendDuration() const
{
    return d_ptr->duration;
}

void ShortTrendItem::paintEvent(QPaintEvent *ev)
{
    if (d_ptr->resetPointBufFlag)
    {
        d_ptr->resetPointBuffer();
        d_ptr->resetPointBufFlag = true;
    }

    if (d_ptr->updateBGFlag)
    {
        d_ptr->updateBackground();
        d_ptr->updateBGFlag = true;
    }
}

void ShortTrendItem::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);
    d_ptr->updateBGFlag = true;
    d_ptr->resetPointBufFlag = true;
}

