/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/20
 **/

#include "FreezeWaveReviewMode.h"
#include "WaveDefine.h"
#include "WaveWidget.h"
#include "ParamDefine.h"
#include <QPointer>
#include <QPainter>
#include <QTimer>
#include "FreezeTimeIndicator.h"
#include "LayoutManager.h"

FreezeWaveReviewMode::FreezeWaveReviewMode(WaveWidget *wave)
    : QObject(), WaveNormalMode(wave), _paintWaveFunc(NULL)
{
}

FreezeWaveReviewMode::~FreezeWaveReviewMode()
{
}

bool FreezeWaveReviewMode::match(int mode, bool isCascade)
{
    return ((mode == FREEZE_REVIEW_MODE) && !isCascade);
}

void FreezeWaveReviewMode::prepareTransformFactor()
{
    WaveNormalMode::prepareTransformFactor();
    if (!_wave)
    {
        return;
    }

    if (dataSize() >= _wave->width())
    {
        _paintWaveFunc = _wave->isFill() ? _fillDenseCurve : _drawDenseCurve;
    }
    else
    {
        _paintWaveFunc = _wave->isFill() ? _fillSparseCurve : _drawSparseCurve;
    }
}

void FreezeWaveReviewMode::paintWave(QPainter &painter, const QRect &rect)
{
    if (!_wave || !_paintWaveFunc)
    {
        return;
    }

    painter.setBrush(_wave->palette().windowText());

    // 把绘制区域的左右边界换算为波形坐标序列索引
    int leftIndex = qMax(0, xToIndex(rect.left() - _wave->lineWidth()) - 1);
    int rightIndex = qMin((_wave->_size - 1),
                          xToIndex(rect.right() + _wave->lineWidth()) + 1);
    int beginIndex = qMax(leftIndex, _wave->_tail);
    int endIndex = qMin(rightIndex, _wave->_head - 1);
    if (endIndex > beginIndex)
    {
        _drawWave(painter, beginIndex, endIndex);
    }
}

void FreezeWaveReviewMode::enter()
{
    if (!_wave || !_wave->_freezeDataModel)
    {
        return;
    }

    connect(_wave->_freezeDataModel.data(), SIGNAL(dataChanged()), this, SLOT(_onModelChanged()));
    // load data later, after the wave buffer reset
    QTimer::singleShot(10, this, SLOT(_onModelChanged()));
    if (layoutManager.isLastWaveWidget(_wave) && _wave->_freezeIndicator)
    {
        _wave->_freezeIndicator->setVisible(true);
        _wave->_freezeIndicator->setPalette(_wave->palette());
    }
}

void FreezeWaveReviewMode::exit()
{
    if (!_wave || !_wave->_freezeDataModel)
    {
        return;
    }
    disconnect(_wave->_freezeDataModel.data(), SIGNAL(dataChanged()), this, SLOT(_onModelChanged()));
    if (layoutManager.isLastWaveWidget(_wave) && _wave->_freezeIndicator)
    {
        _wave->_freezeIndicator->setVisible(false);
    }
}

void FreezeWaveReviewMode::_onModelChanged()
{
    if (!_wave || !_wave->_freezeDataModel)
    {
        return;
    }

    // load the wave data
    _loadWaveData();

    if (_wave->_freezeIndicator)
    {
        _wave->_freezeIndicator->setReviewTime(_wave->_freezeDataModel->getReviewStartSecond());
    }

    _wave->update();
}

