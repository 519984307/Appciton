/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/19
 **/

#pragma once
#include <stddef.h>
#include "TimeManagerInterface.h"

class DateTimeWidget;
class ElapseTimeWidget;
class QDateTime;
class TimeManager : public TimeManagerInterface
{
public:
    static TimeManager &getInstance(void);

    // 注册窗体界面。
    void registerWidgets(DateTimeWidget *widget, ElapseTimeWidget *elapseWidget);

    // 主运行。
    void mainRun(unsigned t);

    /**
     * @brief recordRunTime  记录运行时间 默认执行周期1S
     */
    void recordRunTime(void);

    /**
     * @brief getRunTime  获取运行时间
     */
    unsigned getRunTime(void) const;

    // 获取开机时的状态。
    virtual PowerOnSession getPowerOnSession(void) const
    {
        return _powerOnSession;
    }

    // 获取开始时间。
    virtual unsigned getStartTime(void) const
    {
        return _elapseStartTime;
    }

    // 获取系统当前时间
    virtual unsigned getCurTime() const
    {
        return _curTime;
    }

    // get elapsed time
    unsigned getElapsedTime() const
    {
        return _curTime - _elapseStartTime; // 多线程访问。
    }

    // set elapsed time
    virtual void setElapsedTime(void);

    /**
     * @brief roloadConfig 重新加载配置文件
     */
    void roloadConfig();
    ~TimeManager();


    /**
     * @brief setSystemTime set the system datetime
     * @param datetime the setting datetime
     */
    void setSystemTime(const QDateTime &datetime);

    /**
     * @brief checkAndFixSystemTime check and fix the system time,
     *        should be called at startup
     */
    void checkAndFixSystemTime();

    bool isShowSecond(void);

private:
    TimeManager();
    void _refreshWidgets(void);

    DateTimeWidget *_dateTimeWidget;
    ElapseTimeWidget *_elapsedWidget;

    PowerOnSession _powerOnSession;
    unsigned _elapseStartTime;
    unsigned _curTime;
    bool _showSecond;                 //是否显示秒
    unsigned _runTime;
};
#define timeManager (TimeManager::getInstance())
