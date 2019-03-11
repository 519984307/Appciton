/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/19
 **/

#include "OxyCRGTrendWaveWidget.h"
#include "OxyCRGTrendWaveWidget_p.h"
#include <QPainter>
#include <QPainterPath>
#include "BaseDefine.h"
#include "FontManager.h"
#include "ThemeManager.h"
#include "ConfigManager.h"
#include "WaveWidgetLabel.h"
#include <QTimer>
#include <QScopedPointer>
#include <QResizeEvent>
#include "ConfigManager.h"

OxyCRGTrendWaveWidgetPrivate::OxyCRGTrendWaveWidgetPrivate(OxyCRGTrendWaveWidget * const q_ptr)
    : q_ptr(q_ptr),
      dataBuf(NULL),
      waveBuffer(NULL),
      name(""),
      rulerHigh(InvData()),
      rulerLow(InvData()),
      waveColor(Qt::green),
      waveDataRate(1),
      drawRuler(true),
      interval(OXYCRG_INTERVAL_2),
      pointGap(0),
      pointGapSumFraction(0.0)
{
    int index = OXYCRG_INTERVAL_2;
    currentConfig.getNumValue("OxyCRG|Interval", index);
    interval = static_cast<OxyCRGInterval>(index);

    int dataLen = waveDataRate * MAX_WAVE_DURATION * 60;  // 最大8分钟数据
    dataBuf = new RingBuff<short>(dataLen);
}

int OxyCRGTrendWaveWidgetPrivate::getIntervalSeconds(OxyCRGInterval interval)
{
    switch (interval) {
    case OXYCRG_INTERVAL_1:
        return 60;
    case OXYCRG_INTERVAL_2:
        return 120;
    case OXYCRG_INTERVAL_4:
        return 240;
    case OXYCRG_INTERVAL_8:
        return 480;
    default:
        break;
    }
    return 120;
}

void OxyCRGTrendWaveWidgetPrivate::drawWave(QPainter *painter, const QRect &waveRegion, const OxyCRGWaveBuffer *buffer, const QColor &waveColor)
{
    if (!painter || !waveRegion.isValid() || !buffer)
    {
        return;
    }

    painter->save();
    painter->setClipRect(waveRegion);
    painter->setPen(QPen(waveColor, 2, Qt::SolidLine));

    QPainterPath path;
    int curSegmementIndex = (buffer->newestSegmentIndex + 1) % buffer->segmentCount;
    float xPos = waveRegion.x();
    bool moveTo = true;
    QPoint lastPoint;
    while (true)
    {
        const OxyCRGWaveSegment &seg = buffer->segments[curSegmementIndex];

        if (seg.isValid())
        {
            if (seg.isPoint())
            {
                // draw point
                QPoint curPoint(xPos, seg.maxY);
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
            }
            else
            {
                // draw line
                if (!moveTo)
                {
                    path.lineTo(lastPoint);
                    moveTo = true;
                }

                path.moveTo(QPoint(xPos, seg.maxY));
                path.lineTo(QPoint(xPos, seg.minY));
            }
        }
        else
        {
            if (!moveTo)
            {
                // current point is not a valid point,
                // draw the latest valid point
                path.lineTo(lastPoint);
                moveTo = true;
            }
        }

        if (curSegmementIndex == buffer->newestSegmentIndex)
        {
            if (!moveTo)
            {
                // reach the end
                path.lineTo(lastPoint);
            }
            break;
        }

        if (pointGap > 1)
        {
            xPos += pointGap;
        }
        else
        {
            xPos += 1;
        }

        curSegmementIndex = (curSegmementIndex + 1) % buffer->segmentCount;
    }

    painter->drawPath(path);
    painter->restore();
}

void OxyCRGTrendWaveWidgetPrivate::updateWaveDrawingContext()
{
    int sampleCount = getIntervalSeconds(interval) * waveDataRate;

    int segmentCount = waveRegion.width();
    if (sampleCount < segmentCount)
    {
        segmentCount = sampleCount + 1;
    }

    if (!waveBuffer)
    {
        waveBuffer = new OxyCRGWaveBuffer(segmentCount);
    }
    else
    {
        waveBuffer->resize(segmentCount);
    }

    pointGap = 1.0 * waveRegion.width() / sampleCount;

    if (pointGap < 1)
    {
        waveBuffer->highDensitiy = true;
    }

    pointGapSumFraction = 0.0;
}

void OxyCRGTrendWaveWidgetPrivate::reloadWaveBuffer()
{
    if (!waveBuffer)
    {
        return;
    }

    waveBuffer->clear();
    pointGapSumFraction = 0.0;

    Q_ASSERT(rulerHigh != rulerLow);
    int datasize = dataBuf->dataSize();
    int requestDataSize = getIntervalSeconds(interval) * waveDataRate;
    int index = (datasize > requestDataSize) ? datasize - requestDataSize : 0;

    for (int i = index; i < datasize; ++i)
    {
        short value = dataBuf->at(i);
        short mapYValue;
        if (value == InvData())
        {
            mapYValue = 0;
        }
        else
        {
            mapYValue = waveRegion.bottom() -
                    (value - rulerLow) * waveRegion.height() / (rulerHigh - rulerLow);
        }

        if (pointGapSumFraction > 1.0)
        {
            waveBuffer->pushPointData(mapYValue, true);
            pointGapSumFraction -= static_cast<int>(pointGapSumFraction);
        }
        else
        {
            waveBuffer->pushPointData(mapYValue, false);
        }
        pointGapSumFraction += pointGap;
    }
    q_ptr->update();
}

