////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联模式
////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "WaveCascadeMode.h"
#include "WaveWidget.h"
#include "FloatHandle.h"

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
WaveCascadeMode::WaveCascadeMode(WaveWidget *wave)
        : WaveMode(wave), _a1(0), _b1(0), _c1(0), _a2(0), _b2(0), _c2(0),
          _maxY1(0), _minY2(0), _n(0)
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
WaveCascadeMode::~WaveCascadeMode()
{
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 返回波形控件所能容纳的原始波形数据量
//
// 参数:
// 无
//
// 返回值:
// 波形控件所能容纳的原始波形数据量
////////////////////////////////////////////////////////////////////////////////
int WaveCascadeMode::dataSize()
{
    if (!_wave)
    {
        return 0;
    }

    int minX = _wave->_margin.left();
    int maxX = _wave->width() - 1 - _wave->_margin.right();
    if (minX > maxX)
    {
        return 0;
    }

    int n = ceilf((maxX - minX + 1) * _wave->dataRate() / _wave->waveSpeed() * _wave->pixelWPitch());
    return n * 2;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// Y坐标换算为波形幅值
//
// 参数:
// y: Y坐标
//
// 返回值:
// 波形幅值
////////////////////////////////////////////////////////////////////////////////
int WaveCascadeMode::yToValue(int y)
{
    if ((y <= _maxY1) && (_b1 != 0))
    {
        // 上半段波形
        return (((y - _c1) << 16) / _b1);
    }
    else
    {
        // 下半段波形
        if (_b2 != 0)
        {
            return (((y - _c2) << 16) / _b2);
        }
    }

    return 0;
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
int WaveCascadeMode::valueToY(int value)
{
    // TODO: 默认上半段波形, 下半段波形呢?
    return (((_b1 * value) >> 16) + _c1);
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
int WaveCascadeMode::xToIndex(int x)
{
    if (!_wave || (0 == _a1))
    {
        return 0;
    }

    // TODO: 默认上半段波形, 下半段波形呢?
    int minX = _wave->_margin.left();
    return (((x - minX) << 16) / _a1);
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
int WaveCascadeMode::indexToX(int index)
{
    if (!_wave)
    {
        return 0;
    }

    int minX = _wave->_margin.left();
    if (index < _n)
    {
        // 上半段波形
        return (((_a1 * index) >> 16) + minX);
    }
    else
    {
        // 下半段波形
        index -= _n;
        return (((_a2 * index) >> 16) + minX);
    }
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
void WaveCascadeMode::prepareTransformFactor()
{
    if (!_wave)
    {
        return;
    }

    int minX = _wave->_margin.left();
    int maxX = _wave->width() - 1 - _wave->_margin.right();
    int minY = _wave->_margin.top();
    int maxY = _wave->_margin.top() +
            ((_wave->height() - _wave->_margin.top() - _wave->_margin.bottom() - _wave->_spacing) / 2);

    if (isZero(_wave->pixelWPitch()) || isZero(_wave->waveSpeed() / _wave->pixelWPitch()))
    {
        return;
    }
    int n = (maxX - minX + 1) * _wave->dataRate() / (_wave->waveSpeed() / _wave->pixelWPitch());

    if (1 == n)
    {
        return;
    }
    _a1 = ((maxX - minX) << 16) / (n - 1);

    if (_wave->maxValue() == _wave->minValue())
    {
        return;
    }
    _b1 = ((minY - maxY) << 16) / (_wave->maxValue() - _wave->minValue());
    _c1 = minY - ((_wave->maxValue() * _b1) >> 16);
    _maxY1 = maxY;

    minY = maxY + 1 + _wave->_spacing;
    maxY = _wave->height() - 1 - _wave->_margin.bottom();
    _a2 = ((maxX - minX) << 16) / (n - 1);
    _b2 = ((minY - maxY) << 16) / (_wave->maxValue() - _wave->minValue());
    _c2 = minY - ((_wave->maxValue() * _b2) >> 16);
    _minY2 = minY;
    _n = n;
}