void FreezeWaveReviewMode::_loadWaveData()
{
    if (!_wave || !_wave->_waveBuf || !_wave->_dataBuf || !_wave->_flagBuf
            || !_wave->_freezeDataModel)
    {
        return;
    }

    // clear the wave
    _wave->_head = 0;
    _wave->_tail = 0;
    _wave->_waveRangeInfo.clear();

    // load data
    _wave->_freezeDataModel->getWaveData(reinterpret_cast<WaveDataType *>(_wave->_dataBuf), _wave->_size);
    _wave->_head = _wave->_size - 1;

    // fill the buffer
    for (int i = _wave->_tail; i != _wave->_head; i = _wave->_bufNext(i))
    {
        _wave->_waveBuf[i].setX(_wave->_xBuf[i]);
        short yValue = _wave->_dataBuf[i] & 0xFFFF;
        _wave->_waveBuf[i].setY(valueToY(yValue));
        _wave->_flagBuf[i] = (_wave->_dataBuf[i] & 0xFFFF0000) >> 16;
        _wave->_setDyBuf(i);
        _wave->_updateRangeInfo(i);
    }
}

void FreezeWaveReviewMode::_drawWave(QPainter &painter, int beginIndex, int endIndex)
{
    while (beginIndex <= endIndex)
    {
        int j = beginIndex + 1;
        if (_wave->_flagBuf[beginIndex] & 0x4000)
        {
            while ((j <= endIndex) && (_wave->_flagBuf[j] & 0x4000))
            {
                ++j;
            }
            _drawDotLine(painter, beginIndex, j - 1);
        }
        else
        {
            while ((j <= endIndex) && !(_wave->_flagBuf[j] & 0x4000))
            {
                ++j;
            }

            _paintWaveFunc(this, painter, beginIndex, j - 1);
        }

        beginIndex = j;
    }
}

void FreezeWaveReviewMode::_drawDotLine(QPainter &painter, int beginIndex, int endIndex)
{
    QPen p = QPen(_wave->palette().windowText(), _wave->lineWidth());
    qreal dashWidth =  6.5 / _wave->lineWidth();
    QVector<qreal> pat;
    pat << dashWidth << dashWidth;
    p.setDashPattern(pat);
    p.setCapStyle(Qt::FlatCap);
    painter.setPen(p);

    painter.setRenderHint(QPainter::Antialiasing, _wave->isAntialias());
    painter.drawPolyline(&_wave->_waveBuf[beginIndex], endIndex - beginIndex + 1);
}

void FreezeWaveReviewMode::_drawSparseCurve(FreezeWaveReviewMode *self, QPainter &painter, int beginIndex, int endIndex)
{
    if (!self || !self->_wave)
    {
        return;
    }
    if (self->_wave->colorGradationScanMode)
    {
        drawColorGradationWave(self, &painter, beginIndex, endIndex);
    }
    else
    {
        painter.setPen(
                    QPen(self->_wave->palette().windowText(),
                         self->_wave->lineWidth()));
        painter.setRenderHint(QPainter::Antialiasing, self->_wave->isAntialias());

        if (beginIndex < endIndex)
        {
            painter.drawPolyline(&self->_wave->_waveBuf[beginIndex],
                                 endIndex - beginIndex + 1);
        }
        else if (beginIndex == endIndex)
        {
            painter.drawPoints(&self->_wave->_waveBuf[beginIndex], 1);
        }
    }
}

void FreezeWaveReviewMode::_drawDenseCurve(FreezeWaveReviewMode *self, QPainter &painter, int beginIndex, int endIndex)
{
    if (!self || !self->_wave)
    {
        return;
    }

    if (self->_wave->colorGradationScanMode && !self->_wave->isFill())
    {
        drawColorGradationWave(self, &painter, beginIndex, endIndex);
    }
    else
    {
        painter.setPen(
                    QPen(self->_wave->palette().windowText(),
                         self->_wave->lineWidth()));
        painter.setRenderHint(QPainter::Antialiasing, self->_wave->isAntialias());

        int i = beginIndex;
        while (i <= endIndex)
        {
            int j = i + 1;
            if (j > endIndex)
            {
                painter.drawPoints(&self->_wave->_waveBuf[i], 1);
                break;
            }
            else if (self->_wave->_dyBuf[j])
            {
                while (((j + 1) <= endIndex) && self->_wave->_dyBuf[j + 1])
                {
                    j++;
                }
                painter.drawPolyline(&self->_wave->_waveBuf[i], (j - i + 1));
                i = j + 1;
            }
            else
            {
                while (((j + 1) <= endIndex) && !self->_wave->_dyBuf[j + 1])
                {
                    j++;
                }
                painter.drawPoints(&self->_wave->_waveBuf[i], (j - i));
                i = j;
            }
        }
    }
}

