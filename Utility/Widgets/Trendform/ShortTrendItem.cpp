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
#include <QPainter>
#include "SystemManager.h"
#include "FontManager.h"
#include "Utility.h"
#include <QVector>
#include "TrendDataStorageManager.h"
#include <QPaintEvent>

#define PREFER_ROW_HEIGHT 4     // mm
#define COLUMN_COUNT  4
#define FONT_HEIGHT 12


typedef QVector<QPointF> PointBufType;

class ShortTrendItemPrivate
{
public:
    ShortTrendItemPrivate(const QString &name, ShortTrendItem *const q_ptr)
        : q_ptr(q_ptr), name(name), startPos(0), duration(ShortTrendItem::TREND_DURATION_60M),
          maxValue(100), minValue(0), scale(1), waveRegion(), waveMargin(40, 8, 2, 8), waveColor(Qt::white),
          updateBGFlag(true), resetPointBufFlag(true), drawTimeLabelFlag(false), isNibp(false)
    {
    }

    static int getPreferRowHeight()
    {
        static int preferHeight = 0;
        if (preferHeight == 0)
        {
            preferHeight = PREFER_ROW_HEIGHT / systemManager.getScreenPixelHPitch();
        }
        return preferHeight;
    }

    QStringList getTimeLabels()
    {
        QStringList list;
        switch (duration)
        {
        case ShortTrendItem::TREND_DURATION_30M:
            list << "-30M" << "-15M" << "0";
            break;
        case ShortTrendItem::TREND_DURATION_60M:
            list << "-60M" << "-30M" << "0";
            break;
        case ShortTrendItem::TREND_DURATION_120M:
            list << "-2H" << "-1H" << "0";
            break;
        case ShortTrendItem::TREND_DURATION_240M:
            list << "-4H" << "-2H" << "0";
            break;
        case ShortTrendItem::TREND_DURATION_480M:
            list << "-8H" << "-4H" << "0";
            break;
        default:
            break;
        }
        return list;
    }

    void updateBackground();

    void resetPointBuffer();

    void drawWave(QPainter *painter, const QRect &r);

    void drawNibpMark(QPainter  *painter, const QRect &r);

    ShortTrendInterval getShortTrendInterval()
    {
        switch (duration)
        {
        case ShortTrendItem::TREND_DURATION_120M:
            return SHORT_TREND_INTERVAL_20S;
        case ShortTrendItem::TREND_DURATION_240M:
            return  SHORT_TREND_INTERVAL_30S;
        case ShortTrendItem::TREND_DURATION_480M:
            return SHORT_TREND_INTERVAL_60S;
        case ShortTrendItem::TREND_DURATION_30M:
        case ShortTrendItem::TREND_DURATION_60M:
        default:
            break;
        }
        return SHORT_TREND_INTERVAL_10S;
    }

    int trendDurationToPointNum()
    {
        switch (duration)
        {
        case ShortTrendItem::TREND_DURATION_30M:
            return 30 * 60 / 10;
        case ShortTrendItem::TREND_DURATION_60M:
            return 60 * 60 / 10;
        case ShortTrendItem::TREND_DURATION_120M:
            return 120 * 60 / 20;
        case ShortTrendItem::TREND_DURATION_240M:
            return 240 * 60 / 30;
        case ShortTrendItem::TREND_DURATION_480M:
            return 480 * 60 / 60;
        default:
            break;
        }
        return 0;
    }

    ShortTrendItem *const q_ptr;
    QString name;
    QPixmap background;
    QList<SubParamID> subParams;
    QList<PointBufType> pointBufs;
    int startPos;
    ShortTrendItem::TrendDuration duration;
    int maxValue;
    int minValue;
    int scale;
    QRect waveRegion;
    QMargins waveMargin;
    QColor waveColor;
    bool updateBGFlag;
    bool resetPointBufFlag;
    bool drawTimeLabelFlag;
    bool isNibp;
};

