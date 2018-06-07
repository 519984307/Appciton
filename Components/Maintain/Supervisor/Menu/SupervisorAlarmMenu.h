#pragma once
#include "SubMenu.h"

class IComboList;

//声音配置
class SupervisorAlarmMenu : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorAlarmMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SupervisorAlarmMenu();
        }

        return *_selfObj;
    }
    static SupervisorAlarmMenu *_selfObj;

    ~SupervisorAlarmMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
//    void _ltaAlarmChangeSlot(int index);
    void _minAlarmVolumeChangeSlot(int index);
    void _alarmPauseTimeChangeSlot(int index);
    void _alarmClosePromptTimeChangeSlot(int index);
    void _alarmOffAtPowerOnChangeSlot(int index);
    void _enableAlarmAudioOffChangeSlot(int index);
    void _enableAlarmOffChangeSlot(int index);
    void _nonAlertsBeepsInNonAEDSlot(int index);

private:
    SupervisorAlarmMenu();

//    IComboList *_ltaAlarm;            // 生命报警开关
    IComboList *_minAlarmVolume;      // 最小报警音
    IComboList *_alarmPauseTime;      // 报警暂停时间
    IComboList *_alarmClosePromptTime;// 报警关闭，报警音关闭提示时间
    IComboList *_enableAlarmAudioOff; // 使能报警声音关闭
    IComboList *_enableAlarmOff;      // 使能报警关闭
    IComboList *_alarmOffAtPowerOn;   // 开机报警关闭
    IComboList *_nonAlertsBeepsInNonAED;   // Non-alerts beeps in Non-AED modes
};
#define supervisorAlarmMenu (SupervisorAlarmMenu::construction())

