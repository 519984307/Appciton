/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/3
 **/

#include "AlarmMaintainSymbol.h"
#include "AlarmMaintainMenu.h"

#include <QVBoxLayout>
#include <QRegExp>
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IButton.h"
#include "IComboList.h"
#include "SupervisorMenuManager.h"
#include "SupervisorConfigManager.h"
#include "PatientDefine.h"
#include "SoundManager.h"
#include "KeyBoardPanel.h"
#include "Debug.h"
#include "AlarmSymbol.h"
#include "AlarmStateDefine.h"
#include "UserMaintainManager.h"
#include "Alarm.h"

AlarmMaintainMenu *AlarmMaintainMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmMaintainMenu::AlarmMaintainMenu() : SubMenu(trs("AlarmMaintainMenu")),
    _minAlarmVolume(NULL),
    _alarmPauseTime(NULL),
    _alarmClosePromptTime(NULL),
    _enableAlarmAudioOff(NULL),
    _enableAlarmOff(NULL),
    _alarmOffAtPowerOn(NULL),
    _isPauseMaxAlarm15Min(NULL),
    _reminderTone(NULL),
    _reminderToneIntervals(NULL),
    _alarmLightOnAlarmReset(NULL),
    _defaults(NULL)
{
    setDesc(trs("AlarmMaintainMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void AlarmMaintainMenu::readyShow()
{
//    QString tmpStr;
    int index;

    systemConfig.getNumValue("Alarms|MinimumAlarmVolume", index);
    _minAlarmVolume->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|AlarmPauseTime", index);
    _alarmPauseTime->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|AlarmOffPrompting", index);
    _alarmClosePromptTime->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|EnableAlarmAudioOff", index);
    _enableAlarmAudioOff->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|EnableAlarmOff", index);
    _enableAlarmOff->setCurrentIndex(index);

    if (0 == index)
    {
        _alarmOffAtPowerOn->setDisabled(true);
    }
    else
    {
        _alarmOffAtPowerOn->setDisabled(false);
    }
    systemConfig.getNumValue("Alarms|AlarmOffAtPowerOn", index);
    _alarmOffAtPowerOn->setCurrentIndex(index);

//    systemConfig.getNumValue("Alarms|NonAlertsBeepsInNonAED", index);
//    _nonAlertsBeepsInNonAED->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|PauseMaxAlarm15Min", index);
    _isPauseMaxAlarm15Min->setCurrentIndex(index);
    index = 0;

//    systemConfig.getNumValue("Alarms|AlarmVol", index);
//    _alarmVol->setCurrentIndex(index);
//    index = 0;

    systemConfig.getNumValue("Alarms|ReminderToneIntervals", index);
    _reminderToneIntervals->setCurrentIndex(index);
    index = 0;

    systemConfig.getNumValue("Alarms|AlarmLightOnAlarmReset", index);
    _alarmLightOnAlarmReset->setCurrentIndex(index);
    index = 0;

    systemConfig.getNumValue("Alarms|ReminderTone", index);
    _reminderTone->setCurrentIndex(index);
    index = 0;
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void AlarmMaintainMenu::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    // 最小报警音量
    _minAlarmVolume = new IComboList(trs("MinimumALarmVolume"));
    _minAlarmVolume->setFont(fontManager.textFont(fontSize));
    _minAlarmVolume->label->setFixedSize(labelWidth, ITEM_H);
    _minAlarmVolume->combolist->setFixedSize(btnWidth, ITEM_H);

    for (unsigned i = SoundManager::VOLUME_LEV_1; i <= SoundManager::VOLUME_LEV_5; ++i)
    {
        _minAlarmVolume->addItem(QString::number(i));
    }
    connect(_minAlarmVolume, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_minAlarmVolumeChangeSlot(int)));
    mainLayout->addWidget(_minAlarmVolume);

    // 报警暂停时间
    _alarmPauseTime = new IComboList(trs("AlarmPauseTime"));
    _alarmPauseTime->setFont(fontManager.textFont(fontSize));
    _alarmPauseTime->label->setFixedSize(labelWidth, ITEM_H);
    _alarmPauseTime->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < ALARM_PAUSE_TIME_NR; ++i)
    {
        _alarmPauseTime->addItem(trs(AlarmSymbol::convert((AlarmPauseTime)i)));
    }

    connect(_alarmPauseTime, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_alarmPauseTimeChangeSlot(int)));
    mainLayout->addWidget(_alarmPauseTime);

    // 报警关闭，报警音关闭提示时间
    _alarmClosePromptTime = new IComboList(trs("AlarmOffPromptMechanism"));
    _alarmClosePromptTime->setFont(fontManager.textFont(fontSize));
    _alarmClosePromptTime->label->setFixedSize(labelWidth, ITEM_H);
    _alarmClosePromptTime->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < ALARM_CLOSE_PROMPT_NR; ++i)
    {
        _alarmClosePromptTime->addItem(trs(AlarmSymbol::convert((AlarmClosePromptTime)i)));
    }

    connect(_alarmClosePromptTime, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_alarmClosePromptTimeChangeSlot(int)));
    mainLayout->addWidget(_alarmClosePromptTime);

    // eable alarm audio off
    _enableAlarmAudioOff = new IComboList(trs("EnableAlarmAudioOff"));
    _enableAlarmAudioOff->setFont(fontManager.textFont(fontSize));
    _enableAlarmAudioOff->label->setFixedSize(labelWidth, ITEM_H);
    _enableAlarmAudioOff->combolist->setFixedSize(btnWidth, ITEM_H);

    _enableAlarmAudioOff->addItem(trs("No"));
    _enableAlarmAudioOff->addItem(trs("Yes"));

    connect(_enableAlarmAudioOff, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_enableAlarmAudioOffChangeSlot(int)));
    mainLayout->addWidget(_enableAlarmAudioOff);

    // enable alarm off
    _enableAlarmOff = new IComboList(trs("EnableAlarmOff"));
    _enableAlarmOff->setFont(fontManager.textFont(fontSize));
    _enableAlarmOff->label->setFixedSize(labelWidth, ITEM_H);
    _enableAlarmOff->combolist->setFixedSize(btnWidth, ITEM_H);

    _enableAlarmOff->addItem(trs("No"));
    _enableAlarmOff->addItem(trs("Yes"));

    connect(_enableAlarmOff, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_enableAlarmOffChangeSlot(int)));
    mainLayout->addWidget(_enableAlarmOff);

    // alarm off at power on
    _alarmOffAtPowerOn = new IComboList(trs("AlarmOffAtPowerOn"));
    _alarmOffAtPowerOn->setFont(fontManager.textFont(fontSize));
    _alarmOffAtPowerOn->label->setFixedSize(labelWidth, ITEM_H);
    _alarmOffAtPowerOn->combolist->setFixedSize(btnWidth, ITEM_H);

    _alarmOffAtPowerOn->addItem(trs("No"));
    _alarmOffAtPowerOn->addItem(trs("Yes"));

    connect(_alarmOffAtPowerOn, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_alarmOffAtPowerOnChangeSlot(int)));
    mainLayout->addWidget(_alarmOffAtPowerOn);

