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
#include "ParamDefine.h"

class ParamInfoInterface
{
public:
    virtual ~ParamInfoInterface(){}

    static ParamInfoInterface *registerParamInfo(ParamInfoInterface *instance);

    static ParamInfoInterface *getParamInfo(void);

    virtual ParamID getParamID(SubParamID id) = 0;
};
