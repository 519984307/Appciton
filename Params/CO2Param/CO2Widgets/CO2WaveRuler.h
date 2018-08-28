/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/28
 **/



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

    explicit CO2WaveRuler(CO2WaveWidget *wave);
    ~CO2WaveRuler();

private:
    double _up;             // 上标尺数值, 单位%
    double _mid;            // 中标尺数值, 单位%
    double _low;            // 下标尺数值, 单位%
};
