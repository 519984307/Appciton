////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联模式
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_CASCADE_MODE_H
#define WAVE_CASCADE_MODE_H

#include "WaveMode.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联模式
////////////////////////////////////////////////////////////////////////////////
class WaveCascadeMode: public WaveMode
{
public:
    WaveCascadeMode(WaveWidget *wave);
    ~WaveCascadeMode();
    int dataSize();
    int yToValue(int y);
    int valueToY(int value);
    int xToIndex(int x);
    int indexToX(int index);
    void prepareTransformFactor();

private:
    int _a1;           // 上半段波形坐标换算因子a
    int _b1;           // 上半段波形坐标换算因子b
    int _c1;           // 上半段波形坐标换算因子c
    int _a2;           // 下半段波形坐标换算因子a
    int _b2;           // 下半段波形坐标换算因子b
    int _c2;           // 下半段波形坐标换算因子c
    int _maxY1;        // 上半段波形的Y坐标上限
    int _minY2;        // 下半段波形的Y坐标下限
    int _n;            // 半段波形的点数
};

#endif
