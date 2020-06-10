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
#define LAYOUT_ROW_COUNT 7
#define LAYOUT_MAX_WAVE_ROW_NUM 6
#define LAYOUT_WAVE_END_COLUMN 4

enum LayoutNodeType
{
    LAYOUT_NODE_NONE,
    // param
    LAYOUT_NODE_PARAM_ECG,
    LAYOUT_NODE_PARAM_ECG2,
    LAYOUT_NODE_PARAM_RESP,
    LAYOUT_NODE_PARAM_SPO2,
    LAYOUT_NODE_PARAM_PI,
    LAYOUT_NODE_PARAM_PVI,
    LAYOUT_NODE_PARAM_SPHB,
    LAYOUT_NODE_PARAM_SPOC,
    LAYOUT_NODE_PARAM_SPMET,
    LAYOUT_NODE_PARAM_SPCO,
    LAYOUT_NODE_PARAM_CO2,
    LAYOUT_NODE_PARAM_IBP1,
    LAYOUT_NODE_PARAM_IBP2,
    LAYOUT_NODE_PARAM_N2O,
    LAYOUT_NODE_PARAM_AA1,
    LAYOUT_NODE_PARAM_AA2,
    LAYOUT_NODE_PARAM_O2,

    LAYOUT_NODE_PARAM_NIBP,
    LAYOUT_NODE_PARAM_NIBPLIST,
    LAYOUT_NODE_PARAM_TEMP,
    LAYOUT_NODE_PARAM_CO,
    LAYOUT_NODE_PARAM_ST,
    LAYOUT_NODE_PARAM_PVCS,
    LAYOUT_NODE_PARAM_OXYGEN,
    // wave
    LAYOUT_NODE_WAVE_ECG1,
    LAYOUT_NODE_WAVE_ECG2,
    LAYOUT_NODE_WAVE_RESP,
    LAYOUT_NODE_WAVE_SPO2,
    LAYOUT_NODE_WAVE_SPO2_2,
    LAYOUT_NODE_WAVE_CO2,
    LAYOUT_NODE_WAVE_IBP1,
    LAYOUT_NODE_WAVE_IBP2,
    LAYOUT_NODE_WAVE_N2O,
    LAYOUT_NODE_WAVE_AA1,
    LAYOUT_NODE_WAVE_AA2,
    LAYOUT_NODE_WAVE_O2,

    // trendWave
    LAYOUT_NODE_TREND_WAVE_SPO2,
    LAYOUT_NODE_TREND_WAVE_PVI,
    LAYOUT_NODE_TREND_WAVE_SPHB,
    LAYOUT_NODE_TREND_WAVE_SPMET,
    LAYOUT_NODE_TREND_WAVE_SPCO,
    LAYOUT_NODE_TREND_WAVE_PR,

    LAYOUT_NODE_NR,
};

/**
 * @brief layoutNodeName get the layout node name
 * @param nodeType the node type
 * @return  the node name
 */
const char *layoutNodeName(LayoutNodeType nodeType);

/**
 * @brief The LayoutNode struct layout node info
 */
struct LayoutNode
{
    LayoutNode()
        : pos(0), span(1), editable(true)
    {}

    QString name;       // the node name
    int pos;            // the node pos
    int span;           // the node span
    bool editable;      // the node is editable or not
};

/**
 * @brief The ScreenLayoutRole enum screen layout model data role
 */
enum ScreenLayoutRole
{
    ReplaceRole = Qt::UserRole + 1,
    InsertRole,
    RemoveRole,
    BorderRole,    // the table cell border role
};

#define BORDER_LEFT         0x01
#define BORDER_TOP          0x02
#define BORDER_RIGHT        0x04
#define BORDER_BOTTOM       0x08

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

