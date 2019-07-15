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
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"
#include "IMessageBox.h"
#include "Button.h"
#include "SoundManager.h"
#include "AlarmSymbol.h"
#include "Alarm.h"
#include "AlarmIndicator.h"
#include "AlarmSourceManager.h"

class AlarmMaintainMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_MIN_ALARM_VOLUME,
        ITEM_CBO_ALARAM_PAUSE_TIME,
        ITEM_CBO_ALARM_CLOSE_PROMPT_TIME,
        ITEM_CBO_ENABLE_ALARM_OFF,
        ITEM_CBO_POWERON_ALARM_OFF,
        ITEM_CBO_PAUSE_MAX_ALARM_15MIN,
        ITEM_CBO_REMINDER_TONE,
        ITEM_CBO_REMINDER_TONE_INTERVAL,
        ITEM_CBO_ALARM_LIGHT_RESET,
        ITEM_CBO_ALARM_LATCH_LOCK,
        ITEM_CBO_ALARM_AUDIO_OFF,
        ITEM_CBO_DEFAULT
    };

    AlarmMaintainMenuContentPrivate() : defaultBtn(NULL) {}

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
};

void AlarmMaintainMenuContentPrivate::loadOptions()
{
    int index;

    systemConfig.getNumValue("Alarms|MinimumAlarmVolume", index);
    combos[ITEM_CBO_MIN_ALARM_VOLUME]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|AlarmPauseTime", index);
    combos[ITEM_CBO_ALARAM_PAUSE_TIME]->setCurrentIndex(index);

#ifndef CLOSE_USELESS_ALARM_FUNCTION
    systemConfig.getNumValue("Alarms|AlarmOffPrompting", index);
    combos[ITEM_CBO_ALARM_CLOSE_PROMPT_TIME]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|EnableAlarmOff", index);
    combos[ITEM_CBO_ENABLE_ALARM_OFF]->setCurrentIndex(index);

    if (0 == index)
    {
        combos[ITEM_CBO_POWERON_ALARM_OFF]->setDisabled(true);
    }
    else
    {
        combos[ITEM_CBO_POWERON_ALARM_OFF]->setDisabled(false);
    }
    systemConfig.getNumValue("Alarms|AlarmOffAtPowerOn", index);
    combos[ITEM_CBO_POWERON_ALARM_OFF]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|PauseMaxAlarm15Min", index);
    combos[ITEM_CBO_PAUSE_MAX_ALARM_15MIN]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|ReminderTone", index);
    combos[ITEM_CBO_REMINDER_TONE]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|ReminderToneIntervals", index);
    combos[ITEM_CBO_REMINDER_TONE_INTERVAL]->setCurrentIndex(index);
#endif

    bool flag = alertor.getAlarmLightOnAlarmReset();
    index = flag ? 1 : 0;
    combos[ITEM_CBO_ALARM_LIGHT_RESET]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|PhyParAlarmLatchlockOn", index);
    combos[ITEM_CBO_ALARM_LATCH_LOCK]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|AlarmAudio", index);
    combos[ITEM_CBO_ALARM_AUDIO_OFF]->setCurrentIndex(index);

#ifdef DISABLED_ALARM_LATCH
    combos[ITEM_CBO_ALARM_LATCH_LOCK]->setEnabled(false);
#endif
    combos[ITEM_CBO_MIN_ALARM_VOLUME]->setEnabled(false);
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
    case ITEM_CBO_ALARM_CLOSE_PROMPT_TIME:
        systemConfig.setNumValue("Alarms|AlarmOffPrompting", index);
        break;
    case ITEM_CBO_ENABLE_ALARM_OFF:
        systemConfig.setNumValue("Alarms|EnableAlarmOff", index);
#ifndef CLOSE_USELESS_ALARM_FUNCTION
        if (0 == index)
        {
            combos[ITEM_CBO_POWERON_ALARM_OFF]->setDisabled(true);
        }
        else
        {
            combos[ITEM_CBO_POWERON_ALARM_OFF]->setDisabled(false);
        }
#endif
        break;
    case ITEM_CBO_POWERON_ALARM_OFF:
        systemConfig.setNumValue("Alarms|AlarmOffAtPowerOn", index);
        break;
    case ITEM_CBO_PAUSE_MAX_ALARM_15MIN:
        systemConfig.setNumValue("Alarms|PauseMaxAlarm15Min", index);
        break;
    case ITEM_CBO_REMINDER_TONE:
        systemConfig.setNumValue("Alarms|ReminderTone", index);
        break;
    case ITEM_CBO_REMINDER_TONE_INTERVAL:
        systemConfig.setNumValue("Alarms|ReminderToneIntervals", index);
        break;
    case ITEM_CBO_ALARM_LIGHT_RESET:
        alertor.setAlarmLightOnAlarmReset(index);
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
            systemConfig.setNumValue("Alarms|AlarmAudio", index);
            alarmIndicator.updateAlarmAudioState();
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
    defalutConfig.getNumValue("Alarms|MinimumAlarmVolume", index);
    defaultIndexMap[ITEM_CBO_MIN_ALARM_VOLUME] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|AlarmPauseTime", index);
    defaultIndexMap[ITEM_CBO_ALARAM_PAUSE_TIME] = index;

#ifndef CLOSE_USELESS_ALARM_FUNCTION
    index = 0;
    defalutConfig.getNumValue("Alarms|AlarmOffPrompting", index);
    defaultIndexMap[ITEM_CBO_ALARM_CLOSE_PROMPT_TIME] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|EnableAlarmOff", index);
    defaultIndexMap[ITEM_CBO_ENABLE_ALARM_OFF] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|AlarmOffAtPowerOn", index);
    defaultIndexMap[ITEM_CBO_POWERON_ALARM_OFF] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|PauseMaxAlarm15Min", index);
    defaultIndexMap[ITEM_CBO_PAUSE_MAX_ALARM_15MIN] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|ReminderTone", index);
    defaultIndexMap[ITEM_CBO_REMINDER_TONE] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|ReminderToneIntervals", index);
    defaultIndexMap[ITEM_CBO_REMINDER_TONE_INTERVAL] = index;
#endif

    index = 0;
    defalutConfig.getNumValue("Alarms|AlarmLightOnAlarmReset", index);
    defaultIndexMap[ITEM_CBO_ALARM_LIGHT_RESET] = index;

    index = 0;
    defalutConfig.getNumValue("Alarms|AlarmAudio", index);
    defaultIndexMap[ITEM_CBO_ALARM_AUDIO_OFF] = index;

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

#ifndef CLOSE_USELESS_ALARM_FUNCTION
    // 报警关闭,报警音关闭提示时间
    label = new QLabel(trs("AlarmOffPromptMechanism"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(AlarmSymbol::convert(ALARM_CLOSE_PROMPT_OFF))
                       << trs(AlarmSymbol::convert(ALARM_CLOSE_PROMPT_5MIN))
                       << trs(AlarmSymbol::convert(ALARM_CLOSE_PROMPT_10MIN))
                       << trs(AlarmSymbol::convert(ALARM_CLOSE_PROMPT_15MIN)));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_CLOSE_PROMPT_TIME);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_CLOSE_PROMPT_TIME, comboBox);

    // enable alarm off
    label = new QLabel(trs("EnableAlarmOff"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ENABLE_ALARM_OFF);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ENABLE_ALARM_OFF, comboBox);

    // alarm off at power on
    label = new QLabel(trs("AlarmOffAtPowerOn"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_POWERON_ALARM_OFF);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_POWERON_ALARM_OFF, comboBox);

    // pause max alarm 15min
    label = new QLabel(trs("IsPauseMaxAlarm15Min"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_PAUSE_MAX_ALARM_15MIN);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_PAUSE_MAX_ALARM_15MIN, comboBox);

    // reminder tone
    label = new QLabel(trs("ReminderTone"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Close")
                       << trs("Open")
                       << trs("OpenAgain"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_REMINDER_TONE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_REMINDER_TONE, comboBox);

    // reminder tone interval
    label = new QLabel(trs("ReminderToneIntervals"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("H1min")
                       << trs("H2min")
                       << trs("H3min"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_REMINDER_TONE_INTERVAL);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_REMINDER_TONE_INTERVAL, comboBox);
#endif

    // alarmLight On Alarm Reset
//    label = new QLabel(trs("AlarmLightOnAlarmReset"));
//    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Close")
                       << trs("Open"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LIGHT_RESET);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
//    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LIGHT_RESET, comboBox);

    // latch lock
//    label = new QLabel(trs("LatchLockSwitch"));
//    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On"));
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LATCH_LOCK);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
//    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LATCH_LOCK, comboBox);

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

    // default
    label = new QLabel(trs("Defaults"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->defaultBtn = new Button(trs("RecoverDefaults"));
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
