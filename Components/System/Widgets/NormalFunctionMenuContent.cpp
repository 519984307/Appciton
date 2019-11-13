/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#include "NormalFunctionMenuContent.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include "SoundManager.h"
#include "SystemManager.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "SystemDefine.h"
#include "PrintDefine.h"
#include "RecorderManager.h"
#include "NightModeManager.h"
#include "CalculateWindow.h"
#include "WindowManager.h"
#include "WiFiProfileWindow.h"
#include "SoftwareVersionWindow.h"
#include "NightModeWindow.h"
#include "PasswordWindow.h"
#include "AlarmIndicator.h"
#include "SoftKeyManager.h"
#ifdef Q_WS_QWS
#include <QWSServer>
#endif

class NormalFunctionMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_CALCULATE = 0,

        ITEM_CBO_WAVE_LEN,
        ITEM_CBO_SCREEN_BRIGHTNESS,
        ITEM_CBO_KEYPRESS_VOLUME,
#ifdef Q_WS_QWS
#ifndef VITAVUE_15_INCHES
        ITEM_CBO_TOUCH_SCREEN,
#endif
#endif
        ITEM_BTN_WIFI_PROFILE,
        ITEM_BTN_SOFTWARE_VERSION,
        ITEM_BTN_ENTER_STANDY,
        ITEM_BTN_NIGHT_MODE,
        ITEM_BTN_DEMO_MODE
    };

    NormalFunctionMenuContentPrivate()
                    : demoBtn(NULL), standbyBtn(NULL), touchLab(NULL)
    {}

    // load settings
    void loadOptions();

    Button *demoBtn;
    Button *standbyBtn;

    QMap<MenuItem, ComboBox *> combos;
    QLabel *touchLab;
};

void NormalFunctionMenuContentPrivate::loadOptions()
{
    int waveLength;
    currentConfig.getNumValue("Event|WaveLength", waveLength);
    if (waveLength == 8)
    {
        combos[ITEM_CBO_WAVE_LEN]->setCurrentIndex(0);
    }
    else if (waveLength == 16)
    {
        combos[ITEM_CBO_WAVE_LEN]->setCurrentIndex(1);
    }
    else if (waveLength == 32)
    {
        combos[ITEM_CBO_WAVE_LEN]->setCurrentIndex(2);
    }

    if (nightModeManager.nightMode())
    {
        combos[ITEM_CBO_SCREEN_BRIGHTNESS]->setEnabled(false);
        combos[ITEM_CBO_KEYPRESS_VOLUME]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_SCREEN_BRIGHTNESS]->setEnabled(true);
        combos[ITEM_CBO_KEYPRESS_VOLUME]->setEnabled(true);
        int index = 0;
        index = systemManager.getBrightness();
        combos[ITEM_CBO_SCREEN_BRIGHTNESS]->setCurrentIndex(index);

        systemConfig.getNumValue("General|KeyPressVolume", index);
        combos[ITEM_CBO_KEYPRESS_VOLUME]->setCurrentIndex(index);
    }

    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
         standbyBtn->setEnabled(false);
    }
    else
    {
        standbyBtn->setEnabled(true);
    }

#ifdef Q_WS_QWS
#ifndef VITAVUE_15_INCHES
    // 加载数据时，强制锁住该信号。该信号会触发openMouse()函数，在调试期间，openMouse函数会有堵塞现象.
    combos[ITEM_CBO_TOUCH_SCREEN]->blockSignals(true);
    if (!systemManager.isSupport(CONFIG_TOUCH))
    {
        touchLab->setVisible(false);
        combos[ITEM_CBO_TOUCH_SCREEN]->setVisible(false);
    }
    combos[ITEM_CBO_TOUCH_SCREEN]->setCurrentIndex(systemManager.isTouchScreenOn());
    combos[ITEM_CBO_TOUCH_SCREEN]->blockSignals(false);
#endif
#endif
}

NormalFunctionMenuContent::NormalFunctionMenuContent()
    : MenuContent(trs("NormalFunctionMenu"),
                  trs("NormalFunctionMenuDesc")),
      d_ptr(new NormalFunctionMenuContentPrivate)
{
    connect(&nightModeManager, SIGNAL(nightModeChanged(bool)), this, SLOT(nightModeHandle(bool)));
}

NormalFunctionMenuContent::~NormalFunctionMenuContent()
{
    delete d_ptr;
}

void NormalFunctionMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void NormalFunctionMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;
    int row = 0;

    Button *btn;

