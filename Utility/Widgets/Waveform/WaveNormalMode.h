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
#ifndef WAVE_NORMAL_MODE_H
#define WAVE_NORMAL_MODE_H

#include "WaveMode.h"

class QColor;
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

protected:
    /**
     * @brief drawColorGradationWave draw a color gradataion wave
     * @param self the wave draw mode
     * @param beginIndex the wave data start index
     * @param endIndex the wave data end index
     */
    static void drawColorGradationWave(WaveNormalMode *self, QPainter *painter, int beginIndex, int endIndex);

private:
    int _a;          // 波形坐标换算因子a
    double _b;          // 波形坐标换算因子b
    double _c;          // 波形坐标换算因子c
};

#endif
