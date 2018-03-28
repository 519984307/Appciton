////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的非级联模式基类
////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "WaveNormalMode.h"
#include "WaveWidget.h"
#include "FloatHandle.h"
#include "Debug.h"

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
    if(!_wave)
    {
        return 0;
    }

    int minX = _wave->_margin.left();
    int maxX = _wave->rect().right() - 1 - _wave->_margin.right();
    if (minX > maxX)
    {
        return 0;
    }

    return (int) ceilf((maxX - minX + 1) * _wave->dataRate() / _wave->waveSpeed() *
            _wave->pixelWPitch());
}

/***************************************************************************************************
 *  计算dataSize，计算所得的dataSize 为同速度波形中采样率最小波形的dataSize的倍数
 * NOTE: it will affect the wave draw speed
 **************************************************************************************************/
int WaveNormalMode::dataSize(float minDataRateOfSameSpeedWave)
{
    if(!_wave)
    {
        return 0;
    }

    int minX = _wave->_margin.left();
    int maxX = _wave->rect().right() - 1 - _wave->_margin.right();
    if (minX > maxX)
    {
        return 0;
    }

    if(isEqual((float)0, minDataRateOfSameSpeedWave) || isEqual(_wave->dataRate(), minDataRateOfSameSpeedWave))
    {
        return (int) ceilf((maxX - minX + 1) * _wave->dataRate() / _wave->waveSpeed() *
                           _wave->pixelWPitch());
    }
    else
    {
        int currentWaveSampleRate = _wave->dataRate();
        int minSampleRate = minDataRateOfSameSpeedWave;

        int dataSizeOfMinSampleWave = (int) ceilf((maxX - minX + 1) * minDataRateOfSameSpeedWave / _wave->waveSpeed() *
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

    return y;
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
    if(!_wave)
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
    if(!_wave)
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
    if(!_wave)
    {
        return;
    }

    int minX = _wave->_margin.left();
    int maxX = _wave->rect().right() - 1 - _wave->_margin.right();
    int minY = _wave->_margin.top();
    int maxY = _wave->height() - 1 - _wave->_margin.bottom();

    if(isZero(_wave->pixelWPitch()) || isZero(_wave->waveSpeed() / _wave->pixelWPitch()))
    {
        return;
    }
    int n = (maxX - minX + 1) * _wave->dataRate() / (_wave->waveSpeed() / _wave->pixelWPitch());

    if(1 == n)
    {
        return;
    }
    _a = ((maxX - minX) << 16) / (n - 1);

    if(_wave->maxValue() == _wave->minValue())
    {
        return;
    }

    _b = (minY - maxY) * 1.0 / (_wave->maxValue() - _wave->minValue());
    _c = minY - _wave->maxValue() * (minY - maxY) * 1.0 / (_wave->maxValue() - _wave->minValue());
}
