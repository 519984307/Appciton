/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/13
 **/

#include "TrendWave.h"
#include <QPaintEvent>
#include <QPainter>
#include "BaseDefine.h"
#include "TrendDataStorageManager.h"
#include "TimeDate.h"
#include "RingBuff.h"
#include "TrendCache.h"
#include "FontManager.h"
#include <QDebug>
#include "AlarmConfig.h"
#include "ParamInfo.h"

#define DELTA_X 5   // 两值间的x轴像素值
typedef QList<float> YAxisValueBufType;

class TrendWavePrivate
{
public:
    explicit TrendWavePrivate(TrendWave *const q_ptr)
        : q_ptr(q_ptr),
          waveMargin(10, 2, 2, 2),
          waveColor(Qt::white),
          resetPointBufFlag(true),
          updateBGFlag(true),
          maxValue(-1),
          minValue(-1),
          scale(1)
    {}
    ~TrendWavePrivate(){}
    TrendWave * const q_ptr;
    QRect waveRegion;
    QMargins waveMargin;
    QColor waveColor;
    bool resetPointBufFlag;
    bool updateBGFlag;
    QList<SubParamID> subParamList;
    QList<YAxisValueBufType> yAxisValueBufs;
    int maxValue;
    int minValue;
    short scale;
    QPixmap background;

    /**
     * @brief drawWave 绘画波形
     * @param painter
     * @param r 需要被更新的区域
     */
    void drawWave(QPainter *painter, const QRect &r);

    /**
     * @brief resetPointBuffer
     */
    void resetPointBuffer();

    void updateBackground();

    /**
     * @brief getPointNum 返回一屏数据量
     * @return
     */
    int getPointNum();
};

TrendWave::TrendWave(const QString &name, QWidget *parent)
    : IWidget(name, parent),
      d_ptr(new TrendWavePrivate(this))
{
    setFocusPolicy(Qt::NoFocus);
    QPalette pal = this->palette();
    pal.setColor(QPalette::Background, Qt::black);
    connect(&trendDataStorageManager, SIGNAL(newTrendDataArrived(unsigned)), this, SLOT(onNewTrendDataArrived(unsigned)));
}

TrendWave::~TrendWave()
{
    delete d_ptr;
}

void TrendWave::addSubParam(SubParamID id)
{
    d_ptr->subParamList.append(id);
    d_ptr->yAxisValueBufs.append(YAxisValueBufType());
    updateRange();
}

void TrendWave::setColor(QColor color)
{
    d_ptr->waveColor = color;
    update();
}

void TrendWave::updateRange()
{
    for (int i = 0; i < d_ptr->subParamList.count(); i++)
    {
        LimitAlarmConfig limit = alarmConfig.getLimitAlarmConfig(d_ptr->subParamList.at(i)
                                                                 , paramInfo.getUnitOfSubParam(d_ptr->subParamList.at(i)));
        if (d_ptr->maxValue == -1 || limit.highLimit > d_ptr->maxValue)
        {
            d_ptr->maxValue = limit.highLimit;
        }
        if (d_ptr->minValue == -1 || limit.lowLimit < d_ptr->minValue)
        {
            d_ptr->minValue = limit.lowLimit;
        }
        if (d_ptr->scale < limit.scale)
        {
            d_ptr->scale = limit.scale;
        }
    }
}

void TrendWave::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    d_ptr->waveRegion = this->rect().adjusted(d_ptr->waveMargin.left(),
                                              d_ptr->waveMargin.top(),
                                              -d_ptr->waveMargin.right(),
                                              -d_ptr->waveMargin.bottom());
    d_ptr->resetPointBufFlag = true;
    d_ptr->updateBGFlag = true;
}

void TrendWave::paintEvent(QPaintEvent *e)
{
    if (d_ptr->resetPointBufFlag)
    {
        d_ptr->resetPointBuffer();
        d_ptr->resetPointBufFlag = false;
    }

    if (d_ptr->updateBGFlag)
    {
        d_ptr->updateBackground();
        d_ptr->updateBGFlag = false;
    }

    QVector<QRect> rects = e->region().rects();
    QPainter painter(this);
    for (int i = 0; i < rects.count(); ++i)
    {
        const QRect &r = rects.at(i);
        painter.setPen(d_ptr->waveColor);
        painter.drawPixmap(r, d_ptr->background, r);
        d_ptr->drawWave(&painter, r);
    }
}

