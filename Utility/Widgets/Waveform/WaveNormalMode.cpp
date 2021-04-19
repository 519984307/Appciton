/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/10/17
 **/

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的非级联模式基类
////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "WaveNormalMode.h"
#include "WaveWidget.h"
#include "FloatHandle.h"
#include <QPainter>
#include "Debug.h"

#define MAX_VAL(a, b) (((a) > (b)) ? (a) : (b))
#define MIN_VAL(a, b) (((a) < (b)) ? (a) : (b))


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
WaveNormalMode::WaveNormalMode(WaveWidget *wave)
        : WaveMode(wave), _a(0), _b(0), _c(0)
{
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 析构函数
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveNormalMode::~WaveNormalMode()
{
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 获取波形控件所能容纳的原始波形数据量
//
// 参数:
// 无
//
// 返回值:
// 波形控件所能容纳的原始波形数据量
////////////////////////////////////////////////////////////////////////////////
int WaveNormalMode::dataSize()
{
    if (!_wave)
    {
        return 0;
    }

    int minX = _wave->_margin.left();
    int maxX = _wave->rect().right() - 1 - _wave->_margin.right();
    if (minX > maxX)
    {
        return 0;
    }

    return ceilf((maxX - minX + 1) * _wave->dataRate() / _wave->waveSpeed() *
            _wave->pixelWPitch());
}

/***************************************************************************************************
 *  计算dataSize，计算所得的dataSize 为同速度波形中采样率最小波形的dataSize的倍数
 * NOTE: it will affect the wave draw speed
 **************************************************************************************************/
int WaveNormalMode::dataSize(float minDataRateOfSameSpeedWave)
{
    if (!_wave)
    {
        return 0;
    }

    int minX = _wave->_margin.left();
    int maxX = _wave->rect().right() - 1 - _wave->_margin.right();
    if (minX > maxX)
    {
        return 0;
    }

    if (isEqual(0.0f, minDataRateOfSameSpeedWave)
            || isEqual(_wave->dataRate(), minDataRateOfSameSpeedWave))
    {
        return ceilf((maxX - minX + 1) * _wave->dataRate() / _wave->waveSpeed() *
                           _wave->pixelWPitch());
    }
    else
    {
        int currentWaveSampleRate = _wave->dataRate();
        int minSampleRate = minDataRateOfSameSpeedWave;

        int dataSizeOfMinSampleWave = ceilf((maxX - minX + 1) * minDataRateOfSameSpeedWave / _wave->waveSpeed() *
                           _wave->pixelWPitch());

        return dataSizeOfMinSampleWave * currentWaveSampleRate / minSampleRate;
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// Y坐标换算为波形幅值
//
// 参数:
// y: Y坐标
//
// 返回值:构造函数
// 波形幅值
////////////////////////////////////////////////////////////////////////////////
int WaveNormalMode::yToValue(int y)
{
    return ((y - _c) / _b);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 波形幅值换算为Y坐标
//
// 参数:
// value: 波形幅值
//
// 返回值:
// Y坐标
////////////////////////////////////////////////////////////////////////////////
int WaveNormalMode::valueToY(int value)
{
    int minY = _wave->_margin.top();
    int maxY = _wave->height() - 1 - _wave->_margin.bottom();
    double y = _b * value + _c;

    // 波形截顶
    if (y < minY)
    {
        y = minY;
    }
    else if (y > maxY)
    {
        y = maxY;
    }

    /* Lower the upper side wave by 1 pixel to meet DAVID specification when the round off */
    int mid = (minY + maxY) / 2;
    int v = y < mid ? y + 0.5 : y;

    return v;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// X坐标换算为波形数据索引值
//
// 参数:
// x: X坐标
//
// 返回值:
// 波形数据索引值
////////////////////////////////////////////////////////////////////////////////
int WaveNormalMode::xToIndex(int x)
{
    if (!_wave)
    {
        return 0;
    }

    int minX = _wave->_margin.left();
    return (((x - minX) << 16) / _a);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 波形数据索引值换算为X坐标
//
// 参数:
// index: 波形数据索引值
//
// 返回值:
// X坐标
////////////////////////////////////////////////////////////////////////////////
int WaveNormalMode::indexToX(int index)
{
    if (!_wave)
    {
        return 0;
    }

    int minX = _wave->_margin.left();
    return (((_a * index) >> 16) + minX);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 调整波形坐标换算因子
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveNormalMode::prepareTransformFactor()
{
    if (!_wave)
    {
        return;
    }

    int minX = _wave->_margin.left();
    int maxX = _wave->rect().right() - 1 - _wave->_margin.right();
    int minY = _wave->_margin.top();
    int maxY = _wave->height() - 1 - _wave->_margin.bottom();

    if (isZero(_wave->pixelWPitch()) || isZero(_wave->waveSpeed() / _wave->pixelWPitch()))
    {
        return;
    }
    int n = (maxX - minX + 1) * _wave->dataRate() / (_wave->waveSpeed() / _wave->pixelWPitch());

    if (1 == n)
    {
        return;
    }
    _a = ((maxX - minX) << 16) / (n - 1);

    if (_wave->maxValue() == _wave->minValue())
    {
        return;
    }

    _b = (minY - maxY) * 1.0 / (_wave->maxValue() - _wave->minValue());
    _c = minY - _wave->maxValue() * (minY - maxY) * 1.0 / (_wave->maxValue() - _wave->minValue());
}

/**
 * @brief drawColorGradationVLine draw a color gradation vertical line, the middle point will draw with base color
 *        the two edge color will be gradient
 * @param painter the paitner
 * @param xPos the x position
 * @param yMin the min y axis value
 * @param yMax the max y axis value
 * @param yMid the middle y axis value
 * @param baseColor the base color
 */
static void drawColorGradationVLine(QPainter *painter, int xPos, int yMin, int yMax,
                                             int yMid, const QColor &baseColor)
{
    int dy = yMax - yMin;
    int index = (dy >> 1) + 2;
    QColor curColor = baseColor;
    QPen curPen(baseColor);
    painter->setPen(curPen);
    painter->drawPoint(xPos, yMid);

    for (int i = 1; i <= (dy >> 1) + 1; ++i)
    {
        curColor.setRed(baseColor.red() * (index - i) / index);
        curColor.setGreen(baseColor.green() * (index - i) / index);
        curColor.setBlue(baseColor.blue() * (index - i) / index);
        curPen.setColor(curColor);
        painter->setPen(curPen);
        if (yMid + i <= yMax) {
            painter->drawPoint(xPos, yMid +i);
        }
        if (yMid - i >= yMin) {
            painter->drawPoint(xPos, yMid - i);
        }
    }
}

void WaveNormalMode::drawColorGradationWave(WaveNormalMode *self, QPainter *painter, int beginIndex, int endIndex)
{
    int xStart = self->_wave->xBuf(beginIndex);
    QColor mainColor = self->_wave->palette().color(QPalette::WindowText);
    int xEnd = self->_wave->xBuf(endIndex);
    WaveRangeInfo olderRangeInfo = self->_wave->rangeInfo(xStart - 2);
    WaveRangeInfo oldRangeInfo = self->_wave->rangeInfo(xStart - 1);

    for (int x = xStart; x <= xEnd; x++)
    {
        const WaveRangeInfo &info = self->_wave->rangeInfo(x);
        if (!info.isValid())
        {
            olderRangeInfo.clear();
            oldRangeInfo.clear();
            continue;
        }

        if (!oldRangeInfo.isValid())
        {
            oldRangeInfo = info;
        }

        if (!olderRangeInfo.isValid())
        {
            olderRangeInfo = oldRangeInfo;
        }
        int mid = oldRangeInfo.midvalue();
        int min = MIN_VAL(olderRangeInfo.min, MIN_VAL(oldRangeInfo.min, info.min));
        int max = MAX_VAL(olderRangeInfo.max, MAX_VAL(oldRangeInfo.max, info.max));
        min -= self->_wave->lineWidth();
        max += self->_wave->lineWidth();

        drawColorGradationVLine(painter, x, min, max, mid, mainColor);

        olderRangeInfo = oldRangeInfo;
        oldRangeInfo = info;
    }
}
