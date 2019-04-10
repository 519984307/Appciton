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
#include "ECGDefine.h"

class ECGParamInterface
{
public:
    virtual ~ECGParamInterface(){}

    static ECGParamInterface *registerECGParam(ECGParamInterface *instance);

    static ECGParamInterface *getECGParam(void);

    virtual ECGLeadNameConvention getLeadConvention(void) const = 0;
};