void ShortTrendItemPrivate::updateBackground()
{
    const QPalette &pal = q_ptr->palette();
    background = QPixmap(q_ptr->size());
    background.fill(pal.color(QPalette::Window));

    QPainter p(&background);
    QFont font = fontManager.textFont(FONT_HEIGHT);

    int perferheight = getPreferRowHeight();
    int rowNum = waveRegion.height() / perferheight;
    float rowHeight = waveRegion.height() * 1.0 / rowNum;

    QPen pen(pal.color(QPalette::Window));
    pen.setStyle(Qt::DashLine);
    p.drawRect(waveRegion);

    // draw row
    for (int i = 1; i < rowNum; ++i)
    {
        float height = rowHeight * i;
        p.drawLine(QPointF(waveRegion.left(), height),
                   QPointF(waveRegion.right(), height));
    }

    // draw column
    float columnWidth = waveRegion.width() * 1.0 / COLUMN_COUNT;
    for (int i = 1; i < COLUMN_COUNT; ++i)
    {
        float width = columnWidth * i;
        p.drawLine(QPointF(width, waveRegion.top()),
                   QPointF(width, waveRegion.bottom()));
    }

    p.setFont(font);

    QRect textRect(0, 0, waveMargin.left(), FONT_HEIGHT);
    QPoint center = textRect.center();

    // draw the y axis label
    float step = (maxValue - minValue) * 1.0 /  rowNum;
    for (int i = 0; i <= rowNum; ++i)
    {
        int yPos = waveRegion.bottom() - (rowHeight * i - 0.5);
        center.setY(yPos);
        textRect.moveCenter(center);
        int value = step * i + minValue + 0.5;
        p.drawText(textRect, Qt::AlignVCenter | Qt::AlignRight, Util::convertToString(value, scale));
    }

    // draw the time label
    if (drawTimeLabelFlag)
    {
        QStringList labels = getTimeLabels();
        Q_ASSERT(labels.length() == 3);
        textRect.moveTo(waveRegion.bottomLeft());
        p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, labels.at(0));
        textRect.moveLeft(waveRegion.center().x() - textRect.width() / 2);
        p.drawText(textRect, Qt::AlignCenter, labels.at(1));
        textRect.moveRight(waveRegion.right());
        p.drawText(textRect, Qt::AlignVCenter | Qt::AlignRight, labels.at(2));
    }

    // draw the name label
    p.setPen(waveColor);
    QFontMetrics fm(font);
    int nameWidth = fm.width(name) + 2;
    textRect.setWidth(nameWidth);
    textRect.setHeight(rowHeight - 2);
    textRect.moveTo(waveRegion.center().x() - textRect.width() / 2,
                    waveRegion.top() + (rowHeight - textRect.height()) / 2);

    p.fillRect(textRect, pal.window());
    p.drawText(textRect, Qt::AlignVCenter, name);
}

void ShortTrendItemPrivate::resetPointBuffer()
{
    int pointNum = trendDurationToPointNum();
    ShortTrendInterval interval = getShortTrendInterval();

    TrendDataType values[SHORT_TREND_DATA_NUM];
    float deltaX = waveRegion.width() * 1.0 / pointNum;
    for (int i = 0; i < subParams.count(); ++i)
    {
        trendDataStorageManager.getShortTrendData(subParams.at(i), values, pointNum, interval);
        if (pointBufs.at(i).size() != pointNum)
        {
            pointBufs[i].resize(pointNum);
        }

        for (int j = 0; j < pointNum; ++j)
        {
            if (values[j] != InvData())
            {
                float xValue = deltaX * j + waveRegion.left();
                float yValue = waveRegion.bottom() - (values[j] - minValue) * waveRegion.height() / (maxValue - minValue);
                pointBufs[i][j] = QPointF(xValue, yValue);
            }
            else
            {
                pointBufs[i][j] = QPointF();
            }
        }
    }

    // reset start position
    startPos = 0;
}

void ShortTrendItemPrivate::drawWave(QPainter *painter, const QRect &r)
{
    if (pointBufs.isEmpty() || pointBufs.at(0).size() == 0)
    {
        return;
    }

    QRect interRect = r.intersect(waveRegion);

    int pointNum = pointBufs.at(0).size();
    float deltaX = waveRegion.width() * 1.0 / pointNum;
    int start = (interRect.left() - waveRegion.left()) / deltaX;
    if (start < 0)
    {
        start = 0;
    }

    int end = (interRect.right() - waveRegion.left() + deltaX) / deltaX;
    if (end >= pointNum)
    {
        end = pointNum - 1;
    }

    int startIndex = (startPos + start) % pointNum;
    int endIndex  =(startPos + end) % pointNum;

    for (int i = 0; i < pointBufs.count(); ++i)
    {
        QPainterPath path;
        int index = startIndex;
        QPointF lastPoint;
        bool moveTo = true;
        while (index != endIndex)
        {
            const QPointF &curPoint = pointBufs.at(i).at(index);
            if (curPoint.isNull())
            {
                if (!moveTo)
                {
                    path.lineTo(lastPoint);
                    moveTo = true;
                }
                continue;
            }

            if (moveTo)
            {
                path.moveTo(curPoint);
                moveTo = false;
            }
            else
            {
                if (lastPoint.y() != curPoint.y())
                {
                    path.lineTo(lastPoint);
                    path.lineTo(curPoint);
                }
            }

            lastPoint = curPoint;

            index = (index + 1) % pointNum;
        }
        painter->drawPath(path);
    }
}

