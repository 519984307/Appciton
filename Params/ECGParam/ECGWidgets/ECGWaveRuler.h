#pragma once
#include <QPoint>
#include "WaveWidgetItem.h"

class ECGWaveWidget;
/***************************************************************************************************
 * ECG 波形控件标尺
 **************************************************************************************************/
class ECGWaveRuler : public WaveWidgetItem
{
public:
    ECGWaveRuler(ECGWaveWidget *wave);
    ~ECGWaveRuler();

    void paintItem(QPainter &painter);
    void setRulerHeight(int rulerHeight);
    int rulerHeight()
    {
        return _rulerHeight;
    }

private:
    int _rulerHeight;            // 标尺高度, 单位像素
};
