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
    /**
     * @brief _setChangeMinAlarmVol 设置最小报警音量方法
     * @param index 选择序列号
     */
    void _setChangeMinAlarmVol(int index);
    /**
     * @brief _alarmPauseTime 设置报警暂停时间方法
     * @param index
     */
    void _alarmPauseTime(int index);
    /**
     * @brief _pauseMaxAlarm15MinSlot 设置是否允许最大报警暂停15min方法
     * @param index
     */
    void _pauseMaxAlarm15MinSlot(int index);
    /**
     * @brief _alarmVolSlot 设置报警声音方法
     * @param index
     */
    void _alarmVolSlot(int index);
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
     * @brief _ecgLeadOffLevSlot 设置ECG脱落级别方法
     * @param index
     */
    void _ecgLeadOffLevSlot(int index);
    /**
     * @brief _spo2LeadOffLevSlot 设置SpO2脱落级别方法
     * @param index
     */
    void _spo2LeadOffLevSlot(int index);
    /**
     * @brief _ibpLeadOffLevSlot 设置IBP脱落级别方法
     * @param index
     */
    void _ibpLeadOffLevSlot(int index);
    /**
     * @brief _lethalArrhOffSlot 设置是否关闭致命心率失常方法
     * @param index
     */
    void _lethalArrhOffSlot(int index);
    /**
     * @brief _extendedArrhSlot 设置是否拓展心率失常方法
     * @param index
     */
    void _extendedArrhSlot(int index);
    /**
     * @brief _alarmLightOnAlarmResetSlot 设置最小报警音量方法
     * @param index
     */
    void _alarmLightOnAlarmResetSlot(int index);
    /**
     * @brief _alarmDelaySlot 设置报警复位时报警灯方法
     * @param index
     */
    void _alarmDelaySlot(int index);
    /**
     * @brief _stAlarmDralySlot 设置报警延迟方法
     * @param index
     */
    void _stAlarmDralySlot(int index);
    /**
     * @brief _defaultsSlot 设置恢复默认值方法
     */
    void _defaultsSlot(void);
    /**
     * @brief _highAlarmIntervalsReleased 设置高级报警音间隔方法
     * @param strValue
     */
    void _highAlarmIntervalsReleased(QString strValue);
    /**
     * @brief _medAlarmIntervalsReleased 设置中级报警音间隔方法
     * @param strValue
     */
    void _medAlarmIntervalsReleased(QString strValue);
    /**
     * @brief _lowAlarmIntervalsReleased 设置低级报警音间隔方法
     * @param strValue
     */
    void _lowAlarmIntervalsReleased(QString strValue);
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
