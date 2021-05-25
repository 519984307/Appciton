/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/16
 **/

#include "AlarmMaintainMenuContent.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include "IConfig.h"
#include "SoundManager.h"
#include "AlarmSymbol.h"
#include "Alarm.h"
#include "AlarmIndicator.h"
#include "AlarmSourceManager.h"
#include "SystemDefine.h"

class AlarmMaintainMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_MIN_ALARM_VOLUME,
        ITEM_CBO_ALARAM_PAUSE_TIME,
        ITEM_CBO_ALARM_LIGHT_RESET,
        ITEM_CBO_ALARM_LATCH_LOCK,
        ITEM_CBO_ALARM_AUDIO_OFF,
        ITEM_CBO_DEFAULT
    };

    AlarmMaintainMenuContentPrivate() : defaultBtn(NULL), minAlarmVol(2) {}

    void loadOptions();

    /**
     * @brief HandlingComboIndexChanged  下拉框item改变时处理接口
     * @param item 下拉框item
     * @param index 下拉框的索引
     */
    void HandlingComboIndexChanged(MenuItem item, int index);

    /**
     * @brief defaultIndexInit  初始化下拉框的默认索引
     */
    void defaultIndexInit();

    QMap<MenuItem, ComboBox *> combos;
    Button *defaultBtn;
    QMap<MenuItem, int> defaultIndexMap;
    int minAlarmVol;
};

void AlarmMaintainMenuContentPrivate::loadOptions()
{
    int index;

    systemConfig.getNumValue("Alarms|AlarmPauseTime", index);
    combos[ITEM_CBO_ALARAM_PAUSE_TIME]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|MinimumAlarmVolume", index);
    combos[ITEM_CBO_MIN_ALARM_VOLUME]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("Alarms|AlarmLightOnAlarmReset", index);
    combos[ITEM_CBO_ALARM_LIGHT_RESET]->setCurrentIndex(index);
    // 报警复位时报警灯，该菜单项已无作用，禁用使能
    combos[ITEM_CBO_ALARM_LIGHT_RESET]->setEnabled(false);

    systemConfig.getNumValue("Alarms|PhyParAlarmLatchlockOn", index);
    combos[ITEM_CBO_ALARM_LATCH_LOCK]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|AlarmAudio", index);
    combos[ITEM_CBO_ALARM_AUDIO_OFF]->setCurrentIndex(index);

#ifndef DISABLED_ALARM_LATCH
    combos[ITEM_CBO_ALARM_LATCH_LOCK]->setEnabled(false);
#endif
}

void AlarmMaintainMenuContentPrivate::
        HandlingComboIndexChanged(AlarmMaintainMenuContentPrivate::MenuItem item, int index)
{
    switch (item)
    {
    case ITEM_CBO_MIN_ALARM_VOLUME:
    {
        systemConfig.setNumValue("Alarms|MinimumAlarmVolume", index);

        int volume = 0;
        systemConfig.getNumValue("Alarms|DefaultAlarmVolume", volume);
        if (volume < index)
        {
            volume = index;
            systemConfig.setNumValue("Alarms|DefaultAlarmVolume", volume);
        }
        soundManager.setVolume(SoundManager::SOUND_TYPE_ALARM, static_cast<SoundManager::VolumeLevel>(volume));
    }
    break;
    case ITEM_CBO_ALARAM_PAUSE_TIME:
        systemConfig.setNumValue("Alarms|AlarmPauseTime", index);
        break;
    case ITEM_CBO_ALARM_LIGHT_RESET:
        break;
    case ITEM_CBO_ALARM_LATCH_LOCK:
        systemConfig.setNumValue("Alarms|PhyParAlarmLatchlockOn", index);
        if (index == 0)
        {
            alertor.setLatchLockSta(false);
        }
        else
        {
            alertor.setLatchLockSta(true);
        }
        break;
    case ITEM_CBO_ALARM_AUDIO_OFF:
    {
            alarmIndicator.setAlarmAudioState(index);
            AlarmOneShotIFace *systemAlarm = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SYSTEM);
            if (systemAlarm)
            {
                systemAlarm->setOneShotAlarm(SYSTEM_ONE_SHOT_ALARM_AUDIO_OFF, !index);
            }
        break;
    }
    default:
        break;
    }
}

