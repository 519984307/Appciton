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
#include "SystemManager.h"
#include "O2Param.h"

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)

enum O2ConcentrationCalibrte
{
    O2_PERCENT_21,
    O2_PERCENT_100
};

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
        ITEM_BTN_O2_MODULE_CALIBRATION,
    };

    ModuleMaintainMenuContentPrivate() {}

    void loadOption();

    QMap<MenuItem, Button *> buttons;

    QList<Button *> percentBtns;
    QList<QLabel *> percentLab;

    int calibTimerID;
    int timeoutNum;
    O2ConcentrationCalibrte calibratePercent;
};

void ModuleMaintainMenuContentPrivate::loadOption()
{
#ifdef DISABLE_CO2_MODULE_MAINTAIN
    buttons[ITEM_BTN_CO2_MODULE_MAINTAIN]->setEnabled(false);
#endif
#ifdef DISABLE_ECG_MODULE_CALIBRATION
    buttons[ITEM_BTN_ECG_MODULE_CALIBRATION]->setEnabled(false);
#endif
    percentLab.at(O2_PERCENT_21)->setText(trs("WaitingCalibration"));
    percentLab.at(O2_PERCENT_100)->setText(trs("WaitingCalibration"));
}

ModuleMaintainMenuContent::ModuleMaintainMenuContent()
    : MenuContent(trs("ModuleMaintainMenu"), trs("ModuleMaintainMenuDesc")),
      d_ptr(new ModuleMaintainMenuContentPrivate)
{
    d_ptr->calibTimerID = -1;
    d_ptr->timeoutNum = 0;
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
    SystemManagerInterface *sysManager = SystemManagerInterface::getSystemManager();

    if (sysManager && sysManager->isSupport(PARAM_AG))
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

    if (sysManager && !sysManager->isSupport(CONFIG_TOUCH))
    {
        // touch screen is not support
        button->setEnabled(false);
    }
#endif

    // add o2 calibrate
    label = new QLabel(trs("O2CalibrateDesc"));
    layout->addWidget(label, d_ptr->buttons.count(), 0);
    button = new Button(""); //占位
    button->setBorderWidth(0);
    button->setEnabled(false);
    layout->addWidget(button, d_ptr->buttons.count(), 1);

    QGridLayout *layout1 = new QGridLayout();
    layout1->setMargin(10);

    button = new Button(trs("Percent21Calibration"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(percent21Released()));
    layout1->addWidget(button, 0, 0);
    d_ptr->percentBtns.append(button);

    label = new QLabel();
    layout1->addWidget(label, 0, 1, Qt::AlignCenter);
    d_ptr->percentLab.append(label);

    button = new Button(trs("Percent100Calibration"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(percent100Released()));
    layout1->addWidget(button, 1, 0);
    d_ptr->percentBtns.append(button);

    label = new QLabel();
    layout1->addWidget(label, 1, 1, Qt::AlignCenter);
    d_ptr->percentLab.append(label);

    layout1->setRowStretch(2, 1);

    layout->addLayout(layout1, d_ptr->buttons.count() + 1, -1);


    layout->setRowStretch(d_ptr->buttons.count() + 2, 1);
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
            SystemManagerInterface *sysManager;
            sysManager = SystemManagerInterface::getSystemManager();
            if (sysManager && sysManager->isTouchScreenOn())
            {
                QWSServer::instance()->closeMouse();
            }
            lightManager.stopHandlingLight(true);
            soundManager.stopHandlingSound(true);

            TSCalibrationWindow w;
            w.exec();

            if (sysManager && sysManager->isTouchScreenOn())
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

void ModuleMaintainMenuContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->calibTimerID == ev->timerId())
    {
        bool reply = o2Param.getCalibrationReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            if (reply && o2Param.getCalibrationResult())
            {
                d_ptr->percentLab.at(d_ptr->calibratePercent)->setText(trs("CalibrationSuccess"));
            }
            else
            {
                d_ptr->percentLab.at(d_ptr->calibratePercent)->setText(trs("CalibrationFail"));
            }
            killTimer(d_ptr->calibTimerID);
            d_ptr->calibTimerID = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
}

void ModuleMaintainMenuContent::percent21Released(void)
{
    if (o2Param.isServiceProviderOk())
    {
        o2Param.sendCalibration(1);
        d_ptr->calibTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
        d_ptr->calibratePercent = O2_PERCENT_21;
    }
}

void ModuleMaintainMenuContent::percent100Released(void)
{
    if (o2Param.isServiceProviderOk())
    {
        o2Param.sendCalibration(2);
        d_ptr->calibTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
        d_ptr->calibratePercent = O2_PERCENT_100;
    }
}
