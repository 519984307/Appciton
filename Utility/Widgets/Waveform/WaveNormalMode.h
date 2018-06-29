////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的非级联模式基类
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_NORMAL_MODE_H
#define WAVE_NORMAL_MODE_H

#include "WaveMode.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的非级联模式基类
////////////////////////////////////////////////////////////////////////////////
class WaveNormalMode: public WaveMode
{
public:
    explicit WaveNormalMode(WaveWidget *wave);
    ~WaveNormalMode();
    int dataSize();
    int dataSize(float minDataRateOfSameSpeedWave);
    int yToValue(int y);
    int valueToY(int value);
    int xToIndex(int x);
    int indexToX(int index);
    void prepareTransformFactor();

private:
    int _a;          // 波形坐标换算因子a
    double _b;          // 波形坐标换算因子b
    double _c;          // 波形坐标换算因子c
};

#endif
