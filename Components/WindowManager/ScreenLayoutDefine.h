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

/**
 * @brief The ScreenLayoutItemInfo struct mainly use to draw the item
 */
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

    QString name;           // the item's name, this name is saved in the config file. a empyty value
                            // means the item is not used yet
    QString displayName;    // the item's display name, this is used to draw in the screen
    QByteArray waveContent;
    int baseLine;
    int waveMinValue;
    int waveMaxValue;
    int sampleRate;
    float drawSpeed;
    WaveformID waveid;
};

Q_DECLARE_METATYPE(ScreenLayoutItemInfo)
