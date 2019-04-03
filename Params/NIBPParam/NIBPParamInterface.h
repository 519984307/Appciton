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

#include "NIBPDefine.h"
#include "NIBPProviderIFace.h"
#include <QString>
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

    virtual void setAutoMeasure(bool flag) = 0;

    virtual void setSTATMeasure(bool flag) = 0;

    virtual void switchToAuto(void) = 0;

    virtual void switchToManual(void) = 0;

    virtual void setModelText(QString text) = 0;

    virtual NIBPMode getSuperMeasurMode(void) = 0;

    virtual NIBPAutoInterval getAutoInterval(void) = 0;

    virtual NIBPMode getMeasurMode(void) = 0;

    virtual void createSnapshot(NIBPOneShotType) = 0;

    virtual NIBPProviderIFace &provider(void) = 0;

    virtual void setAdditionalMeasure(bool flag) = 0;

    virtual void setSTATClose(bool flag) = 0;

    virtual bool isAutoMeasure() = 0;
};