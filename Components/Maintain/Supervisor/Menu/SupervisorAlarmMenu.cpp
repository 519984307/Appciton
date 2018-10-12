/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/12
 **/



#include <QVBoxLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "SupervisorAlarmMenu.h"
#include "SupervisorConfigManager.h"
#include "SupervisorMenuManager.h"
#include "AlarmSymbol.h"
#include "SoundManager.h"
#include "AlarmStateDefine.h"

SupervisorAlarmMenu *SupervisorAlarmMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorAlarmMenu::SupervisorAlarmMenu() : SubMenu(trs("AlarmSettingMenu"))
{
    setDesc(trs("AlarmSettingMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SupervisorAlarmMenu::readyShow()
{
    int index = 0;
//    systemConfig.getNumValue("Alarms|LTAAlarm", index);
//    _ltaAlarm->setCurrentIndex(index);

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
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SupervisorAlarmMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    //生命报警开关
//    _ltaAlarm = new IComboList(trs("LTAAlarm"));
//    _ltaAlarm->setFont(fontManager.textFont(fontsize));
//    _ltaAlarm->label->setFixedSize(labelWidth, ITEM_H);
//    _ltaAlarm->combolist->setFixedSize(btnWidth, ITEM_H);
//    _ltaAlarm->addItem(trs("Off"));
//    _ltaAlarm->addItem(trs("On"));
//    connect(_ltaAlarm, SIGNAL(currentIndexChanged(int)), this,
//            SLOT(_ltaAlarmChangeSlot(int)));
//    mainLayout->addWidget(_ltaAlarm);

    //最小报警音量
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

    //报警暂停时间
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

    //报警关闭，报警音关闭提示时间
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
}

/**************************************************************************************************
 * 最低报警音量。
 *************************************************************************************************/
void SupervisorAlarmMenu::_minAlarmVolumeChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|MinimumAlarmVolume", index);
}

/**************************************************************************************************
 * 报警暂停时间。
 *************************************************************************************************/
void SupervisorAlarmMenu::_alarmPauseTimeChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|AlarmPauseTime", index);
}

/**************************************************************************************************
 * 报警关闭提示机制。
 *************************************************************************************************/
void SupervisorAlarmMenu::_alarmClosePromptTimeChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|AlarmOffPrompting", index);
}

/**************************************************************************************************
 * enable alarm audio off。
 *************************************************************************************************/
void SupervisorAlarmMenu::_enableAlarmAudioOffChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|EnableAlarmAudioOff", index);
}

/**************************************************************************************************
 * enable alarm off。
 *************************************************************************************************/
void SupervisorAlarmMenu::_enableAlarmOffChangeSlot(int index)
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
void SupervisorAlarmMenu::_alarmOffAtPowerOnChangeSlot(int index)
{
    systemConfig.setNumValue("Alarms|AlarmOffAtPowerOn", index);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorAlarmMenu::~SupervisorAlarmMenu()
{
}


