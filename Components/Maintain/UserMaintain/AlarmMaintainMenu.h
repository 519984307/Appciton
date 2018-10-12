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
#include "SubMenu.h"
#include "AlarmLimitMenu.h"
class LabelButton;
class IComboList;
class AlarmMaintainMenu : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static AlarmMaintainMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new AlarmMaintainMenu();
        }
        return *_selfObj;
    }
    static AlarmMaintainMenu *_selfObj;
    ~AlarmMaintainMenu();
protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  数据加载方法
     */
    virtual void readyShow(void);
private slots:
    void _minAlarmVolumeChangeSlot(int index);
    void _alarmPauseTimeChangeSlot(int index);
    void _alarmClosePromptTimeChangeSlot(int index);
    void _alarmOffAtPowerOnChangeSlot(int index);
    void _enableAlarmAudioOffChangeSlot(int index);
    void _enableAlarmOffChangeSlot(int index);
    /**
     * @brief _pauseMaxAlarm15MinSlot 设置是否允许最大报警暂停15min方法
     * @param index
     */
    void _pauseMaxAlarm15MinSlot(int index);
    /**
     * @brief _alarmVolSlot 设置报警声音方法
     * @param index
     */
//    void _alarmVolSlot(int index);
    /**
     * @brief _reminderToneSlot 设置报警提示音方法
     * @param index
     */
    void _reminderToneSlot(int index);
    /**
     * @brief _reminderToneIntervalsSlot 设置报警提示音间隔时间方法
     * @param index
     */
    void _reminderToneIntervalsSlot(int index);
    /**
     * @brief _alarmLightOnAlarmResetSlot 设置最小报警音量方法
     * @param index
     */
    void _alarmLightOnAlarmResetSlot(int index);
    /**
     * @brief _defaultsSlot 设置恢复默认值方法
     */
    void _defaultsSlot(void);

    /**
     * @brief _boltLockComboListSlot 栓锁设置项槽函数
     * @param index
     */
    void _boltLockComboListSlot(int index);
private:
    AlarmMaintainMenu();
    IComboList *_minAlarmVolume;            // 最小报警音
    IComboList *_alarmPauseTime;            // 报警暂停时间
    IComboList *_alarmClosePromptTime;      // 报警关闭，报警音关闭提示时间
    IComboList *_enableAlarmAudioOff;       // 使能报警声音关闭
    IComboList *_enableAlarmOff;            // 使能报警关闭
    IComboList *_alarmOffAtPowerOn;         // 开机报警关闭

    IComboList  *_isPauseMaxAlarm15Min;     // 是否允许最大报警暂停15min
//    IComboList  *_alarmVol;               // 报警声音
    IComboList  *_reminderTone;             // 报警提示音
    IComboList  *_reminderToneIntervals;    // 报警提示音间隔时间
    IComboList  *_alarmLightOnAlarmReset;   // 报警复位时报警灯
    LabelButton *_defaults;                 // 恢复默认值
    IComboList *_boltLockComboList;         // 报警栓锁开关
};
#define alarmMaintainMenu (AlarmMaintainMenu::construction())
