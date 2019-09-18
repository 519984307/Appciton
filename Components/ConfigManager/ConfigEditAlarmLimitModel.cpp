/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/19
 **/

#include "ConfigEditAlarmLimitModel.h"
#include "PatientManager.h"
#include "ParamInfo.h"
#include "ParamManager.h"

class ConfigEditAlarmLimitModelPrivate
{
public:
    explicit ConfigEditAlarmLimitModelPrivate(Config *config)
                    : config(config)
    {
    }
    Config *config;
};

ConfigEditAlarmLimitModel::ConfigEditAlarmLimitModel(Config *config)
                         : AlarmLimitModel()
                         , d_ptr(new ConfigEditAlarmLimitModelPrivate(config))
{
}

ConfigEditAlarmLimitModel::~ConfigEditAlarmLimitModel()
{
   delete d_ptr;
}

void ConfigEditAlarmLimitModel::alarmDataUpdate(const AlarmDataInfo &info, int type)
{
    Config *config = d_ptr->config;
    if (!config)
    {
        return;
    }

    UnitType unit  = paramManager.getSubParamUnit(info.paramID, info.subParamID);

    QString prefix = "AlarmSource|";
    prefix += paramInfo.getSubParamName(info.subParamID, true);

    switch (type)
    {
    case SECTION_STATUS:
    {
        int status = info.status ? 1 : 0;
        config->setNumAttr(prefix, "Enable", status);
        break;
    }
    case SECTION_LEVEL:
        config->setNumAttr(prefix, "Prio", info.alarmLevel);
        break;
    case SECTION_HIGH_LIMIT:
        prefix += "|";
        prefix += Unit::getSymbol(unit);
        config->setNumValue((prefix + "|High"), info.limitConfig.highLimit);
        break;
    case SECTION_LOW_LIMIT:
        prefix += "|";
        prefix += Unit::getSymbol(unit);
        config->setNumValue((prefix + "|Low"), info.limitConfig.lowLimit);
        break;
    }
}





