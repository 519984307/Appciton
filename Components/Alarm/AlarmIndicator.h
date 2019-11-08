/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/12
 **/



#pragma once
#include <QList>
#include "AlarmIndicatorInterface.h"

class AlarmInfoBarWidget;
class AlarmStatusWidget;
class PatientInfoWidgetInterface;
class AlarmIndicator : public AlarmIndicatorInterface
{
public:
    static AlarmIndicator &getInstance(void);

    // 注册报警界面对象。
    void setAlarmPhyWidgets(AlarmInfoBarWidget *alarmWidget, AlarmStatusWidget *muteWidget,
                            PatientInfoWidgetInterface *patInfoWidget);
    void setAlarmTechWidgets(AlarmInfoBarWidget *alarmWidget);

    // 增加/删除报警消息。
    bool addAlarmInfo(unsigned alarmTime, AlarmType alarmType,
                      AlarmPriority alarmPriority, const char *alarmMessage,
                      AlarmParamIFace *alarmSource, int alarmID, bool isRemoveAfterLatch = false,
                      bool isRemoveLightAfterConfirm = true);
    void delAlarmInfo(AlarmType alarmType, const char *alarmMessage);

    // 报警栓锁
    bool latchAlarmInfo(AlarmType alarmType, const char *alarmMessage);
    bool updateLatchAlarmInfo(const char *alarmMessage, bool flag);

    // 删除栓锁的生理报警
    void delLatchPhyAlarm();

    // 是否有处于非暂停的生理报警
    bool hasNonPausePhyAlarm();

    // 是否有处于栓锁的生理报警
    bool hasLatchPhyAlarm();

    // 删除所有的生理报警
    void delAllPhyAlarm();

    // 清除报警Pause倒计时
    void clearAlarmPause();

    // 检查报警是否存在
    bool checkAlarmIsExist(AlarmType alarmType, const char *alarmMessage);

    // 更新报警级别
    bool updataAlarmPriority(AlarmType alarmType, const char *alArmMessage,
                         AlarmPriority priority);

    // 更新报警信息
    void updateAlarmInfo(const AlarmInfoNode &node);

    // 发布报警。
    void publishAlarm(AlarmStatus status);

    // 设置/获取报警状态。
    void setAlarmStatus(AlarmStatus status);

    // 获取当前报警的个数。
    int getAlarmCount(AlarmType type);
    int getAlarmCount();
    /**
     * @brief getAlarmCount 获取当前报警个数
     * @param priority  筛选出与其等级一致的报警
     * @return
     */
    int getAlarmCount(AlarmPriority priority);

    /**
     * @brief getAlarmCount 获取当前报警个数
     * @param type 筛选出报警类型一致的报警
     * @param priority 筛选出报警等级一致的报警
     * @return
     */
    int getAlarmCount(AlarmType type, AlarmPriority priority);

    AlarmInfoNode getAlarmInfo(int index);

    // 构造与析构。
    virtual ~AlarmIndicator();

    /**
     * @brief updateAlarmPauseTime udpate the alarm pause time
     * @param seconds the left pause time
     */
    void updateAlarmPauseTime(int seconds);

    /**
     * @brief phyAlarmPauseStatusHandle 报警暂停状态处理
     * @return
     */
    bool phyAlarmPauseStatusHandle();

    /**
     * @brief phyAlarmResetStatusHandle 处理复位后的生理报警状态
     * @return 是否有新被确认的生理报警
     */
    bool phyAlarmResetStatusHandle();
    /**
     * @brief techAlarmResetStatusHandle 处理复位后的技术报警状态
     * @return 是否有新被确认的技术报警
     */
    bool techAlarmResetStatusHandle();

    /**
     * @brief removeAllAlarmResetStatus 移除全部报警复位状态
     * @return
     */
    bool removeAllAlarmResetStatus();

    /**
     * @brief updateAlarmStateWidget 刷新报警状态图标
     */
    void updateAlarmAudioState();

    /**
     * @brief isAlarmAudioState
     * @return
     */
    bool isAlarmAudioState();

    /**
     * @brief setAlarmAudioState 设置报警音暂停状态
     * @param flag
     */
    void setAlarmAudioState(bool flag);

private:
    AlarmIndicator();

private:    // 报警信息显示。
    void _displayPhyClear(void);     // 清除生理报警界面。
    void _displayTechClear(void);    // 清除技术报警界面。
    void _displayPhySet(AlarmInfoNode node);  // 设置生理报警提示信息。
    void _displayTechSet(AlarmInfoNode node);  // 设置技术报警提示信息。
    bool _canPlayAudio(AlarmStatus status, bool isTechAlarm);   // check whether can play alarm sound

    AlarmInfoBarWidget *_alarmPhyInfoWidget;
    AlarmInfoBarWidget *_alarmTechInfoWidget;
    AlarmStatusWidget *_alarmStatusWidget;
    PatientInfoWidgetInterface *_patInfoWidget;

private:
    // 最大报警信息数量。
    #define ALARM_INFO_POOL_LEN 60
    typedef QList<AlarmInfoNode> AlarmInfoList;

    AlarmInfoList _alarmInfoDisplayPool;
    int _alarmPhyDisplayIndex;
    int _alarmTechDisplayIndex;

    void _displayInfoNode(AlarmInfoNode *alarmNode, int *indexint,
                          int newAlarmIndex, int oldAlarmIndex,
                          int firstIndex, int lastIndex);

private:
    AlarmStatus _audioStatus;
    int _audioPauseTime;

    bool _isForbidLight;
};
#define alarmIndicator (AlarmIndicator::getInstance())
