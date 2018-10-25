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
#include "LanguageManager.h"
#ifdef Q_WS_QWS
#include "TSCalibrationWindow.h"
#include <QWSServer>
#endif
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"
#include "IMessageBox.h"
#include "Button.h"
#include "IBPParam.h"
#include "KeyInputPanel.h"
#include "MessageBox.h"

class ModuleMaintainMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_AG_MODULE_CALIBRATION,
        ITEM_BTN_IBP1_PRESSURE_CALIBRATION,
        ITEM_BTN_IBP2_PRESSURE_CALIBRATION,
        ITEM_BTN_CO2_MODULE_MAINTAIN,
        ITEM_BTN_NIBP_PRESSURE_TEST,
        ITEM_BTN_NIBP_LEAKAGE_DETECTION,
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

    // IBP1 pressure calibration
    label = new QLabel(trs("IBP1PressureCalibration"));
    layout->addWidget(label, d_ptr->buttons.count(), 0);
    button = new Button("80");
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ModuleMaintainMenuContentPrivate::ITEM_BTN_IBP1_PRESSURE_CALIBRATION);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(ModuleMaintainMenuContentPrivate::ITEM_BTN_IBP1_PRESSURE_CALIBRATION, button);

    // IBP2 pressure calibration
    label = new QLabel(trs("IBP2PressureCalibration"));
    layout->addWidget(label, d_ptr->buttons.count(), 0);
    button = new Button("80");
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ModuleMaintainMenuContentPrivate::ITEM_BTN_IBP2_PRESSURE_CALIBRATION);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(ModuleMaintainMenuContentPrivate::ITEM_BTN_IBP2_PRESSURE_CALIBRATION, button);

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

    // NIBP Pressure Test
    label = new QLabel(trs("NIBPPressureTest"));
    layout->addWidget(label, d_ptr->buttons.count(), 0);
    button = new Button(trs("Start"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ModuleMaintainMenuContentPrivate::ITEM_BTN_NIBP_PRESSURE_TEST);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(ModuleMaintainMenuContentPrivate::ITEM_BTN_NIBP_PRESSURE_TEST, button);

    // NIBP Leakage Detection
    label = new QLabel(trs("NIBPLeakageDetection"));
    layout->addWidget(label, d_ptr->buttons.count(), 0);
    button = new Button(trs("Start"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ModuleMaintainMenuContentPrivate::ITEM_BTN_NIBP_LEAKAGE_DETECTION);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(ModuleMaintainMenuContentPrivate::ITEM_BTN_NIBP_LEAKAGE_DETECTION, button);

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

    if (!systemManager.isSupport(CONFIG_TOUCH))
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
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_IBP1_PRESSURE_CALIBRATION:
        {
            KeyInputPanel numberPad;
            numberPad.setWindowTitle(trs("ServiceCalibrate"));
            numberPad.setMaxInputLength(7);
            numberPad.setInitString(button->text());
            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                quint16 value = text.toShort(&ok);
                if (ok)
                {
                    if (value >= 80 && value <= 300)
                    {
                        button->setText(text);
                        ibpParam.setCalibration(IBP_INPUT_1, value);
                    }
                    else
                    {
                        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + " 80-300 ", QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_IBP2_PRESSURE_CALIBRATION:
        {
            KeyInputPanel numberPad;
            numberPad.setWindowTitle(trs("ServiceCalibrate"));
            numberPad.setMaxInputLength(7);
            numberPad.setInitString(button->text());
            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                quint16 value = text.toShort(&ok);
                if (ok)
                {
                    if (value >= 80 && value <= 300)
                    {
                        button->setText(text);
                        ibpParam.setCalibration(IBP_INPUT_2, value);
                    }
                    else
                    {
                        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + " 80-300 ", QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_CO2_MODULE_MAINTAIN:

            break;
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_NIBP_PRESSURE_TEST:

            break;
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_NIBP_LEAKAGE_DETECTION:

            break;
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_ECG_MODULE_CALIBRATION:

            break;
#ifdef  Q_WS_QWS
        case ModuleMaintainMenuContentPrivate::ITEM_BTN_TOUCH_SCREEN_CALIBRATION:
        {
            windowManager.closeAllWidows();
            if (systemManager.isTouchScreenOn())
            {
                QWSServer::instance()->closeMouse();
            }

            TSCalibrationWindow w;
            w.exec();

            if (systemManager.isTouchScreenOn())
            {
                QWSServer::instance()->openMouse();
            }
        }
        break;
#endif
        default:
            break;
        }
    }
}
