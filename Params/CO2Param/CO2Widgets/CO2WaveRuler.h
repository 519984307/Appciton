#pragma once
#include "WaveWidgetItem.h"

class CO2WaveWidget;

/***************************************************************************************************
 * ECG 波形控件标尺
 **************************************************************************************************/
class CO2WaveRuler : public WaveWidgetItem
{
public:
    virtual void paintItem(QPainter &painter);
    void setRuler(double up, double mid, double low);

    CO2WaveRuler(CO2WaveWidget *wave);
    ~CO2WaveRuler();

private:
    double _up;             // 上标尺数值, 单位%
    double _mid;            // 中标尺数值, 单位%
    double _low;            // 下标尺数值, 单位%
};
