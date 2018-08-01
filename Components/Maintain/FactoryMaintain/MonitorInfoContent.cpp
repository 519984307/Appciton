/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/

#include "MonitorInfoContent.h"
#include <QMap>
#include <QGridLayout>
#include <QLabel>
#include "LanguageManager.h"
#include "IConfig.h"
#include "KeyBoardPanel.h"
#include "Button.h"
#include <QApplication>
#include <QDesktopWidget>

class MonitorInfoContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_LAB_CMLV_WORKTIME = 0,
        ITEM_LAB_TEM_INSIDECASE,
        ITEM_LAB_BAT_LEV,
        ITEM_LAB_BAT_VOLT,
        ITEM_LAB_MACHINE_TYPE,
        ITEM_LAB_MAC_ADDR,
        ITEM_LAB_SCR_RESOLASIZE,
        ITEM_BTN_ELEC_SERIALNUM,
    };
    MonitorInfoContentPrivate();
    /**
     * @brief loadOptions
     */
    void loadOptions();
    QMap <MenuItem, QLabel *> labs;
    Button *button;
};

MonitorInfoContentPrivate::MonitorInfoContentPrivate():
    button(NULL)
{
    labs.clear();
}

MonitorInfoContent::~MonitorInfoContent()
{
    delete d_ptr;
}

MonitorInfoContent::MonitorInfoContent()
    : MenuContent(trs("MonitorInfoMenu"),
                  trs("MonitorInfoMenuDesc")),
      d_ptr(new MonitorInfoContentPrivate)
{
}

void MonitorInfoContentPrivate::loadOptions()
{

    QDesktopWidget *w = QApplication::desktop();
    QString temStr;

    labs[ITEM_LAB_SCR_RESOLASIZE]->setText(trs(QString("%1*%2")
                                         .arg(w->width())
                                         .arg(w->height())));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|CumulativeWorkingTime", temStr);
    labs[ITEM_LAB_CMLV_WORKTIME]->setText(trs(temStr));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|TemperatureInsideCase", temStr);
    labs[ITEM_LAB_TEM_INSIDECASE]->setText(trs(temStr));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|BatteryLevel", temStr);
    labs[ITEM_LAB_BAT_LEV]->setText(trs(temStr));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|BatteryVoltage", temStr);
    labs[ITEM_LAB_BAT_VOLT]->setText(trs(temStr));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|MachineType", temStr);
    labs[ITEM_LAB_MACHINE_TYPE]->setText(trs(temStr));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|MACAddress", temStr);
    labs[ITEM_LAB_MAC_ADDR]->setText(trs(temStr));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|ElectronicSerialNumber", temStr);
    button->setText(trs(temStr));
}

void MonitorInfoContent::readyShow()
{
    d_ptr->loadOptions();
}

void MonitorInfoContent::onBtnReleasedChanged()
{
    KeyBoardPanel setSerialNumber;
    setSerialNumber.setMaxInputLength(16);
    setSerialNumber.setTitleBarText(trs("SetElectronicSerialNumber"));
    setSerialNumber.setInitString(d_ptr->button->text());
    QString regKeyStr("[a-zA-Z][0-9]|_");
    setSerialNumber.setBtnEnable(regKeyStr);
    if (setSerialNumber.exec())
    {
        QString serialNum = setSerialNumber.getStrValue();
        systemConfig.setStrValue("MonitorInfo|ElectronicSerialNumber", serialNum);
        d_ptr->button->setText(trs(serialNum));
    }
}

void MonitorInfoContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *labelLeft;
    QLabel *labelRight;

    labelLeft = new QLabel(trs("CumulativeWorkingTime"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoContentPrivate
                       ::ITEM_LAB_CMLV_WORKTIME, labelRight);

    labelLeft = new QLabel(trs("TemperatureInsideCase"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoContentPrivate
                       ::ITEM_LAB_TEM_INSIDECASE, labelRight);

    labelLeft = new QLabel(trs("BatteryLevel"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoContentPrivate
                       ::ITEM_LAB_BAT_LEV, labelRight);

    labelLeft = new QLabel(trs("BatteryVoltage"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoContentPrivate
                       ::ITEM_LAB_BAT_VOLT, labelRight);

    labelLeft = new QLabel(trs("MachineType"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoContentPrivate
                       ::ITEM_LAB_MACHINE_TYPE, labelRight);

    labelLeft = new QLabel(trs("MACAddress"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoContentPrivate
                       ::ITEM_LAB_MAC_ADDR, labelRight);

    labelLeft = new QLabel(trs("ScreenResolationSize"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoContentPrivate
                       ::ITEM_LAB_SCR_RESOLASIZE, labelRight);

    labelLeft = new QLabel(trs("ElectronicSerialNumber"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    d_ptr->button = new Button("");
    d_ptr->button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->button, d_ptr->labs.count(), 1);
    connect(d_ptr->button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));

    layout->setRowStretch((d_ptr->labs.count() + 1), 1);
}
















