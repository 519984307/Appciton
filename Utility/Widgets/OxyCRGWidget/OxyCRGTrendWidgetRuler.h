#pragma once
#include "WaveWidgetItem.h"

class OxyCRGTrendWidget;

/***************************************************************************************************
 * ECG 波形控件标尺
 **************************************************************************************************/
class OxyCRGTrendWidgetRuler : public WaveWidgetItem
{
public:
    virtual void paintItem(QPainter &painter);
    void setRuler(int up, int mid, int low);

    OxyCRGTrendWidgetRuler(OxyCRGTrendWidget *wave);
    ~OxyCRGTrendWidgetRuler();

private:
    int _up;             // 上标尺数值, 单位%
    int _mid;            // 中标尺数值, 单位%
    int _low;            // 下标尺数值, 单位%
    float _pixelWPitch;                // 屏幕像素点距, 单位mm
    float _pixelHPitch;                // 屏幕像素点距, 单位mm
};
