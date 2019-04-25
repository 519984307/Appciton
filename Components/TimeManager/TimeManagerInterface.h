/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/20
 **/

#pragma once

enum PowerOnSession
{
    POWER_ON_SESSION_CONTINUE,
    POWER_ON_SESSION_NEW,
    POWER_ON_SESSION_NR
};

class TimeManagerInterface
{
public:
    virtual ~TimeManagerInterface(){}

    /**
     * @brief registerTimeManager register TimeManager handle
     * @param instance
     * @return
     */
    static TimeManagerInterface *registerTimeManager(TimeManagerInterface *instance);

    /**
     * @brief getTimeManager get TimeManager handle
     * @return
     */
    static TimeManagerInterface *getTimeManager();

    // 获取系统当前时间
    virtual unsigned getCurTime() const = 0;

    // set elapsed time
    virtual void setElapsedTime(void) = 0;

    // 获取开机时的状态。
    virtual PowerOnSession getPowerOnSession(void) const = 0;

    // 获取开始时间。
    virtual unsigned getStartTime(void) const = 0;
};
