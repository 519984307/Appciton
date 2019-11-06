/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/


#pragma once
#include <stddef.h>
#include "Framework/Utility/Unit.h"
#include "ParamInfoInterface.h"

class ParamInfo : public ParamInfoInterface
{
public:
    static ParamInfo &getInstance(void);
    ~ParamInfo();

public:
    // 根据参数的ID查询对应的名字。
    const char *getParamName(ParamID id);
    ParamID getParamID(const QString &name);

    // Param 与 SubParam之间转换。
    ParamID getParamID(SubParamID id);
    void getSubParams(ParamID id, QList<SubParamID> &ids);  // NOLINT
    const char *getSubParamName(SubParamID id, bool ignoreModuleConfig = false);
    const char *getSubParamName(SubDupParamID id);
    const char *getIBPPressName(SubParamID id);

    // 获取波形ID与字串名之间的转换。
    const char *getParamWaveformName(WaveformID id);
    ParamID getParamID(WaveformID id);

    // 获取参数存放趋势数据时的放大倍数。
    int getMultiOfSubParam(SubParamID id);
    int getMultiOfSubParam(const char *name);

    // 获取子参数的默认单位。
    UnitType getUnitOfSubParam(SubParamID id);

    // 查询子参数的默认单位。
    UnitType getUnitOfSubParam(SubParamID id, UnitType &t0, UnitType &t1);  // NOLINT

    // sort the param id
    static void sortSubParamId(QList<SubParamID> &subIDlist);  // NOLINT

private:
    ParamInfo();
};
#define paramInfo (ParamInfo::getInstance())