void FreezeWaveReviewMode::_fillSparseCurve(FreezeWaveReviewMode *self, QPainter &painter, int beginIndex, int endIndex)
{
    if (!self || !self->_wave)
    {
        return;
    }

    painter.setPen(self->_wave->palette().color(QPalette::WindowText));
    painter.setRenderHint(QPainter::Antialiasing, false);

    int i = beginIndex;
    int yBottom = self->_wave->height() - 1 - self->_wave->_margin.bottom();
    while (i <= endIndex)
    {
        int j = i + 1;
        if (j > endIndex)
        {
            // 单数据点
            painter.drawLine(self->_wave->_waveBuf[i],
                             QPoint(self->_wave->_xBuf[i], yBottom));
            break;
        }
        else if (self->_wave->_waveBuf[i].y() == self->_wave->_waveBuf[j].y())
        {
            // 矩形
            while (((j + 1) <= endIndex)
                    && (self->_wave->_waveBuf[i].y()
                        == self->_wave->_waveBuf[j + 1].y()))
            {
                j++;
            }
            QRect rect;
            rect.setTopLeft(self->_wave->_waveBuf[i]);
            rect.setWidth(self->_wave->_xBuf[j] - self->_wave->_xBuf[i] + 1);
            rect.setBottom(yBottom);
            painter.fillRect(rect,
                             self->_wave->palette().color(QPalette::WindowText));
            i = j;
        }
        else if (self->_wave->_waveBuf[i].y() > self->_wave->_waveBuf[j].y())
        {
            // 梯形
            QPoint tri[3];
            tri[0] = self->_wave->_waveBuf[i];
            tri[1] = self->_wave->_waveBuf[j];
            tri[2].setX(tri[1].x());
            tri[2].setY(tri[0].y());
            painter.drawConvexPolygon(tri, 3);

            QRect rect;
            rect.setTopLeft(tri[0]);
            rect.setWidth(tri[1].x() - tri[0].x() + 1);
            rect.setBottom(yBottom);
            painter.fillRect(rect,
                             self->_wave->palette().color(QPalette::WindowText));
            i = j;
        }
        else
        {
            // 梯形
            QPoint tri[3];
            tri[0] = self->_wave->_waveBuf[i];
            tri[1] = self->_wave->_waveBuf[j];
            tri[2].setX(tri[0].x());
            tri[2].setY(tri[1].y());
            painter.drawConvexPolygon(tri, 3);

            QRect rect;
            rect.setTopLeft(tri[2]);
            rect.setWidth(tri[1].x() - tri[0].x() + 1);
            rect.setBottom(yBottom);
            painter.fillRect(rect,
                             self->_wave->palette().color(QPalette::WindowText));
            i = j;
        }
    }
}

void FreezeWaveReviewMode::_fillDenseCurve(FreezeWaveReviewMode *self, QPainter &painter, int beginIndex, int endIndex)
{
    if (!self || !self->_wave)
    {
        return;
    }

    painter.setPen(self->_wave->palette().color(QPalette::WindowText));
    painter.setRenderHint(QPainter::Antialiasing, false);

    int yBottom = self->_wave->height() - 1 - self->_wave->_margin.bottom();
    for (int i = beginIndex; i <= endIndex; i++)
    {
        painter.drawLine(self->_wave->_waveBuf[i],
                         QPoint(self->_wave->_xBuf[i], yBottom));
    }

    _drawDenseCurve(self, painter, beginIndex, endIndex);
}