#ifndef HIDE_CALCULATE_FUNCITON
    // calculate
    btn = new Button(trs("Calculate"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_BTN_CALCULATE);
    btn->setProperty("Item", qVariantFromValue(itemID));
    layout->addWidget(btn, row, 1);
    row++;
#endif

    // 波形长度
    label = new QLabel(trs("WaveLength"));
    layout->addWidget(label, row, 0);
    comboBox = new ComboBox();
    comboBox->addItem(trs("_8sec"));
    comboBox->addItem(trs("_16sec"));
    comboBox->addItem(trs("_32sec"));
    itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_CBO_WAVE_LEN);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, row, 1);
    row++;
    d_ptr->combos.insert(NormalFunctionMenuContentPrivate::ITEM_CBO_WAVE_LEN, comboBox);

    // screen brightness
    label = new QLabel(trs("SystemBrightness"));
    layout->addWidget(label, row, 0);
    comboBox = new ComboBox();
    for (int i = BRT_LEVEL_0; i < BRT_LEVEL_NR; i++)
    {
        comboBox->addItem(QString::number(i + 1));
    }
    itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    comboBox->setObjectName("SystemBrightness");
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, row, 1);
    row++;
    d_ptr->combos.insert(NormalFunctionMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS, comboBox);

    // key press volume
    label = new QLabel(trs("ToneVolume"));
    layout->addWidget(label , row , 0);
    comboBox = new ComboBox();

    // 设置声音触发方式
    connect(comboBox, SIGNAL(itemFoucsIndexChanged(int)),
            this, SLOT(onPopupListItemFocusChanged(int)));

    comboBox->addItems(QStringList()
                       <<trs("Off")
                       <<QString::number(SoundManager::VOLUME_LEV_1)
                       <<QString::number(SoundManager::VOLUME_LEV_2)
                       <<QString::number(SoundManager::VOLUME_LEV_3)
                       <<QString::number(SoundManager::VOLUME_LEV_4)
                       <<QString::number(SoundManager::VOLUME_LEV_5));
    layout->addWidget(comboBox , row , 1);
    comboBox->setObjectName("ToneVolume");
    row++;
    itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME);
    comboBox->setProperty("Item" , qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(activated(int)) , this , SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(NormalFunctionMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME , comboBox);

#ifdef Q_WS_QWS
#ifndef VITAVUE_15_INCHES
    // touch screen function
    label = new QLabel(trs("TouchScreen"));
    layout->addWidget(label, row, 0);
    d_ptr->touchLab = label;
    comboBox = new ComboBox();
    comboBox->addItem(trs("Off"));
    comboBox->addItem(trs("On"));
    layout->addWidget(comboBox, row, 1);
    row++;
    itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_CBO_TOUCH_SCREEN);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)) , this , SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(NormalFunctionMenuContentPrivate::ITEM_CBO_TOUCH_SCREEN, comboBox);
#endif
#endif

    if (systemManager.isSupport(CONFIG_WIFI))
    {
        // wifi profile
        btn = new Button(trs("WiFiProfile"));
        btn->setButtonStyle(Button::ButtonTextOnly);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_BTN_WIFI_PROFILE);
        btn->setProperty("Item", qVariantFromValue(itemID));
        layout->addWidget(btn, row, 1);
        row++;
    }

    // Software Version
    btn = new Button(trs("SoftwareVersion"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_BTN_SOFTWARE_VERSION);
    btn->setProperty("Item", qVariantFromValue(itemID));
    layout->addWidget(btn, row, 1);
    row++;

    // standy
    btn = new Button(trs("Standy"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_BTN_ENTER_STANDY);
    btn->setProperty("Item", qVariantFromValue(itemID));
    d_ptr->standbyBtn = btn;
#ifndef HIDE_STANDBY_FUNCTION
    layout->addWidget(btn, row, 1);
    row++;
#endif

    // night mode
    btn = new Button(trs("NightMode"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_BTN_NIGHT_MODE);
    btn->setProperty("Item", qVariantFromValue(itemID));
    layout->addWidget(btn, row, 1);
    row++;

    // demo mode
    btn = new Button(trs("DemoMode"));
    d_ptr->demoBtn = btn;
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    itemID = static_cast<int>(NormalFunctionMenuContentPrivate::ITEM_BTN_DEMO_MODE);
    btn->setProperty("Item", qVariantFromValue(itemID));
    layout->addWidget(btn, row, 1);
    row++;

    layout->setRowStretch(row, 1);
}

void NormalFunctionMenuContent::setShowParam(const QVariant &val)
{
    if (val.isValid())
    {
        QString objName = val.toString();
        if (!objName.isEmpty())
        {
            ComboBox *obj = findChild<ComboBox*>(objName);
            if (obj)
            {
                obj->setFocus();
            }
        }
    }
}

void NormalFunctionMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        NormalFunctionMenuContentPrivate::MenuItem item
                = (NormalFunctionMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case NormalFunctionMenuContentPrivate::ITEM_CBO_WAVE_LEN:
        {
            int waveLength;
            if (index == 0)
            {
                waveLength = 8;
            }
            else if (index == 1)
            {
                waveLength = 16;
            }
            else
            {
                waveLength = 32;
            }
            currentConfig.setNumValue("Event|WaveLength", waveLength);
            break;
        }
        case NormalFunctionMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS:
        {
            systemManager.setBrightness(static_cast<BrightnessLevel>(index));
            break;
        }
        case NormalFunctionMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME:
        {
            int volume = box->itemText(index).toInt();
            soundManager.setVolume(SoundManager::SOUND_TYPE_NOTIFICATION ,
                                   static_cast<SoundManager::VolumeLevel>(volume));
            systemConfig.setNumValue("General|KeyPressVolume", volume);
            break;
        }
#ifdef Q_WS_QWS
#ifndef VITAVUE_15_INCHES
        case NormalFunctionMenuContentPrivate::ITEM_CBO_TOUCH_SCREEN:
        {
            systemManager.setTouchScreenOnOff(index);
            softkeyManager.refreshTouchKey();
            break;
        }
#endif
#endif
        default:
            break;
        }
    }
}

