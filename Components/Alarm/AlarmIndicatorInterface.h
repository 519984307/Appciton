/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/2/27
 **/
#pragma once

#include "AlarmDefine.h"
class AlarmIndicatorInterface
{
public:
    AlarmIndicatorInterface(){}
    virtual ~AlarmIndicatorInterface(){}

    /**
     * @brief registerAlarmIndicator 注册报警指示器
     * @return
     */
    static AlarmIndicatorInterface *registerAlarmIndicator(AlarmIndicatorInterface* interface);

    /**
     * @brief getAlarmIndicator 获取报警指示器句柄
     * @return
     */
    static AlarmIndicatorInterface *getAlarmIndicator();

    /**
     * @brief delLatchPhyAlarm 删除所有的生理报警
     */
    virtual void delLatchPhyAlarm() = 0;

    /**
     * @brief setAlarmStatus 设置/获取报警状态。
     * @param status
     */
    virtual void setAlarmStatus(AlarmStatus status) = 0;

    /**
     * @brief updateAlarmStateWidget 刷新报警状态图标
     */
    virtual void updateAlarmStateWidget() = 0;

    /**
     * @brief phyAlarmResetStatusHandle 处理复位后的生理报警状态
     * @return 是否有新被确认的生理报警
     */
    virtual bool phyAlarmResetStatusHandle() = 0;
    /**
     * @brief techAlarmResetStatusHandle 处理复位后的技术报警状态
     * @return 是否有新被确认的技术报警
     */
    virtual bool techAlarmResetStatusHandle() = 0;

    /**
     * @brief getAlarmCount 获取当前报警个数
     * @param priority  筛选出与其等级一致的报警
     * @return
     */
    virtual int getAlarmCount(AlarmType type) = 0;
    virtual int getAlarmCount() = 0;
    virtual int getAlarmCount(AlarmPriority priority) = 0;

    /**
     * @brief phyAlarmPauseStatusHandle 报警暂停状态处理
     * @return
     */
    virtual bool phyAlarmPauseStatusHandle() = 0;
};
