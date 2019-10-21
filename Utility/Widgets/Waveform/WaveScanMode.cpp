/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/26
 **/

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的扫描模式
////////////////////////////////////////////////////////////////////////////////
#include <QPainter>
#include "WaveScanMode.h"
#include "WaveWidget.h"
#include "Debug.h"
// #include "CommonApplication.h"

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 构造函数
//
// 参数:
// parent: 波形控件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveScanMode::WaveScanMode(WaveWidget *wave)
    : WaveNormalMode(wave), _paintWaveFunc(_drawSparseCurve), _pendingCount(
          0), _scanLineSpace(0)
{
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 析构函数
//
// 参数:
// 无
// _pending_count
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveScanMode::~WaveScanMode()
{
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 模式匹配检查
//
// 参数:
// mode: 模式标志
// isCascade: 是否级联
//
// 返回值:
// 匹配返回true, 否则返回false
////////////////////////////////////////////////////////////////////////////////
bool WaveScanMode::match(int mode, bool isCascade)
{
    return ((mode == SCAN_MODE) && !isCascade);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 获取刷新率上限, 单位Hz
//
// 参数:
// 无
//
// 返回值:
// 刷新率上限, 单位Hz
////////////////////////////////////////////////////////////////////////////////
int WaveScanMode::maxUpdateRate()
{
    return 30;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 获取更新区域
//
// 参数:
// region: 返回更新区域
//
// 返回值:
// 更新区域非空返回true, 否则返回false
////////////////////////////////////////////////////////////////////////////////
bool WaveScanMode::updateRegion(QRegion &region)
{
    if (_pendingCount < 1)
    {
        return false;
    }

    if (!_wave)
    {
        return false;
    }

    int count = _wave->_bufCount();
    if (count < 3 || (_pendingCount < 4 && _wave->dataRate() > 200))
    {
        return false;
    }

    _pendingCount = qMin(_pendingCount, count);

    QRect rect;
    rect.setTop(0);
    rect.setBottom(_wave->height() - 1);

    // 计算波头更新区域
    //预留画标记长度
    int startX = _wave->_waveBufHead(_pendingCount + 1).x() - 4;
    int endX = _wave->_waveBufHead(0).x();
    if (startX <= endX)
    {
        rect.setLeft(startX);
        rect.setRight(endX);
        region += rect;
    }
    else
    {
        rect.setLeft(startX);
        rect.setRight(_wave->width() - 1);
        region += rect;

        rect.setLeft(0);
        rect.setRight(endX);
        region += rect;
    }

    _pendingCount = 0;

    // 计算波尾更新区域
    int deleteCount = _scanLineSpace - _wave->_bufSpace();
    if (deleteCount > 0)
    {
        startX = _wave->_waveBufTail(0).x() - 1;
        endX = _wave->_waveBufTail(deleteCount).x();
        if (startX <= endX)
        {
            rect.setLeft(startX);
            rect.setRight(endX);
            region += rect;
        }
        else
        {
            rect.setLeft(startX);
            rect.setRight(_wave->width() - 1);
            region += rect;

            rect.setLeft(0);
            rect.setRight(endX);
            region += rect;
        }

        _wave->_bufPop(deleteCount);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 调整波形坐标换算参数
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::prepareTransformFactor()
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

    _pendingCount = 0;

    if (_wave->waveSpeed() != 0)
    {
//        _scanLineSpace = 2 * _wave->dataRate() / _wave->waveSpeed();
        if (_wave->dataRate() <= 50)
        {
            // if the sample rate is too small, the use floor function to avoid large scan line space
            _scanLineSpace = floor(_wave->scanLineSpace * 1.0 * (_wave->_pixelWPitch * _wave->dataRate() /
                                   _wave->waveSpeed()));
            if (_scanLineSpace == 0)
            {
                /* at lease 1 sample */
                _scanLineSpace = 1;
            }
        }
        else
        {
            _scanLineSpace = ceilf(_wave->scanLineSpace * 1.0 * (_wave->_pixelWPitch * _wave->dataRate() /
                                   _wave->waveSpeed()));
        }
        _wave->_spaceDataNum = ceilf(6.5  * (_wave->_pixelWPitch * _wave->dataRate() /
                                             _wave->waveSpeed()));
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 响应用户对波形上下限的修改, 该函数重新计算整个波形的坐标序列, 注意调用该函数前, 需要保证
// WaveWidget::_minValue和WaveWidget::_maxValue修改完毕
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::valueRangeChanged()
{
    if (!_wave)
    {
        return;
    }

    _pendingCount = 0;

    if (_wave->bufIsEmpty())
    {
        return;
    }
    else if (_wave->bufIsContinuous())
    {
        for (int i = _wave->_tail; i < _wave->_head; i++)
        {
            _wave->_waveBuf[i].setX(_wave->_xBuf[i]);
            _wave->_waveBuf[i].setY(valueToY(_wave->_dataBuf[i]));
            _wave->_setDyBuf(i);
            _wave->_updateRangeInfo(i);
        }
    }
    else
    {
        for (int i = 0; i < _wave->_head; i++)
        {
            _wave->_waveBuf[i].setX(_wave->_xBuf[i]);
            _wave->_waveBuf[i].setY(valueToY(_wave->_dataBuf[i]));
            _wave->_setDyBuf(i);
            _wave->_updateRangeInfo(i);
        }
        for (int i = _wave->_tail; i < _wave->_size; i++)
        {
            _wave->_waveBuf[i].setX(_wave->_xBuf[i]);
            _wave->_waveBuf[i].setY(valueToY(_wave->_dataBuf[i]));
            _wave->_setDyBuf(i);
            _wave->_updateRangeInfo(i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 接收原始波形数据, 换算成XY坐标缓存起来
//
// 参数:
// value: 原始波形数据
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::addData(int value, int flag, bool isUpdated)
{
    Q_UNUSED(isUpdated)
    if (!_wave)
    {
        return;
    }

    _wave->_bufPush(_wave->_xBuf[_wave->_head], valueToY(value), value, flag);

    _pendingCount++;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制波形: 根据绘制区域的X轴跨度来决定绘制曲线的一小部分, 以优化性能
//
// 参数:
// painter: 绘图对象
// rect: 重绘区域
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::paintWave(QPainter &painter, const QRect &rect)
{
    if (!_wave)
    {
        return;
    }

    if (_wave->bufIsEmpty())
    {
        return;
    }

    painter.setBrush(_wave->palette().windowText());

    // 把绘制区域的左右边界换算为波形坐标序列索引
    int leftIndex = qMax(0, xToIndex(rect.left() - _wave->lineWidth()) - 1);
    int rightIndex = qMin((_wave->_size - 1), xToIndex(rect.right() + _wave->lineWidth()) + 1);
//    int leftIndex = qMax(0, xToIndex(rect.left()));
//    int rightIndex = qMin((_wave->_size - 1), xToIndex(rect.right()));
    int beginIndex;
    int endIndex;
    int count;

    // 绘制波形曲线
    if (_wave->_head > _wave->_tail)
    {
        // 缓冲连续情形
        beginIndex = qMax(leftIndex, _wave->_tail);
        endIndex = qMin(rightIndex, _wave->_head - 1);
        count = endIndex - beginIndex + 1;
        if (count > 0)
        {
            _drawWave(painter, beginIndex, endIndex);
//            _paintWaveFunc(this, painter, beginIndex, endIndex);
        }
    }
    else
    {
        // 缓冲不连续情形
        beginIndex = qMax(leftIndex, _wave->_tail);
        endIndex = qMin(rightIndex, _wave->_size - 1);
        count = endIndex - beginIndex + 1;
        if (count > 0)
        {
            _drawWave(painter, beginIndex, endIndex);
//            _paintWaveFunc(this, painter, beginIndex, endIndex);
        }

        beginIndex = qMax(leftIndex, 0);
        endIndex = qMin(rightIndex, _wave->_head - 1);
        count = endIndex - beginIndex + 1;
        if (count > 0)
        {
            _drawWave(painter, beginIndex, endIndex);
//            _paintWaveFunc(this, painter, beginIndex, endIndex);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 画波形
//
// 参数:
// painter: 绘图对象
// beginIndex: 开始索引
// endIndex:结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::_drawWave(QPainter &painter, int beginIndex, int endIndex)
{
//    int j = 0;
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

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 画虚线
//
// 参数:
// painter: 绘图对象
// beginIndex: 开始索引
// endIndex:结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::_drawDotLine(QPainter &painter, int beginIndex, int endIndex)
{
    painter.setPen(
        QPen(_wave->palette().windowText(),
             _wave->lineWidth()));
    painter.setRenderHint(QPainter::Antialiasing, _wave->isAntialias());
    int x = beginIndex;
    for (int i = x + 1; i <= endIndex; ++i)
    {
        if (_wave->_spaceFlag[i])
        {
            painter.drawLine(_wave->_waveBuf[x], _wave->_waveBuf[i]);
        }
        x = i;
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制稀疏曲线, 把波形分解为折线段
//
// 参数:
// painter: 绘图对象
// beginIndex: 波形起始索引
// endIndex: 波形结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::_drawSparseCurve(WaveScanMode *self, QPainter &painter,
                                    int beginIndex, int endIndex)
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

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制稠密曲线, 根据相邻点Y坐标的差异把波形分解为点和线
//
// 参数:
// painter: 绘图对象
// beginIndex: 波形起始索引
// endIndex: 波形结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::_drawDenseCurve(WaveScanMode *self, QPainter &painter,
                                   int beginIndex, int endIndex)
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

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制填充曲线, 分解为三角形和矩形
//
// 参数:
// painter: 绘图对象
// beginIndex: 波形起始索引
// endIndex: 波形结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::_fillSparseCurve(WaveScanMode *self, QPainter &painter,
                                    int beginIndex, int endIndex)
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

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制稠密填充曲线
//
// 参数:
// painter: 绘图对象
// beginIndex: 波形起始索引
// endIndex: 波形结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanMode::_fillDenseCurve(WaveScanMode *self, QPainter &painter,
                                   int beginIndex, int endIndex)
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