//    _nonAlertsBeepsInNonAED = new IComboList(trs("NonAlertsBeepsInNonAED"));
//    _nonAlertsBeepsInNonAED->setFont(fontManager.textFont(fontSize));
//    _nonAlertsBeepsInNonAED->label->setFixedSize(labelWidth, ITEM_H);
//    _nonAlertsBeepsInNonAED->combolist->setFixedSize(btnWidth, ITEM_H);
//    _nonAlertsBeepsInNonAED->addItem(trs("Disable"));
//    _nonAlertsBeepsInNonAED->addItem(trs("Enable"));
//    connect(_nonAlertsBeepsInNonAED, SIGNAL(currentIndexChanged(int)), this,
//            SLOT(_nonAlertsBeepsInNonAEDSlot(int)));
//    mainLayout->addWidget(_nonAlertsBeepsInNonAED);

    // _isPauseMaxAlarm15Min
    _isPauseMaxAlarm15Min = new IComboList(trs("IsPauseMaxAlarm15Min"));
    _isPauseMaxAlarm15Min->label->setFixedSize(labelWidth, ITEM_H);
    _isPauseMaxAlarm15Min->combolist->setFixedSize(btnWidth, ITEM_H);
    _isPauseMaxAlarm15Min->setFont(fontManager.textFont(fontSize));
    _isPauseMaxAlarm15Min->addItem(trs("forbid"));
    _isPauseMaxAlarm15Min->addItem(trs("allow"));
    connect(_isPauseMaxAlarm15Min->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_pauseMaxAlarm15MinSlot(int)));
    mainLayout->addWidget(_isPauseMaxAlarm15Min);


    // _alarmVol
