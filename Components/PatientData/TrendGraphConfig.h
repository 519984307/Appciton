/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/10/31
 **/

#pragma once
#include "ParamInfo.h"

struct ParamRulerConfig
{
    ParamRulerConfig()
        : upRuler(0), minUpRuler(0), maxUpRuler(0), downRuler(0), minDownRuler(0), maxDownRuler(0), scale(1)
    {}

    int upRuler;
    int minUpRuler;
    int maxUpRuler;
    int downRuler;
    int minDownRuler;
    int maxDownRuler;
    int scale;
};

class TrendGraphConfig
{
public:
    TrendGraphConfig();

    /**
     * @brief getParamRulerConfig 获取参数默认标尺配置
     * @param subParamId 参数ID
     * @param unit
     * @return
     */
    static ParamRulerConfig getParamRulerConfig(SubParamID subParamId, UnitType unit);

    /**
     * @brief setParamRulerConfig 设置参数默认标尺配置
     * @param subParamID 参数ID
     * @param unit 单位
     * @param low
     * @param high
     */
    static void setParamRulerConfig(SubParamID subParamID, UnitType unit, int low, int high);
};
