/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#include <QByteArray>
#include <QVariant>
#include <QModelIndex>
#include "ParamDefine.h"


struct ScreenLayoutItemInfo
{
    enum LayoutInfoRole
    {
        AvaliableWaveRole = Qt::UserRole + 1,
        AvaliableParamRole,
    };

    ScreenLayoutItemInfo()
        :  baseLine(128),
          waveMinValue(0),
          waveMaxValue(255),
          sampleRate(250),
          drawSpeed(12.5),
          waveid(WAVE_NONE)
    {}

    QString name;
    QByteArray waveContent;
    int baseLine;
    int waveMinValue;
    int waveMaxValue;
    int sampleRate;
    float drawSpeed;
    WaveformID waveid;
};


Q_DECLARE_METATYPE(ScreenLayoutItemInfo)