OxyCRGTrendWaveWidgetPrivate::~OxyCRGTrendWaveWidgetPrivate()
{
    if (waveBuffer)
    {
        delete waveBuffer;
        waveBuffer = NULL;
    }

    if (dataBuf)
    {
        delete dataBuf;
        dataBuf = NULL;
    }
}

OxyCRGTrendWaveWidget::OxyCRGTrendWaveWidget(const QString &waveName,
                                             OxyCRGTrendWaveWidgetPrivate *p)
                     : IWidget(waveName),
                       d_ptr(p)
{
    setFocusPolicy(Qt::NoFocus);
}

OxyCRGTrendWaveWidget::~OxyCRGTrendWaveWidget()
{
    delete d_ptr;
}

void OxyCRGTrendWaveWidget::addWaveData(int value)
{
    d_ptr->dataBuf->push(value);

    if (!d_ptr->waveBuffer)
    {
        return;
    }

    Q_ASSERT(d_ptr->rulerHigh != d_ptr->rulerLow);
    d_ptr->pointGapSumFraction += d_ptr->pointGap;

    short mapYValue;
    if (value == InvData())
    {
        mapYValue = 0;
    }
    else
    {
        mapYValue = d_ptr->waveRegion.bottom() -
                (value - d_ptr->rulerLow) * d_ptr->waveRegion.height() / (d_ptr->rulerHigh - d_ptr->rulerLow);
    }

    if (d_ptr->pointGapSumFraction > 1.0)
    {
        d_ptr->waveBuffer->pushPointData(mapYValue, true);
        d_ptr->pointGapSumFraction -= static_cast<int>(d_ptr->pointGapSumFraction);
    }
    else
    {
        d_ptr->waveBuffer->pushPointData(mapYValue, false);
    }
}

void OxyCRGTrendWaveWidget::setInterval(OxyCRGInterval interval)
{
    if (d_ptr->interval == interval)
    {
        return;
    }

    d_ptr->interval = interval;
    d_ptr->updateWaveDrawingContext();
    d_ptr->reloadWaveBuffer();
}

OxyCRGInterval OxyCRGTrendWaveWidget::getInterval() const
{
    return d_ptr->interval;
}

void OxyCRGTrendWaveWidget::setRulerValue(int valueHigh, int valueLow)
{
    if (d_ptr->rulerHigh == valueHigh && d_ptr->rulerLow == valueLow)
    {
        return;
    }

    d_ptr->rulerHigh = valueHigh;
    d_ptr->rulerLow = valueLow;
    d_ptr->reloadWaveBuffer();
}

void OxyCRGTrendWaveWidget::clearData()
{
    d_ptr->dataBuf->clear();
    d_ptr->reloadWaveBuffer();
}

void OxyCRGTrendWaveWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    QRect r = rect();
    int w = r.width();
    int h = r.height();
    int xShift = X_SHIFT;
    int yShift = Y_SHIFT;
    int wxShift = WX_SHIFT;

    QPainter painter(this);
    // 添加边框
    painter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    painter.drawRect(r);

    // 添加波形名字
    painter.setPen(QPen(d_ptr->waveColor, 1, Qt::SolidLine));
    painter.setFont(fontManager.textFont(16));
    painter.drawText(xShift, yShift,
                     wxShift, h / 3,
                     Qt::AlignLeft|Qt::AlignTop,
                     d_ptr->name);

    // 添加标尺高低值
    if (d_ptr->drawRuler)
    {
        painter.setPen(QPen(d_ptr->waveColor, 1, Qt::SolidLine));
        painter.drawText(wxShift + xShift, yShift,
                         wxShift, wxShift / 2,
                         Qt::AlignTop | Qt::AlignLeft,
                         QString::number(d_ptr->rulerHigh));

        painter.setPen(QPen(d_ptr->waveColor, 1, Qt::SolidLine));
        painter.drawText(wxShift + xShift, h - yShift - wxShift / 2,
                         wxShift, wxShift / 2,
                         Qt::AlignBottom | Qt::AlignLeft,
                         QString::number(d_ptr->rulerLow));
    }

    if (d_ptr->backgroundRulerPath.isEmpty())
    {
        // 添加背景标尺虚线
        int xStep = qRound((w - wxShift * 2) * 1.0 / 4);
        for (int i = 1; i < 4; i++)
        {
            d_ptr->backgroundRulerPath.moveTo(wxShift + xStep * i, 0);
            d_ptr->backgroundRulerPath.lineTo(wxShift + xStep * i, h);
        }

        int yStep = qRound(h * 1.0 / 3);
        for (int i = 1; i < 3; i++)
        {
            d_ptr->backgroundRulerPath.moveTo(wxShift, yStep * i);
            d_ptr->backgroundRulerPath.lineTo(w - wxShift, yStep * i);
        }
    }

    painter.setPen(QPen(Qt::white, 1, Qt::DotLine));
    painter.drawPath(d_ptr->backgroundRulerPath);
}

void OxyCRGTrendWaveWidget::resizeEvent(QResizeEvent *e)
{
    IWidget::resizeEvent(e);

    // clear the background ruler
    d_ptr->backgroundRulerPath = QPainterPath();
    // update the wave region
    d_ptr->waveRegion = QRect(WX_SHIFT, Y_SHIFT,
                              width() - WX_SHIFT * 2,
                              height() - Y_SHIFT * 2);

    d_ptr->updateWaveDrawingContext();
    d_ptr->reloadWaveBuffer();
}