void NormalFunctionMenuContent::onBtnReleasd()
{
    Button *btn = qobject_cast<Button*>(sender());
    int index = btn->property("Item").toInt();

    switch (index)
    {
        case NormalFunctionMenuContentPrivate::ITEM_BTN_CALCULATE:
        {
            CalculateWindow *w = CalculateWindow::getInstance();
            windowManager.showWindow(w, WindowManager::ShowBehaviorHideOthers);
        }
        break;
        case NormalFunctionMenuContentPrivate::ITEM_BTN_WIFI_PROFILE:
        {
            WiFiProfileWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
        case NormalFunctionMenuContentPrivate::ITEM_BTN_SOFTWARE_VERSION:
        {
            SoftwareVersionWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
        case NormalFunctionMenuContentPrivate::ITEM_BTN_ENTER_STANDY:
        {
            systemManager.setWorkMode(WORK_MODE_STANDBY);
        }
        break;
        case NormalFunctionMenuContentPrivate::ITEM_BTN_NIGHT_MODE:
        {
            NightModeWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
        case NormalFunctionMenuContentPrivate::ITEM_BTN_DEMO_MODE:
        {
            if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
            {
                systemManager.setWorkMode(WORK_MODE_NORMAL);
                d_ptr->demoBtn->setText(trs("DemoMode"));
                windowManager.closeAllWidows();
                break;
            }

            QString password;
            systemConfig.getStrValue("General|DemoModePassword", password);

            PasswordWindow w(trs("DemoModePassword"), password);
            w.setWindowTitle(btn->text());
            if (w.exec() != PasswordWindow::Accepted)
            {
                return;
            }

            systemManager.setWorkMode(WORK_MODE_DEMO);
            d_ptr->demoBtn->setText(trs("ExitDemoMode"));
            windowManager.closeAllWidows();
        }
        break;
    }
}

void NormalFunctionMenuContent::onPopupListItemFocusChanged(int volume)
{
    ComboBox *w = qobject_cast<ComboBox*>(sender());

    if (w == d_ptr->combos[NormalFunctionMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME])
    {
        soundManager.setVolume(SoundManager::SOUND_TYPE_NOTIFICATION , static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.keyPressTone();
    }
}

void NormalFunctionMenuContent::nightModeHandle(bool isNightMode)
{
    if (isNightMode)
    {
        d_ptr->combos[NormalFunctionMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS]->setEnabled(false);
        d_ptr->combos[NormalFunctionMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME]->setEnabled(false);
    }
    else
    {
        d_ptr->combos[NormalFunctionMenuContentPrivate::ITEM_CBO_SCREEN_BRIGHTNESS]->setEnabled(true);
        d_ptr->combos[NormalFunctionMenuContentPrivate::ITEM_CBO_KEYPRESS_VOLUME]->setEnabled(true);
    }

    // 根据夜间模式状态, 设置屏幕亮度和音量按键类型是否可用
    softkeyManager.setKeyTypeAvailable(SOFT_BASE_KEY_KEYBOARD_VOLUMN, !isNightMode);
    softkeyManager.setKeyTypeAvailable(SOFT_BASE_KEY_SCREEN_BRIGHTNESS, !isNightMode);
}
