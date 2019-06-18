/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/

#pragma once

class RunningStatusBarInterface
{
public:
    virtual ~RunningStatusBarInterface(){}

    static RunningStatusBarInterface *registerRunningStatusBar(RunningStatusBarInterface *instance);

    static RunningStatusBarInterface *getRunningStatusBar(void);

    /**
     * @brief setNightModeStatus set the running night mode status
     * @param onOff
     */
    virtual void setNightModeStatus(bool onOff) = 0;

    /**
     * @brief setPacerStatus
     * @param onOff
     */
    virtual void setPacerStatus(bool onOff) = 0;
};
