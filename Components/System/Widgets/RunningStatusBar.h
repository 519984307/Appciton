/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/20
 **/

#pragma once
#include "IWidget.h"
#include "RunningStatusBarInterface.h"

enum ShakeStatus
{
    SHAKE_OFF,
    SHAKE_ON,
    SHAKING
};

class RunningStatusBarPrivate;

class RunningStatusBar : public IWidget, public RunningStatusBarInterface
{
public:
    static RunningStatusBar &getInstance();

    ~RunningStatusBar();


    /**
     * @brief setPacerStatus
     * @param onOff
     */
    void setPacerStatus(bool onOff);

    /**
     * @brief setNightModeStatus set the running night mode status
     * @param onOff
     */
    void setNightModeStatus(bool onOff);

    /**
     * @brief setTouchStatus set the touch screen status
     * @param onOff
     */
    void setTouchStatus(bool onOff);

    /**
     * @brief clearTouchStatus clear the touch status, should be called when the touch screen function is disabled
     */
    void clearTouchStatus();

    /**
     * @brief setShakeStatus set apnea shake status
     * @param sta
     */
    void setShakeStatus(ShakeStatus sta);
    ShakeStatus getShakeStatus(void);

private:
    RunningStatusBar();
    RunningStatusBarPrivate *const d_ptr;
};

#define runningStatus (RunningStatusBar::getInstance())