void TrendWave::onNewTrendDataArrived(unsigned timeStamp)
{
    if (d_ptr->getPointNum() == 0)
    {
        return;
    }
    TrendCacheData datas;
    trendCache.getTrendData(timeStamp, datas);
    for (int i = 0; i < d_ptr->subParamList.count(); i++)
    {
        TrendDataType data = datas.values.value(d_ptr->subParamList.at(i));
        YAxisValueBufType &buf = d_ptr->yAxisValueBufs[i];
        if (buf.count() == d_ptr->getPointNum())
        {
            buf.removeLast();
        }

        if (data != InvData() && data > d_ptr->minValue)
        {
            float yValue = d_ptr->waveRegion.top();
            if (data < d_ptr->maxValue)
            {
                yValue = d_ptr->waveRegion.bottom()
                                   - (data - d_ptr->minValue) * d_ptr->waveRegion.height()
                                   / (d_ptr->maxValue - d_ptr->minValue);
            }
            buf.append(yValue);
        }
        else
        {
            buf.append(0);
        }
    }


    update(d_ptr->waveRegion);
}

void TrendWavePrivate::drawWave(QPainter *painter, const QRect &r)
{
    if (yAxisValueBufs.isEmpty() || yAxisValueBufs.at(0).isEmpty())
    {
        return;
    }

    QRect interRect = r.intersected(waveRegion);
    if (interRect.isNull())
    {
        return;
    }
    int pointNum = yAxisValueBufs.at(0).count();
    int start = (interRect.left() - waveRegion.left()) / DELTA_X;
    if (start < 0)
    {
        start = 0;
    }
    int end = (interRect.right() - waveRegion.left() + DELTA_X) / DELTA_X;
    if (end >= pointNum)
    {
        end = pointNum -1;
    }

    int startIndex = start % pointNum;
    int endIndex = end % pointNum;

    for (int i = 0; i < yAxisValueBufs.count(); ++i)
    {
        QPainterPath path;
        int index = startIndex;
        QPointF lastPoint;
        bool moveTo = true;
        float xPos = waveRegion.left() + start * DELTA_X;

        while (index != endIndex)
        {
            const float &curYAxisValue = yAxisValueBufs[i][index];
            if (curYAxisValue <= 0)
            {
                if (!moveTo)
                {
                    path.lineTo(lastPoint);
                }

                index = (index + 1) % pointNum;
                xPos += DELTA_X;
                continue;
            }

            QPointF curPoint(xPos, curYAxisValue);

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
                else if (((index + 1) % pointNum) == endIndex)
                {
                    path.lineTo(curPoint);
                }
            }

            lastPoint = curPoint;

            index = (index + 1) % pointNum;
            xPos += DELTA_X;
        }
        painter->drawPath(path);
    }
}

void TrendWavePrivate::resetPointBuffer()
{
    for (int i = 0; i < yAxisValueBufs.count(); i++)
    {
        yAxisValueBufs[i].append(0);
    }
}

void TrendWavePrivate::updateBackground()
{
    const QPalette &pal = q_ptr->palette();
    background = QPixmap(q_ptr->size());
    background.fill(pal.color(QPalette::Window));

    QPainter painter(&background);
    QPen pen(Qt::SolidLine);
    pen.setColor(waveColor);

    // draw ruler
    pen.setStyle(Qt::DotLine);
    pen.setColor(waveColor);
    painter.setPen(pen);
    painter.drawLine(waveRegion.topLeft(), waveRegion.topRight());
    painter.drawLine(waveRegion.bottomLeft(), waveRegion.bottomRight());

    QFont font = fontManager.textFont(fontManager.getFontSize(2));
    painter.setFont(font);

    // draw limit
    QString maxStr, minStr;
    if (scale != 1)
    {
        maxStr = QString::number(maxValue / (scale * 1.0), 'f', 1);
        minStr = QString::number(minValue / (scale * 1.0), 'f', 1);
    }
    else
    {
        maxStr = QString::number(maxValue);
        minStr = QString::number(minValue);
    }
    int fontHeight = fontManager.textHeightInPixels(font);
    int fontWidget = fontManager.textWidthInPixels(maxStr, font);
    int drawTextX = waveRegion.left();
    int drawTextY = waveRegion.top();
    painter.drawText(QRect(drawTextX, drawTextY, fontWidget, fontHeight), maxStr);
    drawTextY = waveRegion.bottom() - fontHeight;
    fontWidget = fontManager.textWidthInPixels(minStr, font);
    painter.drawText(QRect(drawTextX, drawTextY, fontWidget, fontHeight), minStr);
}

int TrendWavePrivate::getPointNum()
{
    return waveRegion.width() / DELTA_X;
}
