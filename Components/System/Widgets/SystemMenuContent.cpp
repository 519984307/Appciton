/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#include "SystemMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include <QList>
#include "SoundManager.h"
#include "SystemManager.h"
#include "IConfig.h"
#include "SystemDefine.h"
#include "PrintDefine.h"
#include "RecorderManager.h"

class SystemMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ALARM_VOLUME,
        ITEM_CBO_SCREEN_BRIGHTNESS,
        ITEM_CBO_PRINT_SPEED,
        ITEM_CBO_PRINT_WAVEFORM_NUM,
        ITEM_CBO_KEYPRESS_VOLUME_NUM
    };

    SystemMenuContentPrivate() {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
};

void SystemMenuContentPrivate::loadOptions()
{
    int index = 0;
    index = soundManager.getVolume(SoundManager::SOUND_TYPE_ALARM) - 1;
    combos[ITEM_CBO_ALARM_VOLUME]->setCurrentIndex(index);

    index = systemManager.getBrightness() - 1;
    combos[ITEM_CBO_SCREEN_BRIGHTNESS]->setCurrentIndex(index);

    index = recorderManager.getPrintSpeed();
    combos[ITEM_CBO_PRINT_SPEED]->setCurrentIndex(index);

    index = recorderManager.getPrintWaveNum() - 1;
    combos[ITEM_CBO_PRINT_WAVEFORM_NUM]->setCurrentIndex(index);
}

SystemMenuContent::SystemMenuContent()
    : MenuContent(trs("SystemMenu"), trs("SystemMenuDesc")),
      d_ptr(new SystemMenuContentPrivate)
{
}

SystemMenuContent::~SystemMenuContent()
{
    delete d_ptr;
}

void SystemMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void SystemMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // alarm volume
    label = new QLabel(trs("SystemAlarmVolume"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << QString::number(SoundManager::VOLUME_LEV_1)
                       << QString::number(SoundManager::VOLUME_LEV_2)
                       << QString::number(SoundManager::VOLUME_LEV_3)
                       << QString::number(SoundManager::VOLUME_LEV_4)
                       << QString::number(SoundManager::VOLUME_LEV_5)
                      );
    itemID = static_cast<int>(SystemMenuContentPrivate::ITEM_CBO_ALARM_VOLUME);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SystemMenuContentPrivate::ITEM_CBO_ALARM_VOLUME, comboBox);

    // screen brightness
    label = new QLabel(trs("SystemBrightness"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << QString::number(BRT_LEVEL_1)
                       << QString::number(BRT_LEVEL_2)
                       << QString::number(BRT_LEVEL_3)
                       << QString::number(BRT_LEVEL_4)
                       << QString::number(BRT_LEVEL_5)
                       << QString::number(BRT_LEVEL_6)
                       << QString::number(BRT_LEVEL_7)
                       << QString::number(BRT_LEVEL_8)
                       << QString::number(BRT_LEVEL_9)
                      );
    itemID = static_cast<int>(SystemMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SystemMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS, comboBox);

    // print speed
    label = new QLabel(trs("PrintSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << PrintSymbol::convert(PRINT_SPEED_125)
                       << PrintSymbol::convert(PRINT_SPEED_250)
                       << PrintSymbol::convert(PRINT_SPEED_500)
                      );
    itemID = static_cast<int>(SystemMenuContentPrivate::ITEM_CBO_PRINT_SPEED);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SystemMenuContentPrivate::ITEM_CBO_PRINT_SPEED, comboBox);

    // print waveform num
    label = new QLabel(trs("PrintNumOfTraces"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << QString::number(PRINT_WAVEFORM_NUM_1)
                       << QString::number(PRINT_WAVEFORM_NUM_2)
                       << QString::number(PRINT_WAVEFORM_NUM_3)
                       << QString::number(PRINT_WAVEFORM_NUM_4)
                      );
    itemID = static_cast<int>(SystemMenuContentPrivate::ITEM_CBO_PRINT_WAVEFORM_NUM);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SystemMenuContentPrivate::ITEM_CBO_PRINT_WAVEFORM_NUM, comboBox);

    // key press volume
    label = new QLabel(trs("KeyPressVolume"));
    layout->addWidget(label , d_ptr->combos.count() , 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       <<QString::number(SoundManager::VOLUME_LEV_0)
                       <<QString::number(SoundManager::VOLUME_LEV_1)
                       <<QString::number(SoundManager::VOLUME_LEV_2)
                       <<QString::number(SoundManager::VOLUME_LEV_3)
                       <<QString::number(SoundManager::VOLUME_LEV_4)
                       <<QString::number(SoundManager::VOLUME_LEV_5)
                       );
    layout->addWidget(comboBox , d_ptr->combos.count() , 1);
    itemID = static_cast<int>(SystemMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME_NUM);
    comboBox->setProperty("Item" , qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)) , this , SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(SystemMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME_NUM , comboBox);

    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void SystemMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        SystemMenuContentPrivate::MenuItem item
                = (SystemMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case SystemMenuContentPrivate::ITEM_CBO_ALARM_VOLUME:
        {
            int volume = box->itemText(index).toInt();
            soundManager.setVolume(SoundManager::SOUND_TYPE_ALARM, (SoundManager::VolumeLevel) volume);
            break;
        }
        case SystemMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS:
        {
            int brightness = box->itemText(index).toInt();
            systemManager.setBrightness((BrightnessLevel)brightness);
            break;
        }
        case SystemMenuContentPrivate::ITEM_CBO_PRINT_SPEED:
        {
            recorderManager.setPrintSpeed((PrintSpeed)index);
            break;
        }
        case SystemMenuContentPrivate::ITEM_CBO_PRINT_WAVEFORM_NUM:
        {
            int waveformNum = box->itemText(index).toInt();
            recorderManager.setPrintWaveNum(waveformNum);
            break;
        }
        case SystemMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME_NUM:
        {
            int volume = box->itemText(index).toInt();
            soundManager.setVolume(SoundManager::SOUND_TYPE_KEY_PRESS , (SoundManager::VolumeLevel)volume);
            break;
        }
        default:
            break;
        }
    }
}
