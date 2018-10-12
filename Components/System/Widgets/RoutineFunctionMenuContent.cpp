/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#include "RoutineFunctionMenuContent.h"
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
#include "NightModeManager.h"
#include "Button.h"
#include "CalculateWindow.h"
#include "WindowManager.h"
#include "WiFiProfileWindow.h"
#include "SoftWareVersionWindow.h"
#include "DemoModeWindow.h"
#include "NightModeWindow.h"
#include "StandyWindow.h"

class RoutineFunctionMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_CALCULATE = 0,

        ITEM_CBO_ALARM_VOLUME,
        ITEM_CBO_SCREEN_BRIGHTNESS,
        ITEM_CBO_KEYPRESS_VOLUME,

        ITEM_BTN_WIFI_PROFILE,
        ITEM_BTN_MACHINE_VERSION,
        ITEM_BTN_ENTER_STANDY,
        ITEM_BTN_NIGHT_MODE,
        ITEM_BTN_DEMO_MODE
    };

    RoutineFunctionMenuContentPrivate()
                    : demoBtn(NULL)
    {}

    // load settings
    void loadOptions();

    Button *demoBtn;

    QMap<MenuItem, ComboBox *> combos;
};

void RoutineFunctionMenuContentPrivate::loadOptions()
{
    if (nightModeManager.isNightMode())
    {
        combos[ITEM_CBO_SCREEN_BRIGHTNESS]->setEnabled(false);
        combos[ITEM_CBO_KEYPRESS_VOLUME]->setEnabled(false);
        combos[ITEM_CBO_ALARM_VOLUME]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_SCREEN_BRIGHTNESS]->setEnabled(true);
        combos[ITEM_CBO_KEYPRESS_VOLUME]->setEnabled(true);
        combos[ITEM_CBO_ALARM_VOLUME]->setEnabled(true);

        int index = 0;

        index = soundManager.getVolume(SoundManager::SOUND_TYPE_ALARM) - 1;
        combos[ITEM_CBO_ALARM_VOLUME]->setCurrentIndex(index);

        index = systemManager.getBrightness() - 1;
        combos[ITEM_CBO_SCREEN_BRIGHTNESS]->setCurrentIndex(index);

        index = soundManager.getVolume(SoundManager::SOUND_TYPE_KEY_PRESS);
        combos[ITEM_CBO_KEYPRESS_VOLUME]->setCurrentIndex(index);
    }
}

RoutineFunctionMenuContent::RoutineFunctionMenuContent()
    : MenuContent(trs("RoutineFunctionMenu"),
                  trs("RoutineFunctionMenuDesc")),
      d_ptr(new RoutineFunctionMenuContentPrivate)
{
}

RoutineFunctionMenuContent::~RoutineFunctionMenuContent()
{
    delete d_ptr;
}

void RoutineFunctionMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void RoutineFunctionMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;
    int row = 0;

    Button *btn;

    // calculate
    btn = new Button(trs("Calculate"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(RoutineFunctionMenuContentPrivate::ITEM_BTN_CALCULATE);
    btn->setProperty("Item", qVariantFromValue(itemID));
    layout->addWidget(btn, row, 1);
    row++;

    // alarm volume
    label = new QLabel(trs("SystemAlarmVolume"));
    layout->addWidget(label, row, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << QString::number(SoundManager::VOLUME_LEV_1)
                       << QString::number(SoundManager::VOLUME_LEV_2)
                       << QString::number(SoundManager::VOLUME_LEV_3)
                       << QString::number(SoundManager::VOLUME_LEV_4)
                       << QString::number(SoundManager::VOLUME_LEV_5)
                      );
    itemID = static_cast<int>(RoutineFunctionMenuContentPrivate::ITEM_CBO_ALARM_VOLUME);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, row, 1);
    row++;
    d_ptr->combos.insert(RoutineFunctionMenuContentPrivate::ITEM_CBO_ALARM_VOLUME, comboBox);

    // screen brightness
    label = new QLabel(trs("SystemBrightness"));
    layout->addWidget(label, row, 0);
    comboBox = new ComboBox();
    for (int i = BRT_LEVEL_1; i < BRT_LEVEL_NR; i++)
    {
        comboBox->addItem(QString::number(i));
    }
    itemID = static_cast<int>(RoutineFunctionMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, row, 1);
    row++;
    d_ptr->combos.insert(RoutineFunctionMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS, comboBox);

    // key press volume
    label = new QLabel(trs("KeyPressVolume"));
    layout->addWidget(label , row , 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       <<QString::number(SoundManager::VOLUME_LEV_0)
                       <<QString::number(SoundManager::VOLUME_LEV_1)
                       <<QString::number(SoundManager::VOLUME_LEV_2)
                       <<QString::number(SoundManager::VOLUME_LEV_3)
                       <<QString::number(SoundManager::VOLUME_LEV_4)
                       <<QString::number(SoundManager::VOLUME_LEV_5)
                       );
    layout->addWidget(comboBox , row , 1);
    row++;
    itemID = static_cast<int>(RoutineFunctionMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME);
    comboBox->setProperty("Item" , qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)) , this , SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(RoutineFunctionMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME , comboBox);

    if (systemManager.isSupport(CONFIG_WIFI))
    {
        // wifi profile
        btn = new Button(trs("WiFiProfile"));
        btn->setButtonStyle(Button::ButtonTextOnly);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        itemID = static_cast<int>(RoutineFunctionMenuContentPrivate::ITEM_BTN_WIFI_PROFILE);
        btn->setProperty("Item", qVariantFromValue(itemID));
        layout->addWidget(btn, row, 1);
        row++;
    }

    // machine version
    btn = new Button(trs("MachineVersion"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(RoutineFunctionMenuContentPrivate::ITEM_BTN_MACHINE_VERSION);
    btn->setProperty("Item", qVariantFromValue(itemID));
    layout->addWidget(btn, row, 1);
    row++;

    // standy
    btn = new Button(trs("Standy"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(RoutineFunctionMenuContentPrivate::ITEM_BTN_ENTER_STANDY);
    btn->setProperty("Item", qVariantFromValue(itemID));
    layout->addWidget(btn, row, 1);
    row++;

    // night mode
    btn = new Button(trs("NightMode"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(RoutineFunctionMenuContentPrivate::ITEM_BTN_NIGHT_MODE);
    btn->setProperty("Item", qVariantFromValue(itemID));
    layout->addWidget(btn, row, 1);
    row++;

    // demo mode
    btn = new Button(trs("DemoMode"));
    d_ptr->demoBtn = btn;
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(RoutineFunctionMenuContentPrivate::ITEM_BTN_DEMO_MODE);
    btn->setProperty("Item", qVariantFromValue(itemID));
    layout->addWidget(btn, row, 1);
    row++;

    layout->setRowStretch(row, 1);
}

void RoutineFunctionMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        RoutineFunctionMenuContentPrivate::MenuItem item
                = (RoutineFunctionMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case RoutineFunctionMenuContentPrivate::ITEM_CBO_ALARM_VOLUME:
        {
            int volume = box->itemText(index).toInt();
            soundManager.setVolume(SoundManager::SOUND_TYPE_ALARM, (SoundManager::VolumeLevel) volume);
            break;
        }
        case RoutineFunctionMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS:
        {
            int brightness = box->itemText(index).toInt();
            systemManager.setBrightness((BrightnessLevel)brightness);
            break;
        }
        case RoutineFunctionMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME:
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

void RoutineFunctionMenuContent::onBtnReleasd()
{
    Button *btn = qobject_cast<Button*>(sender());
    int index = btn->property("Item").toInt();

    switch (index)
    {
        case RoutineFunctionMenuContentPrivate::ITEM_BTN_CALCULATE:
        {
            CalculateWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
        case RoutineFunctionMenuContentPrivate::ITEM_BTN_WIFI_PROFILE:
        {
            WiFiProfileWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
        case RoutineFunctionMenuContentPrivate::ITEM_BTN_MACHINE_VERSION:
        {
            SoftWareVersionWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
        case RoutineFunctionMenuContentPrivate::ITEM_BTN_ENTER_STANDY:
        {
            StandyWindow w;
            w.exec();
        }
        break;
        case RoutineFunctionMenuContentPrivate::ITEM_BTN_NIGHT_MODE:
        {
            NightModeWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
        case RoutineFunctionMenuContentPrivate::ITEM_BTN_DEMO_MODE:
        {
            if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
            {
                systemManager.setWorkMode(WORK_MODE_NORMAL);
                d_ptr->demoBtn->setText(trs("DemoMode"));
                windowManager.closeAllWidows();
                break;
            }

            DemoModeWindow w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
            if (!w.isUserInputCorrect())
            {
                break;
            }

            systemManager.setWorkMode(WORK_MODE_DEMO);
            d_ptr->demoBtn->setText(trs("ExitDemoMode"));

            windowManager.closeAllWidows();
        }
        break;
    }
}