//    _alarmVol = new IComboList(trs("AlarmVol"));
//    _alarmVol->label->setFixedSize(labelWidth, ITEM_H);
//    _alarmVol->combolist->setFixedSize(btnWidth, ITEM_H);
//    _alarmVol->setFont(fontManager.textFont(fontSize));
//    _alarmVol->addItem(trs("ISO"));
//    _alarmVol->addItem(trs("mode 1"));
//    _alarmVol->addItem(trs("mode 2"));
//    connect(_alarmVol->combolist, SIGNAL(currentIndexChanged(int)),
//            this, SLOT(_alarmVolSlot(int)));
//    mainLayout->addWidget(_alarmVol);

    // _reminderTone
    _reminderTone = new IComboList(trs("ReminderTone"));
    _reminderTone->label->setFixedSize(labelWidth, ITEM_H);
    _reminderTone->combolist->setFixedSize(btnWidth, ITEM_H);
    _reminderTone->setFont(fontManager.textFont(fontSize));
    _reminderTone->addItem(trs("close"));
    _reminderTone->addItem(trs("open"));
    _reminderTone->addItem(trs("openAgain"));
    connect(_reminderTone->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_reminderToneSlot(int)));
    mainLayout->addWidget(_reminderTone);

    // _reminderToneIntervals
    _reminderToneIntervals = new IComboList(trs("ReminderToneIntervals"));
    _reminderToneIntervals->label->setFixedSize(labelWidth, ITEM_H);
    _reminderToneIntervals->combolist->setFixedSize(btnWidth, ITEM_H);
    _reminderToneIntervals->setFont(fontManager.textFont(fontSize));
    _reminderToneIntervals->addItem(trs("H1min"));
    _reminderToneIntervals->addItem(trs("H2min"));
    _reminderToneIntervals->addItem(trs("H3min"));
    connect(_reminderToneIntervals->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_reminderToneIntervalsSlot(int)));
    mainLayout->addWidget(_reminderToneIntervals);

    // _alarmLightOnAlarmReset
    _alarmLightOnAlarmReset = new IComboList(trs("_alarmLightOnAlarmReset"));
    _alarmLightOnAlarmReset->label->setFixedSize(labelWidth, ITEM_H);
    _alarmLightOnAlarmReset->combolist->setFixedSize(btnWidth, ITEM_H);
    _alarmLightOnAlarmReset->setFont(fontManager.textFont(fontSize));
    _alarmLightOnAlarmReset->addItem(trs("close"));
    _alarmLightOnAlarmReset->addItem(trs("open"));
    connect(_alarmLightOnAlarmReset->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_alarmLightOnAlarmResetSlot(int)));
    mainLayout->addWidget(_alarmLightOnAlarmReset);

    // Defaults
    _defaults = new LabelButton(trs("Defaults"));
    _defaults->setFont(fontManager.textFont(fontSize));
    _defaults->label->setFixedSize(labelWidth, ITEM_H);
    _defaults->button->setFixedSize(btnWidth, ITEM_H);
    _defaults->button->setText(trs("RecoverDefaults"));
    connect(_defaults->button, SIGNAL(released(int)), this, SLOT(_defaultsSlot()));
    mainLayout->addWidget(_defaults);

    _boltLockComboList = new IComboList(trs("LatchLockSwitch"));
    _boltLockComboList->label->setFixedSize(labelWidth, ITEM_H);
    _boltLockComboList->combolist->setFixedSize(btnWidth, ITEM_H);
    _boltLockComboList->setFont(fontManager.textFont(fontSize));
    _boltLockComboList->addItem(trs("Off"));
    _boltLockComboList->addItem(trs("On"));
    int boltLockIndex = 0;
    systemConfig.getNumValue("Alarms|PhyParAlarmLatchlockOn", boltLockIndex);
    _boltLockComboList->combolist->setCurrentIndex(boltLockIndex);
    connect(_boltLockComboList->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_boltLockComboListSlot(int)));
    mainLayout->addWidget(_boltLockComboList);
}

