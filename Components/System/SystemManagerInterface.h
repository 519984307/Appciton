/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/6
 **/

#pragma once
#include "SystemDefine.h"

enum WorkMode
{
    WORK_MODE_NORMAL = 0,
    WORK_MODE_DEMO,
    WORK_MODE_STANDBY,
    WORK_MODE_NR,
};

class SystemManagerInterface
{
public:
    virtual ~SystemManagerInterface(){}

    /**
     * @brief registerSystemManager register system manager handle
     * @param instance
     * @return
     */
    static SystemManagerInterface *registerSystemManager(SystemManagerInterface *instance);

    /**
     * @brief getSystemManager get system manager handle
     * @return
     */
    static SystemManagerInterface *getSystemManager(void);

    // 背光亮度设置/获取。
    virtual void setBrightness(BrightnessLevel br) = 0;
    virtual void enableBrightness(int br) = 0;
    virtual BrightnessLevel getBrightness(void) = 0;

    /**
     * @brief getCurWorkMode get the current work mode
     * @return the current work mode
     */
    virtual WorkMode getCurWorkMode() const = 0;
};
