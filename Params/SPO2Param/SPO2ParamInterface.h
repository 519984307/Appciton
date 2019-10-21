/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/10/21
 **/

#pragma once

class SPO2ParamInterface
{
public:
    virtual ~SPO2ParamInterface(){}

    static SPO2ParamInterface *registerSPO2Param(SPO2ParamInterface *handle);

    static SPO2ParamInterface *getSPO2Param(void);

    /**
     * @brief clearTrendWaveData 清除趋势波形的数据
     */
    virtual void clearTrendWaveData() = 0;
};