/**************************************************************************************************
 * MonitorName槽函数。
 *************************************************************************************************/
void AlarmMaintainMenu::_defaultsSlot()
{
}

void AlarmMaintainMenu::_boltLockComboListSlot(int index)
{
    systemConfig.setNumValue("Alarms|PhyParAlarmLatchlockOn", index);
    if (index == 0)
    {
        alertor.setLatchLockSta(false);
    }
    else
    {
        alertor.setLatchLockSta(true);
    }
}
/**************************************************************************************************
 * 最低报警音量。
 *************************************************************************************************/
void AlarmMaintainMenu::_minAlarmVolumeChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|MinimumAlarmVolume", index);
}

/**************************************************************************************************
 * 报警暂停时间。
 *************************************************************************************************/
void AlarmMaintainMenu::_alarmPauseTimeChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|AlarmPauseTime", index);
}

/**************************************************************************************************
 * 报警关闭提示机制。
 *************************************************************************************************/
void AlarmMaintainMenu::_alarmClosePromptTimeChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|AlarmOffPrompting", index);
}

/**************************************************************************************************
 * enable alarm audio off。
 *************************************************************************************************/
void AlarmMaintainMenu::_enableAlarmAudioOffChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|EnableAlarmAudioOff", index);
}

/**************************************************************************************************
 * enable alarm off。
 *************************************************************************************************/
void AlarmMaintainMenu::_enableAlarmOffChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|EnableAlarmOff", index);

    if (0 == index)
    {
        _alarmOffAtPowerOn->setDisabled(true);
    }
    else
    {
        _alarmOffAtPowerOn->setDisabled(false);
    }
}

/**************************************************************************************************
 * alarm off at power on。
 *************************************************************************************************/
void AlarmMaintainMenu::_alarmOffAtPowerOnChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|AlarmOffAtPowerOn", index);
}

/**************************************************************************************************
 * Non-alerts beeps in Non-AED modes。
 *************************************************************************************************/
// void AlarmMaintainMenu::_nonAlertsBeepsInNonAEDSlot(int index)
// {
//     systemConfig.setNumValue("Alarms|NonAlertsBeepsInNonAED", index);
// }
/**************************************************************************************************
 * PauseMaxAlarm15Min。
 *************************************************************************************************/
void AlarmMaintainMenu::_pauseMaxAlarm15MinSlot(int index)
{
    systemConfig.setNumValue("Alarms|PauseMaxAlarm15Min", index);
}

/**************************************************************************************************
 * AlarmVol。
 *************************************************************************************************/
// void AlarmMaintainMenu::_alarmVolSlot(int index)
// {
//     systemConfig.setNumValue("Alarms|AlarmVol", index);
// }

/**************************************************************************************************
 * ReminderTone。
 *************************************************************************************************/
void AlarmMaintainMenu::_reminderToneSlot(int index)
{
    systemConfig.setNumValue("Alarms|ReminderTone", index);
}

/**************************************************************************************************
 * ReminderToneIntervals。
 *************************************************************************************************/
void AlarmMaintainMenu::_reminderToneIntervalsSlot(int index)
{
    systemConfig.setNumValue("Alarms|ReminderToneIntervals", index);
}

/**************************************************************************************************
 * AlarmLightOnAlarmReset。
 *************************************************************************************************/
void AlarmMaintainMenu::_alarmLightOnAlarmResetSlot(int index)
{
    systemConfig.setNumValue("Alarms|AlarmLightOnAlarmReset", index);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmMaintainMenu::~AlarmMaintainMenu()
{
}
