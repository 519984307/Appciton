/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/16
 **/

#include "ArrhythmiaThresholdMenu.h"
#include "LanguageManager.h"
#include "ECGAlg2SoftInterface.h"
#include "Button.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVariant>
#include "KeyInputPanel.h"
#include "WindowManager.h"
#include "MessageBox.h"
#include "ECGParam.h"
#include "ComboBox.h"

class ArrhythmiaThresholdMenuPrivate
{
public:
    ArrhythmiaThresholdMenuPrivate()
        : pauseCbo(NULL)
    {}

    // load settings
    void loadOptions();

    QMap<ECGAlg::ARRPara, Button *> buttons;
    ComboBox *pauseCbo;
    QMap<ECGAlg::ARRPara, QLabel *> nameLabs;
    QMap<ECGAlg::ARRPara, QLabel *> unitLabs;
};
ArrhythmiaThresholdMenu::ArrhythmiaThresholdMenu()
    : MenuContent(trs("ArrhythmiaThresholdMenu"), trs("ArrhythmiaThresholdMenuDesc")),
      d_ptr(new ArrhythmiaThresholdMenuPrivate)
{
}

ArrhythmiaThresholdMenu::~ArrhythmiaThresholdMenu()
{
}

void ArrhythmiaThresholdMenu::readyShow()
{
}

void ArrhythmiaThresholdMenu::layoutExec()
{
    if (layout())
    {
        return;
    }

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label;
    QLabel *unitLab;
    Button *button;
    QHBoxLayout *hLayout;
    int itemID;

    // 停搏报警延迟时间
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Asystole"));
    d_ptr->nameLabs.insert(ECGAlg::ASYS_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::ASYS_THRESHOLD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::ASYS_THRESHOLD, button);
    unitLab = new QLabel("s");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::ASYS_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 心脏暂停时间
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("HeartPause"));
    hLayout->addWidget(label, 2);
    d_ptr->pauseCbo = new ComboBox();
    d_ptr->pauseCbo->addItems(QStringList()
                              << "1.5"
                              << "2.0"
                              << "2.5"
                              );
    connect(d_ptr->pauseCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(pauseChangeSlot(int)));
    hLayout->addWidget(d_ptr->pauseCbo, 1);
    unitLab = new QLabel("s");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::PAUSE_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 心动过速阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Tachycardia"));
    d_ptr->nameLabs.insert(ECGAlg::TACHY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::TACHY_THRESHOLD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::TACHY_THRESHOLD, button);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::TACHY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 心动过缓阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Bradycardia"));
    d_ptr->nameLabs.insert(ECGAlg::BRADY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::BRADY_THRESHOLD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::BRADY_THRESHOLD, button);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::BRADY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 极度心动过速阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Extachycardia"));
    d_ptr->nameLabs.insert(ECGAlg::EXTRETACHY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::EXTRETACHY_THRESHOLD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::EXTRETACHY_THRESHOLD, button);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::EXTRETACHY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 极度心动过缓阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Exbradycardia"));
    d_ptr->nameLabs.insert(ECGAlg::EXTREBRADY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::EXTREBRADY_THRESHOLD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::EXTREBRADY_THRESHOLD, button);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::EXTREBRADY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 室速心率阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("VentTachy"));
    d_ptr->nameLabs.insert(ECGAlg::VENT_TACHY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::VENT_TACHY_THRESHOLD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::VENT_TACHY_THRESHOLD, button);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::VENT_TACHY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 室缓心率阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("VentBrady"));
    d_ptr->nameLabs.insert(ECGAlg::VENT_BRADY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::VENT_BRADY_THRESHOLD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::VENT_BRADY_THRESHOLD, button);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::VENT_BRADY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 室速PVC阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("TachyPVC"));
    d_ptr->nameLabs.insert(ECGAlg::VENT_TACHY_PVC_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::VENT_TACHY_PVC_THRESHOLD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::VENT_TACHY_PVC_THRESHOLD, button);
    unitLab = new QLabel("beats");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::VENT_TACHY_PVC_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 室缓PVC阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("BradyPVC"));
    d_ptr->nameLabs.insert(ECGAlg::VENT_BRADY_PVC_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::VENT_BRADY_PVC_THRESHOLD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::VENT_BRADY_PVC_THRESHOLD, button);
    unitLab = new QLabel("beats");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::VENT_BRADY_PVC_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 多形PVC窗宽
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("MultifPVC"));
    d_ptr->nameLabs.insert(ECGAlg::MULTIF_PVC_WINDOW, label);
    hLayout->addWidget(label, 2);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ECGAlg::MULTIF_PVC_WINDOW);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(ECGAlg::MULTIF_PVC_WINDOW, button);
    unitLab = new QLabel("beats");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::MULTIF_PVC_WINDOW, unitLab);
    layout->addLayout(hLayout);

    d_ptr->loadOptions();
}

