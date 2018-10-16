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
#include "NumberInput.h"
#include "IMessageBox.h"
#include "Button.h"
#include "SoundManager.h"
#include "AlarmSymbol.h"
#include "Alarm.h"

class AlarmMaintainMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_MIN_ALARM_VOLUME,
        ITEM_CBO_ALARAM_PAUSE_TIME,
        ITEM_CBO_ALARM_CLOSE_PROMPT_TIME,
        ITEM_CBO_ENABLE_ALARM_AUDIO_OFF,
        ITEM_CBO_ENABLE_ALARM_OFF,
        ITEM_CBO_POWERON_ALARM_OFF,
        ITEM_CBO_PAUSE_MAX_ALARM_15MIN,
        ITEM_CBO_REMINDER_TONE,
        ITEM_CBO_REMINDER_TONE_INTERVAL,
        ITEM_CBO_ALARM_LIGHT_RESET,
        ITEM_CBO_ALARM_BOLT_LOCK,
        ITEM_CBO_DEFAULT
    };

    AlarmMaintainMenuContentPrivate() : defaultBtn(NULL) {}

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    Button *defaultBtn;
};

void AlarmMaintainMenuContentPrivate::loadOptions()
{
    int index;

    systemConfig.getNumValue("Alarms|MinimumAlarmVolume", index);
    combos[ITEM_CBO_MIN_ALARM_VOLUME]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|AlarmPauseTime", index);
    combos[ITEM_CBO_ALARAM_PAUSE_TIME]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|AlarmOffPrompting", index);
    combos[ITEM_CBO_ALARM_CLOSE_PROMPT_TIME]->setCurrentIndex(index);

    systemConfig.getNumValue("Alarms|EnableAlarmAudioOff", index);
    combos[ITEM_CBO_ENABLE_ALARM_AUDIO_OFF]->setCurrentIndex(index);

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

    systemConfig.getNumValue("Alarms|AlarmLightOnAlarmReset", index);
    combos[ITEM_CBO_ALARM_LIGHT_RESET]->setCurrentIndex(index);
}

AlarmMaintainMenuContent::AlarmMaintainMenuContent()
    : MenuContent(trs("AlarmMaintainMenu"), trs("AlarmMaintainMenuDesc")),
      d_ptr(new AlarmMaintainMenuContentPrivate)
{
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
                       << QString::number(SoundManager::VOLUME_LEV_1)
                       << QString::number(SoundManager::VOLUME_LEV_2)
                       << QString::number(SoundManager::VOLUME_LEV_3)
                       << QString::number(SoundManager::VOLUME_LEV_4)
                       << QString::number(SoundManager::VOLUME_LEV_5)
                      );
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_MIN_ALARM_VOLUME);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_MIN_ALARM_VOLUME, comboBox);

    // 报警暂停时间
    label = new QLabel(trs("AlarmPauseTime"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_60S))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_90S))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_120S))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_150S))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_180S))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_210S))
                       << trs(AlarmSymbol::convert(ALARM_PAUSE_TIME_240S))
                      );
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARAM_PAUSE_TIME);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARAM_PAUSE_TIME, comboBox);

    // 报警关闭,报警音关闭提示时间
    label = new QLabel(trs("AlarmOffPromptMechanism"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(AlarmSymbol::convert(ALARM_CLOSE_PROMPT_OFF))
                       << trs(AlarmSymbol::convert(ALARM_CLOSE_PROMPT_5MIN))
                       << trs(AlarmSymbol::convert(ALARM_CLOSE_PROMPT_10MIN))
                       << trs(AlarmSymbol::convert(ALARM_CLOSE_PROMPT_15MIN))
                      );
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_CLOSE_PROMPT_TIME);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_CLOSE_PROMPT_TIME, comboBox);

    // enable alarm audio off
    label = new QLabel(trs("EnableAlarmAudioOff"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes")
                      );
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ENABLE_ALARM_AUDIO_OFF);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ENABLE_ALARM_AUDIO_OFF, comboBox);

    // enable alarm off
    label = new QLabel(trs("EnableAlarmOff"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes")
                      );
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
                       << trs("Yes")
                      );
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
                       << trs("Yes")
                      );
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
                       << trs("OpenAgain")
                      );
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
                       << trs("H3min")
                      );
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_REMINDER_TONE_INTERVAL);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_REMINDER_TONE_INTERVAL, comboBox);

    // alarmLight On Alarm Reset
    label = new QLabel(trs("AlarmLightOnAlarmReset"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Close")
                       << trs("Open")
                      );
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LIGHT_RESET);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LIGHT_RESET, comboBox);

    // bolt lock
    label = new QLabel(trs("LatchLockSwitch"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On")
                      );
    itemID = static_cast<int>(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_BOLT_LOCK);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_BOLT_LOCK, comboBox);

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
    if (box)
    {
        AlarmMaintainMenuContentPrivate::MenuItem item
                = (AlarmMaintainMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item) {
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_MIN_ALARM_VOLUME:
            systemConfig.setNumValue("Alarms|MinimumAlarmVolume", index);
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARAM_PAUSE_TIME:
            systemConfig.setNumValue("Alarms|AlarmPauseTime", index);
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_CLOSE_PROMPT_TIME:
            systemConfig.setNumValue("Alarms|AlarmOffPrompting", index);
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_ENABLE_ALARM_AUDIO_OFF:
            systemConfig.setNumValue("Alarms|EnableAlarmAudioOff", index);
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_ENABLE_ALARM_OFF:
            systemConfig.setNumValue("Alarms|EnableAlarmOff", index);
            if (0 == index)
            {
                d_ptr->combos[AlarmMaintainMenuContentPrivate::ITEM_CBO_POWERON_ALARM_OFF]->setDisabled(true);
            }
            else
            {
                d_ptr->combos[AlarmMaintainMenuContentPrivate::ITEM_CBO_POWERON_ALARM_OFF]->setDisabled(false);
            }
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_POWERON_ALARM_OFF:
            systemConfig.setNumValue("Alarms|AlarmOffAtPowerOn", index);
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_PAUSE_MAX_ALARM_15MIN:
            systemConfig.setNumValue("Alarms|PauseMaxAlarm15Min", index);
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_REMINDER_TONE:
            systemConfig.setNumValue("Alarms|ReminderTone", index);
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_REMINDER_TONE_INTERVAL:
            systemConfig.setNumValue("Alarms|ReminderToneIntervals", index);
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_LIGHT_RESET:
            systemConfig.setNumValue("Alarms|AlarmLightOnAlarmReset", index);
            break;
        case AlarmMaintainMenuContentPrivate::ITEM_CBO_ALARM_BOLT_LOCK:
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
            break;
        }
        default:
            break;
        }
    }
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

            break;
        default:
            break;
        }
    }
}

