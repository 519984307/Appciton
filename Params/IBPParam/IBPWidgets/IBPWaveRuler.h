#pragma once
#include "WaveWidgetItem.h"

class IBPWaveWidget;

/***************************************************************************************************
 * ECG 波形控件标尺
 **************************************************************************************************/
class IBPWaveRuler : public WaveWidgetItem
{
public:
    virtual void paintItem(QPainter &painter);
    void setRuler(double up, double mid, double low);

    IBPWaveRuler(IBPWaveWidget *wave);
    ~IBPWaveRuler();

private:
    double _up;             // 上标尺数值, 单位%
    double _mid;            // 中标尺数值, 单位%
    double _low;            // 下标尺数值, 单位%
};
