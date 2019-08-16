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
     * @brief delAllPhyAlarm 删除所有的生理报警
     */
    virtual void delAllPhyAlarm() = 0;

    /**
     * @brief setAlarmStatus 设置/获取报警状态。
     * @param status
     */
    virtual void setAlarmStatus(AlarmStatus status) = 0;

    /**
     * @brief updateAlarmStateWidget 刷新报警状态图标
     */
    virtual void updateAlarmAudioState() = 0;

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

    /**
     * @brief updateAlarmPauseTime udpate the alarm pause time
     * @param seconds the left pause time
     */
    virtual void updateAlarmPauseTime(int seconds) = 0;

    /**
     * @brief updataAlarmPriority 更新报警等级
     * @param alarmType
     * @param alArmMessage
     * @param priority
     * @return
     */
    virtual bool updataAlarmPriority(AlarmType alarmType, const char *alArmMessage,
                         AlarmPriority priority) = 0;

    /**
     * @brief latchAlarmInfo
     * @param alarmType
     * @param alarmMessage
     * @return
     */
    virtual bool latchAlarmInfo(AlarmType alarmType, const char *alarmMessage) = 0;

    /**
     * @brief updateLatchAlarmInfo 报警栓锁
     * @param alarmMessage
     * @param flag
     * @return
     */
    virtual bool updateLatchAlarmInfo(const char *alarmMessage, bool flag) = 0;

    /**
     * @brief delAlarmInfo
     * @param alarmType
     * @param alarmMessage
     */
    virtual void delAlarmInfo(AlarmType alarmType, const char *alarmMessage) = 0;

    /**
     * @brief addAlarmInfo  增加/删除报警消息。
     * @param alarmTime
     * @param alarmType
     * @param alarmPriority
     * @param alarmMessage
     * @param alarmSource
     * @param alarmID
     * @param isRemoveAfterLatch
     * @param isRemoveLightAfterConfirm
     * @return
     */
    virtual bool addAlarmInfo(unsigned alarmTime, AlarmType alarmType,
                      AlarmPriority alarmPriority, const char *alarmMessage,
                      AlarmParamIFace *alarmSource, int alarmID, bool isRemoveAfterLatch = false,
                      bool isRemoveLightAfterConfirm = false) = 0;

    /**
     * @brief checkAlarmIsExist 检查报警是否存在
     * @param alarmType
     * @param alarmMessage
     * @return
     */
    virtual bool checkAlarmIsExist(AlarmType alarmType, const char *alarmMessage) = 0;

    /**
     * @brief publishAlarm  发布报警。
     * @param status
     */
    virtual void publishAlarm(AlarmStatus status) = 0;

    /**
     * @brief isAlarmAudioState
     */
    virtual bool isAlarmAudioState() = 0;

    /**
     * @brief setAlarmAudioState 设置报警音暂停状态
     * @param flag
     */
    virtual void setAlarmAudioState(bool flag) = 0;

    /**
     * @brief removeAllAlarmResetStatus 移除全部报警复位状态
     * @return
     */
    virtual bool removeAllAlarmResetStatus() = 0;

    // 清除报警Pause倒计时
    virtual void clearAlarmPause() = 0;
};