void ArrhythmiaThresholdMenu::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    ECGAlg::ARRPara item = (ECGAlg::ARRPara)button->property("Item").toInt();

    KeyInputPanel numberPad;
    QString rec("[0-9]|[.]");
    numberPad.setBtnEnable(rec);
    QString str(d_ptr->nameLabs[item]->text());
    str += "(";
    str += trs("Unit");
    str += ": ";
    str += trs(d_ptr->unitLabs[item]->text());
    str += ")";
    numberPad.setWindowTitle(str);
    numberPad.setMaxInputLength(7);
    numberPad.setInitString(d_ptr->buttons[item]->text());
    if (numberPad.exec())
    {
        QString text = numberPad.getStrValue();
        bool ok = false;
        float value = text.toFloat(&ok);
        if (ok)
        {
            bool isValid = true;
            float highLimit = 0;
            float lowLimit = 0;
            QString rangeStr;
            switch (item)
            {
            case ECGAlg::ASYS_THRESHOLD:
                highLimit = 10;
                lowLimit = 5;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            case ECGAlg::TACHY_THRESHOLD:
                highLimit = 300;
                lowLimit = 60;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            case ECGAlg::BRADY_THRESHOLD:
                highLimit = 120;
                lowLimit = 15;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            case ECGAlg::EXTRETACHY_THRESHOLD:
                highLimit = 300;
                lowLimit = 120;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            case ECGAlg::EXTREBRADY_THRESHOLD:
                highLimit = 60;
                lowLimit = 15;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            case ECGAlg::VENT_TACHY_THRESHOLD:
                highLimit = 200;
                lowLimit = 100;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            case ECGAlg::VENT_BRADY_THRESHOLD:
                highLimit = 60;
                lowLimit = 15;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            case ECGAlg::VENT_TACHY_PVC_THRESHOLD:
                highLimit = 99;
                lowLimit = 3;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            case ECGAlg::VENT_BRADY_PVC_THRESHOLD:
                highLimit = 99;
                lowLimit = 3;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            case ECGAlg::MULTIF_PVC_WINDOW:
                highLimit = 31;
                lowLimit = 3;
                if (value >= lowLimit && value <= highLimit)
                {
                    isValid = true;
                }
                else
                {
                    isValid = false;
                }
                break;
            default:
                break;
            }

            if (isValid)
            {
                ecgParam.setARRThreshold(item, value);
                button->setText(text);
            }
            else
            {
                rangeStr = QString::number(lowLimit) + "-" + QString::number(highLimit);
                MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + rangeStr, QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
        }
    }
}

void ArrhythmiaThresholdMenu::pauseChangeSlot(int index)
{
    ecgParam.setARRThreshold(ECGAlg::PAUSE_THRESHOLD, index);
}

void ArrhythmiaThresholdMenuPrivate::loadOptions()
{
    buttons[ECGAlg::ASYS_THRESHOLD]->setText("5");
    pauseCbo->setCurrentIndex(0);
    buttons[ECGAlg::TACHY_THRESHOLD]->setText("120");
    buttons[ECGAlg::BRADY_THRESHOLD]->setText("50");
    buttons[ECGAlg::EXTRETACHY_THRESHOLD]->setText("160");
    buttons[ECGAlg::EXTREBRADY_THRESHOLD]->setText("35");
    buttons[ECGAlg::VENT_TACHY_THRESHOLD]->setText("130");
    buttons[ECGAlg::VENT_BRADY_THRESHOLD]->setText("40");
    buttons[ECGAlg::VENT_TACHY_PVC_THRESHOLD]->setText("6");
    buttons[ECGAlg::VENT_BRADY_PVC_THRESHOLD]->setText("5");
    buttons[ECGAlg::MULTIF_PVC_WINDOW]->setText("15");
}
