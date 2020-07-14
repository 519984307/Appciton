/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/16
 **/

#include "ModuleMaintainMenuContent.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#ifdef Q_WS_QWS
#include "TSCalibrationWindow.h"
#include <QWSServer>
#endif
#include <QLabel>
#include <QGridLayout>
#include "IConfig.h"
#include "IBPParam.h"
#include "KeyInputPanel.h"
#include "MessageBox.h"
#include "LightManager.h"
#include "AlarmIndicator.h"
#include "WindowManager.h"
#include "SystemManagerInterface.h"
#include "SoundManager.h"


class ModuleMaintainMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_AG_MODULE_CALIBRATION,
        ITEM_BTN_CO2_MODULE_MAINTAIN,
        ITEM_BTN_ECG_MODULE_CALIBRATION,
#ifdef  Q_WS_QWS
        ITEM_BTN_TOUCH_SCREEN_CALIBRATION,
#endif
    };

    ModuleMaintainMenuContentPrivate() {}

    void loadOption();

    QMap<MenuItem, Button *> buttons;
};

void ModuleMaintainMenuContentPrivate::loadOption()
{
#ifdef DISABLE_CO2_MODULE_MAINTAIN
    buttons[ITEM_BTN_CO2_MODULE_MAINTAIN]->setEnabled(false);
#endif
#ifdef DISABLE_ECG_MODULE_CALIBRATION
    buttons[ITEM_BTN_ECG_MODULE_CALIBRATION]->setEnabled(false);
#endif

    // update ibp calibration buttons enabled status
    buttons[ITEM_BTN_IBP1_PRESSURE_CALIBRATION]->setEnabled(ibpParam.isConnected());
    buttons[ITEM_BTN_IBP2_PRESSURE_CALIBRATION]->setEnabled(ibpParam.isConnected());
}

ModuleMaintainMenuContent::ModuleMaintainMenuContent()
    : MenuContent(trs("ModuleMaintainMenu"), trs("ModuleMaintainMenuDesc")),
      d_ptr(new ModuleMaintainMenuContentPrivate)
{
}

ModuleMaintainMenuContent::~ModuleMaintainMenuContent()
{
    delete d_ptr;
}

void ModuleMaintainMenuContent::readyShow()
{
    d_ptr->loadOption();
}

void ModuleMaintainMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    Button *button;
    QLabel *label;
    int itemID;
    SystemManagerInterface *systemManager = SystemManagerInterface::getSystemManager();

    if (systemManager && systemManager->isSupport(PARAM_AG))
    {
        // ag module calibration
        label = new QLabel(trs("AnaesthesiaModuleCalibration"));
        layout->addWidget(label, d_ptr->buttons.count(), 0);
        button = new Button(trs("Start"));
        button->setButtonStyle(Button::ButtonTextOnly);
        itemID = static_cast<int>(ModuleMaintainMenuContentPrivate::ITEM_BTN_AG_MODULE_CALIBRATION);
        button->setProperty("Item", qVariantFromValue(itemID));
        connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
        layout->addWidget(button, d_ptr->buttons.count(), 1);
        d_ptr->buttons.insert(ModuleMaintainMenuContentPrivate::ITEM_BTN_AG_MODULE_CALIBRATION, button);
    }

    // CO2 ModuleMaintenance
    label = new QLabel(trs("CO2ModuleMaintenance"));
    layout->addWidget(label, d_ptr->buttons.count(), 0);
    button = new Button(trs("Start"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ModuleMaintainMenuContentPrivate::ITEM_BTN_CO2_MODULE_MAINTAIN);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(ModuleMaintainMenuContentPrivate::ITEM_BTN_CO2_MODULE_MAINTAIN, button);

    // ECG Module Calibration
    label = new QLabel(trs("ECGModuleCalibration"));
    layout->addWidget(label, d_ptr->buttons.count(), 0);
    button = new Button(trs("Start"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ModuleMaintainMenuContentPrivate::ITEM_BTN_ECG_MODULE_CALIBRATION);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(ModuleMaintainMenuContentPrivate::ITEM_BTN_ECG_MODULE_CALIBRATION, button);


#ifdef Q_WS_QWS
    // Touch Screen Module Calibration
    label = new QLabel(trs("TouchScreenCalibration"));
    layout->addWidget(label, d_ptr->buttons.count(), 0);
    button = new Button(trs("Start"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ModuleMaintainMenuContentPrivate::ITEM_BTN_TOUCH_SCREEN_CALIBRATION);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(ModuleMaintainMenuContentPrivate::ITEM_BTN_TOUCH_SCREEN_CALIBRATION, button);

    if (systemManager && !systemManager->isSupport(CONFIG_TOUCH))
    {
        // touch screen is not support
        button->setEnabled(false);
    }
#endif

    layout->setRowStretch(d_ptr->buttons.count(), 1);
}

void ModuleMaintainMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        ModuleMaintainMenuContentPrivate::MenuItem item
                = (ModuleMaintainMenuContentPrivate::MenuItem)button->property("Item").toInt();
        switch (item) {
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_AG_MODULE_CALIBRATION:

            break;
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_CO2_MODULE_MAINTAIN:

            break;
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_ECG_MODULE_CALIBRATION:

            break;
#ifdef  Q_WS_QWS
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_TOUCH_SCREEN_CALIBRATION:
        {
            windowManager.closeAllWidows();
            SystemManagerInterface *systemManager;
            systemManager = SystemManagerInterface::getSystemManager();
            if (systemManager && systemManager->isTouchScreenOn())
            {
                QWSServer::instance()->closeMouse();
            }
            lightManager.stopHandlingLight(true);
            soundManager.stopHandlingSound(true);

            TSCalibrationWindow w;
            w.exec();

            if (systemManager && systemManager->isTouchScreenOn())
            {
                QWSServer::instance()->openMouse();
            }

            lightManager.stopHandlingLight(false);
            soundManager.stopHandlingSound(false);
        }
        break;
#endif
        default:
            break;
        }
    }
}
