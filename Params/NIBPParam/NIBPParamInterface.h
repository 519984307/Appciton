/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/6
 **/
#include <QString>
#pragma once

class NIBPParamInterface
{
public:
    virtual ~NIBPParamInterface(){}

    static NIBPParamInterface *registerNIBPParam(NIBPParamInterface *instance);

    static NIBPParamInterface *getNIBPParam(void);

    /**
     * @brief stopMeasure 停止测量
     */
    virtual void stopMeasure(void) = 0;

    virtual void setText(QString text) = 0;

    virtual void clearResult(void) = 0;
};
