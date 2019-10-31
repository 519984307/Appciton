/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/10/31
 **/

#include "TrendGraphConfig.h"
#include "IConfig.h"

TrendGraphConfig::TrendGraphConfig()
{
}

ParamRulerConfig TrendGraphConfig::getParamRulerConfig(SubParamID subParamId, UnitType unit)
{
    ParamRulerConfig config;

    // load data from config file
    QString prefix = "TrendGraph|Ruler|";
    prefix += paramInfo.getSubParamName(subParamId, true);
    prefix += "|";
    prefix += Unit::getSymbol(unit);
    prefix += "|";
    int v = 0;
    QString highPrefix = prefix + "High";
    systemConfig.getNumValue(highPrefix, v);
    config.upRuler = v;

    v = 0;
    systemConfig.getNumAttr(highPrefix, "Min", v);
    config.minUpRuler = v;

    v = 0;
    systemConfig.getNumAttr(highPrefix, "Max", v);
    config.maxUpRuler = v;

    v = 0;
    QString lowPrefix = prefix + "Low";
    systemConfig.getNumValue(lowPrefix, v);
    config.downRuler = v;

    v = 0;
    systemConfig.getNumAttr(lowPrefix, "Min", v);
    config.minDownRuler = v;

    v = 0;
    systemConfig.getNumAttr(lowPrefix, "Max", v);
    config.maxDownRuler = v;

    v = 1;
    systemConfig.getNumValue(prefix + "Scale", v);
    config.scale = v;

    return config;
}

void TrendGraphConfig::setParamRulerConfig(SubParamID subParamID, UnitType unit, int low, int high)
{
    // load data from config file
    QString prefix = "TrendGraph|Ruler|";
    prefix += paramInfo.getSubParamName(subParamID, true);
    prefix += "|";
    prefix += Unit::getSymbol(unit);
    prefix += "|";
    QString highPrefix = prefix + "High";
    systemConfig.setNumValue(highPrefix, high);

    QString lowPrefix = prefix + "Low";
    systemConfig.setNumValue(lowPrefix, low);
}
