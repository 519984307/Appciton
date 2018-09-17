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

#define LAYOUT_COLUMN_COUNT 6
#define LAYOUT_ROW_COUNT 8
#define LAYOUT_MAX_WAVE_ROW_NUM 6
#define LAYOUT_WAVE_END_COLUMN 4

enum LayoutWaveNodeType
{
    LAYOUT_WAVE_NODE_ECG1,
    LAYOUT_WAVE_NODE_ECG2,
    LAYOUT_WAVE_NODE_RESP,
    LAYOUT_WAVE_NODE_SPO2,
    LAYOUT_WAVE_NODE_CO2,
    LAYOUT_WAVE_NODE_IBP1,
    LAYOUT_WAVE_NODE_IBP2,
    LAYOUT_WAVE_NODE_N2O,
    LAYOUT_WAVE_NODE_AA1,
    LAYOUT_WAVE_NODE_AA2,
    LAYOUT_WAVE_NODE_O2,
    LAYOUT_WAVE_NODE_NR,
};

enum LayoutParamNodeType
{
    LAYOUT_PARAM_NODE_ECG,
    LAYOUT_PARAM_NODE_SPO2,
    LAYOUT_PARAM_NODE_RESP,
    LAYOUT_PARAM_NODE_IBP1,
    LAYOUT_PARAM_NODE_IBP2,
    LAYOUT_PARAM_NODE_CO2,
    LAYOUT_PARAM_NODE_NIBP,
    LAYOUT_PARAM_NODE_NIBPLIST,
    LAYOUT_PARAM_NODE_TEMP,
    LAYOUT_PARAM_NODE_AA1,
    LAYOUT_PARAM_NODE_AA2,
    LAYOUT_PARAM_NODE_N2O,
    LAYOUT_PARAM_NODE_O2,
    LAYOUT_PARAM_NODE_ST,
    LAYOUT_PARAM_NODE_NR,
};

const char *layoutNodeName(LayoutWaveNodeType waveNode);
const char *layoutNodeName(LayoutParamNodeType paramNode);

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