void ShortTrendItemPrivate::drawNibpMark(QPainter *painter, const QRect &r)
{
    if (pointBufs.isEmpty() || pointBufs.size() != 3)
    {
        return;
    }

    QRect interRect = r.intersect(waveRegion);
    int pointNum = pointBufs.at(0).size();
    float deltaX = waveRegion.width() * 1.0 / pointNum;
    int start = (interRect.left() - waveRegion.left()) / deltaX;
    if (start < 0)
    {
        start = 0;
    }

    int end = (interRect.right() - waveRegion.left() + deltaX) / deltaX;
    if (end >= pointNum)
    {
        end = pointNum - 1;
    }

    int startIndex = (startPos + start) % pointNum;
    int endIndex  =(startPos + end) % pointNum;

    painter->save();

    painter->setClipRect(interRect);
    painter->setRenderHint(QPainter::Antialiasing);

    int index = startIndex;
    QPainterPath path;
    int tickLen = systemManager.getScreenPixelHPitch();
    while (index != endIndex)
    {
        const QPointF &curPoint = pointBufs.at(0).at(index);
        if (!curPoint.isNull())
        {
            qreal x = curPoint.x();
            qreal sys = curPoint.y();
            qreal dia = pointBufs.at(1).at(index).y();
            qreal map = pointBufs.at(2).at(index).y();

            // draw nibp symbol
            path.moveTo(x - tickLen / 2, sys - 0.866 * tickLen);
            path.lineTo(x, sys);
            path.lineTo(x + tickLen / 2, sys - 0.866 * tickLen);

            path.moveTo(x - tickLen / 2, dia + 0.866 * tickLen);
            path.lineTo(x, dia);
            path.lineTo(x + tickLen / 2, dia + 0.866 * tickLen);

            path.moveTo(x, sys);
            path.lineTo(x, dia);

            path.moveTo(x - tickLen / 2, map);
            path.lineTo(x + tickLen / 2, map);
        }

        index = (index + 1) % pointNum;
    }

    if (!path.isEmpty())
    {
        painter->drawPath(path);
    }

    painter->restore();
}

ShortTrendItem::ShortTrendItem(const QString &name, QWidget *parent)
    : QWidget(parent), d_ptr(new ShortTrendItemPrivate(name, this))
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
    d_ptr->pointBufs.clear();
    for (int i = 0; i < subParams.count(); ++i)
    {
        d_ptr->pointBufs.append(PointBufType());
    }
    update();
}

void ShortTrendItem::setValueRange(short max, short min, short scale)
{
    d_ptr->maxValue = max;
    d_ptr->minValue = min;
    d_ptr->scale = scale;
    d_ptr->updateBGFlag = true;
    d_ptr->resetPointBufFlag = true;
    update();
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

void ShortTrendItem::setWaveColor(const QColor &color)
{
    if (d_ptr->waveColor == color)
    {
        return;
    }

    d_ptr->waveColor = color;
    update();
}

QColor ShortTrendItem::getWaveColor() const
{
    return d_ptr->waveColor;
}

void ShortTrendItem::enableDrawingTimeLabel(bool enable)
{
    if (d_ptr->drawTimeLabelFlag == enable)
    {
        return;
    }

    d_ptr->drawTimeLabelFlag = true;

    if (enable)
    {
        d_ptr->waveMargin.setBottom(20);
    }
    else
    {
        d_ptr->waveMargin.setBottom(8);
    }

    d_ptr->waveRegion = this->rect().adjusted(d_ptr->waveMargin.left(),
                        d_ptr->waveMargin.top(),
                        -d_ptr->waveMargin.right(),
                        -d_ptr->waveMargin.bottom());
    d_ptr->updateBGFlag = true;
    d_ptr->resetPointBufFlag = true;
    update();
}

bool ShortTrendItem::isDrawingTimeLabel() const
{
    return d_ptr->drawTimeLabelFlag;
}

void ShortTrendItem::setNibpTrend(bool enable)
{
    if (d_ptr->isNibp == enable)
    {
        return;
    }
    d_ptr->isNibp = enable;
    update();
}

bool ShortTrendItem::isNibpTrend() const
{
    return d_ptr->isNibp;
}

void ShortTrendItem::onNewTrendDataArrived(ShortTrendInterval interval)
{
    if (d_ptr->getShortTrendInterval() == interval || d_ptr->subParams.count())
    {
        for (int i = 0; i < d_ptr->subParams.count(); ++i)
        {
            TrendDataType data = trendDataStorageManager.getLatestShortTrendData(d_ptr->subParams.at(i), interval);
            float yValue = d_ptr->waveRegion.bottom()
                    - (data - d_ptr->minValue) * d_ptr->waveRegion.height()
                    / (d_ptr->maxValue - d_ptr->minValue);
            d_ptr->pointBufs[i][d_ptr->startPos].ry() = yValue;
        }

        d_ptr->startPos = (d_ptr->startPos + 1) % d_ptr->pointBufs.at(0).count();

        update(d_ptr->waveRegion);
    }
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

    QVector<QRect> rects = ev->region().rects();
    QPainter painter(this);
    for (int i = 0; i< rects.count(); ++i)
    {
        const QRect &r = rects.at(i);

        // draw the background
        painter.drawPixmap(r, d_ptr->background, r);

        painter.setPen(d_ptr->waveColor);

        if (d_ptr->isNibp)
        {
            d_ptr->drawNibpMark(&painter, r);
        }
        else
        {
            // draw the wave
            d_ptr->drawWave(&painter, r);
        }
    }
}

void ShortTrendItem::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);
    d_ptr->waveRegion = this->rect().adjusted(d_ptr->waveMargin.left(),
                        d_ptr->waveMargin.top(),
                        -d_ptr->waveMargin.right(),
                        -d_ptr->waveMargin.bottom());
    d_ptr->updateBGFlag = true;
    d_ptr->resetPointBufFlag = true;
}

