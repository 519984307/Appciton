/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/27
 **/


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
    void setRuler(int up, int mid, int low);

    explicit IBPWaveRuler(IBPWaveWidget *wave);
    ~IBPWaveRuler();

private:
    int _up;             // 上标尺数值, 单位%
    int _mid;            // 中标尺数值, 单位%
    int _low;            // 下标尺数值, 单位%
};