void AlarmMaintainMenuContentPrivate::defaultIndexInit()
{
    int index;
    Config defalutConfig(ORGINAL_SYSTEM_CFG_FILE);

    index = 0;
    defalutConfig.getNumValue("Alarms|AlarmPauseTime", index);
    defaultIndexMap[ITEM_CBO_ALARAM_PAUSE_TIME] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|MinimumAlarmVolume", index);
    defaultIndexMap[ITEM_CBO_MIN_ALARM_VOLUME] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|AlarmAudio", index);
    defaultIndexMap[ITEM_CBO_ALARM_AUDIO_OFF] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|AlarmLightOnAlarmReset", index);
    defaultIndexMap[ITEM_CBO_ALARM_LIGHT_RESET] = index;

#ifndef DISABLED_ALARM_LATCH
    index = 0;
    defalutConfig.getNumValue("Alarms|PhyParAlarmLatchlockOn", index);
    defaultIndexMap[ITEM_CBO_ALARM_LATCH_LOCK] = index;
#endif
}

AlarmMaintainMenuContent::AlarmMaintainMenuContent()
    : MenuContent(trs("AlarmMaintainMenu"), trs("AlarmMaintainMenuDesc")),
      d_ptr(new AlarmMaintainMenuContentPrivate)
{
    d_ptr->defaultIndexInit();
}

AlarmMaintainMenuContent::~AlarmMaintainMenuContent()
{
    delete d_ptr;
}

void AlarmMaintainMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void AlarmMaintainMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // 最小报警音量
    label = new QLabel(trs("MinimumALarmVolume"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << QString::number(SoundManager::VOLUME_LEV_0)
                       << QString::number(SoundManager::VOLUME_LEV_1)
                       << QString::number(SoundManager::VOLUME_LEV_2)
                       << QString::number(SoundManager::VOLUME_LEV_3)
                       << QString::number(SoundManager::VOLUME_LEV_4)
                       << QString::number(SoundManager::VOLUME_LEV_5));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_MIN_ALARM_VOLUME);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_MIN_ALARM_VOLUME, comboBox);
    // 设置声音触发方式
    connect(comboBox, SIGNAL(itemFocusChanged(int)), this, SLOT(onPopupListItemFocusChanged(int)));

    // Alarm Audio Off
    label = new QLabel(trs("AlarmAudio"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                           << trs("Off")
                           << trs("On"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_AUDIO_OFF);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_AUDIO_OFF, comboBox);

    // alarmLight On Alarm Reset
    label = new QLabel(trs("AlarmLightOnAlarmReset"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Close")
                       << trs("Open"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LIGHT_RESET);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LIGHT_RESET, comboBox);

    // latch lock
    label = new QLabel(trs("LatchLockSwitch"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LATCH_LOCK);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LATCH_LOCK, comboBox);

    // 报警暂停时间
    label = new QLabel(trs("AlarmPauseTime"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_1MIN))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_2MIN))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_3MIN))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_5MIN))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_10MIN))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_15MIN)));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARAM_PAUSE_TIME);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARAM_PAUSE_TIME, comboBox);

    // default
    label = new QLabel(trs("Defaults"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->defaultBtn = new Button(trs("RestoreDefaults"));
    d_ptr->defaultBtn->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_DEFAULT);
    d_ptr->defaultBtn->setProperty("Item", qVariantFromValue(itemID));
    connect(d_ptr->defaultBtn, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(d_ptr->defaultBtn, d_ptr->combos.count(), 1);

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}

void AlarmMaintainMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box == NULL)
    {
        return;
    }
    AlarmMaintainMenuContentPrivate::MenuItem item
        = (AlarmMaintainMenuContentPrivate::MenuItem)box->property("Item").toInt();
    d_ptr->HandlingComboIndexChanged(item, index);
}

void AlarmMaintainMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        AlarmMaintainMenuContentPrivate::MenuItem item
                = (AlarmMaintainMenuContentPrivate::MenuItem)button->property("Item").toInt();
        switch (item) {
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_DEFAULT:
        {
            for (int i = 0; i < item; i++)
            {
                AlarmMaintainMenuContentPrivate::MenuItem defaultItem  = AlarmMaintainMenuContentPrivate::MenuItem(i);
                d_ptr->HandlingComboIndexChanged(defaultItem, d_ptr->defaultIndexMap[defaultItem]);
            }
            d_ptr->loadOptions();
        }
            break;
        default:
            break;
        }
    }
}

void AlarmMaintainMenuContent::onPopupListItemFocusChanged(int volume)
{
    ComboBox *w = qobject_cast<ComboBox*>(sender());
    if (w == d_ptr->combos[AlarmMaintainMenuContentPrivate::ITEM_CBO_MIN_ALARM_VOLUME])
    {
        soundManager.setVolume(SoundManager::SOUND_TYPE_ALARM , static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.alarmTone();
    }
}
