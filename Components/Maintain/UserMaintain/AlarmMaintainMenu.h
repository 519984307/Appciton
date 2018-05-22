#pragma once
#include "SubMenu.h"
#include "AlarmLimitMenu.h"
class LabelButton;
class IComboList;
class AlarmMaintainMenu : public SubMenu
{
    Q_OBJECT

public:
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
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _setChangeMinAlarmVol(int);
    void _alarmPauseTime(int);
    void _pauseMaxAlarm15MinSlot(int);
    void _alarmVolSlot(int);
    void _reminderToneSlot(int);
    void _reminderToneIntervalsSlot(int);
    void _ecgLeadOffLevSlot(int);
    void _spo2LeadOffLevSlot(int);
    void _ibpLeadOffLevSlot(int);
    void _lethalArrhOffSlot(int);
    void _extendedArrhSlot(int);
    void _alarmLightOnAlarmResetSlot(int);
    void _alarmDelaySlot(int);
    void _stAlarmDralySlot(int);
    void _defaultsSlot(void);
    void _highAlarmIntervalsReleased(QString);
    void _medAlarmIntervalsReleased(QString);
    void _lowAlarmIntervalsReleased(QString);

private:
    AlarmMaintainMenu();

    IComboList  *_minAlarmVol;             //最小报警音量
    IComboList  *_pauseAlarmTime;          //报警暂停时间
    IComboList  *_isPauseMaxAlarm15Min;    //是否允许最大报警暂停15min
    IComboList  *_alarmVol;                //报警声音
    ISpinBox    *_highAlarmIntervals;      //高级报警音间隔
    ISpinBox    *_medAlarmIntervals;       //中级报警音间隔
    ISpinBox    *_lowAlarmIntervals;       //低级报警音间隔
    IComboList  *_reminderTone;            //报警提示音
    IComboList  *_reminderToneIntervals;   //报警提示音间隔时间
    IComboList  *_ecgLeadOffLev;           //ECG脱落级别
    IComboList  *_spo2LeadOffLev;          //SpO2脱落级别
    IComboList  *_ibpLeadOffLev;           //IBP脱落级别
    IComboList  *_isLethalArrhOff;         //是否关闭致命心率失常
    IComboList  *_isExtendedArrh;          //是否拓展心率失常
    IComboList  *_alarmLightOnAlarmReset;  //报警复位时报警灯
    IComboList  *_alarmDelay;              //报警延迟
    IComboList  *_stAlarmDraly;            //ST报警延迟
    LabelButton *_defaults;                //恢复默认值

};
#define alarmMaintainMenu (AlarmMaintainMenu::construction())
