/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#pragma once
#include <QByteArray>
#include <QVariant>
#include <QModelIndex>
#include "ParamDefine.h"


enum ScreenLayoutRole
{
    ReplaceRole = Qt::UserRole + 1,
    InsertRole,
    RemoveRole,
};

struct ScreenLayoutItemInfo
{
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
