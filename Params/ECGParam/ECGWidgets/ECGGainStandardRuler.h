/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by lianghuan <lianghuan@blmed.cn>, 2021/4/23
 **/

#pragma once
#include <QPoint>
#include "WaveWidgetItem.h"
#include <QVector>

class ECGWaveWidget;
/***************************************************************************************************
 * ECGGainStandardRuler
 **************************************************************************************************/
class ECGGainStandardRuler : public WaveWidgetItem
{
public:
    explicit ECGGainStandardRuler(ECGWaveWidget *wave);
    ~ECGGainStandardRuler();

    /**
     * @brief paintItem
     * @param painter
     */
    void paintItem(QPainter &painter);        // NOLINT

private:
    QWidget *_waveWidget;
    QVector<int> gainRuler;
};
