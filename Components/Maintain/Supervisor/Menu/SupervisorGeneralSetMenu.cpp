/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/20
 **/


#include <QVBoxLayout>
#include <QRegExp>
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IButton.h"
#include "IComboList.h"
#include "SupervisorGeneralSetMenu.h"
#include "SupervisorMenuManager.h"
#include "SupervisorConfigManager.h"
#include "PatientDefine.h"
#include "KeyBoardPanel.h"
#include "Debug.h"

SupervisorGeneralSetMenu *SupervisorGeneralSetMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorGeneralSetMenu::SupervisorGeneralSetMenu() : SubMenu(trs("SupervisorGeneralMenu"))
{
    setDesc(trs("SupervisorGeneralMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SupervisorGeneralSetMenu::readyShow()
{
    QString tmpStr;
    currentConfig.getStrValue("General|DeviceIdentifier", tmpStr);
    _deviceID->setValue(tmpStr);

    tmpStr.clear();
    currentConfig.getStrValue("General|Password", tmpStr);
    _password->setValue(tmpStr);

    int value = 0;
    currentConfig.getNumValue("General|DefaultPatientType", value);
    if (value < PATIENT_TYPE_NR)
    {
        _patType->setCurrentIndex(value);
    }

    currentConfig.getNumValue("General|DefaultDisplayBrightness", value);
    _displayBrightness->setCurrentIndex(value);

    currentConfig.getNumValue("General|FullDisclosureRecording", value);
    _recordingFD->setCurrentIndex(value);
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SupervisorGeneralSetMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    // ID
    _deviceID = new LabelButton(trs("SupervisorDeviceIdentifier"));
    _deviceID->setFont(fontManager.textFont(fontSize));
    _deviceID->label->setFixedSize(labelWidth, ITEM_H);
    _deviceID->button->setFixedSize(btnWidth, ITEM_H);
    connect(_deviceID->button, SIGNAL(released(int)), this, SLOT(_idReleased()));
    mainLayout->addWidget(_deviceID);

    // 密码
    _password = new LabelButton(trs("PassWord"));
    _password->label->setFixedSize(labelWidth, ITEM_H);
    _password->button->setFixedSize(btnWidth, ITEM_H);
    _password->setFont(fontManager.textFont(fontSize));
    connect(_password->button, SIGNAL(released(int)), this, SLOT(_passwordReleased()));
    mainLayout->addWidget(_password);

    // 病人类型
    _patType = new IComboList(trs("SupervisorDefaultPatType"));
    _patType->label->setFixedSize(labelWidth, ITEM_H);
    _patType->combolist->setFixedSize(btnWidth, ITEM_H);
    _patType->setFont(fontManager.textFont(fontSize));
    _patType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_ADULT)));
    _patType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_PED)));
    _patType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_NEO)));
    connect(_patType->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_patTypeChange(int)));
    mainLayout->addWidget(_patType);

    // 屏幕亮度。
    _displayBrightness = new IComboList(trs("DefaultSystemBrightness"));
    _displayBrightness->setFont(fontManager.textFont(fontSize));
    for (int i = BRT_LEVEL_1; i < BRT_LEVEL_NR; i++)
    {
        _displayBrightness->addItem(QString::number(i + 1));
    }
    connect(_displayBrightness, SIGNAL(currentIndexChanged(int)), this, SLOT(_displayBrightnessChange(int)));
    _displayBrightness->label->setFixedSize(labelWidth, ITEM_H);
    _displayBrightness->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_displayBrightness);

    _recordingFD = new IComboList(trs("FullDisclosureRecording"));
    _recordingFD->label->setFixedSize(labelWidth, ITEM_H);
    _recordingFD->combolist->setFixedSize(btnWidth, ITEM_H);
    _recordingFD->setFont(fontManager.textFont(fontSize));
    _recordingFD->addItem(trs("Disable"));
    _recordingFD->addItem(trs("Enable"));
    connect(_recordingFD->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_recordingFDChange(int)));
    mainLayout->addWidget(_recordingFD);
}

/**************************************************************************************************
 * 标识符槽函数。
 *************************************************************************************************/
void SupervisorGeneralSetMenu::_idReleased()
{
    KeyBoardPanel idPanel;
    idPanel.setTitleBarText(trs("SupervisorDeviceIdentifier"));
    idPanel.setInitString(_deviceID->button->text());
    idPanel.setMaxInputLength(MAX_DEVICE_ID_LEN - 1);

    QString regKeyStr("[a-zA-Z]|[0-9]|_");
    idPanel.setBtnEnable(regKeyStr);

    if (1 == idPanel.exec())
    {
        QString oldStr = _deviceID->button->text();
        QString text = idPanel.getStrValue();

        if (oldStr != text)
        {
            _deviceID->setValue(text);
            currentConfig.setStrValue("General|DeviceIdentifier", text);
        }
    }
}

/**************************************************************************************************
 * 密码槽函数。
 *************************************************************************************************/
void SupervisorGeneralSetMenu::_passwordReleased()
{
    KeyBoardPanel idPanel(KeyBoardPanel::KEY_TYPE_NUMBER);
    idPanel.setTitleBarText(trs("PassWord"));
    idPanel.setInitString(_password->button->text());
    idPanel.setMaxInputLength(11);
    idPanel.setSpaceEnable(false);
    idPanel.setSymbolEnable(false);
    idPanel.setKeytypeSwitchEnable(false);

    if (1 == idPanel.exec())
    {
        QString oldStr = _password->button->text();
        QString text = idPanel.getStrValue();

        if (oldStr != text)
        {
            _password->setValue(text);
            currentConfig.setStrValue("General|Password", text);
        }
    }
}

/**************************************************************************************************
 * 病人类型槽函数。
 *************************************************************************************************/
void SupervisorGeneralSetMenu::_patTypeChange(int index)
{
    currentConfig.setNumValue("General|DefaultPatientType", index);
}

/**************************************************************************************************
 * default display brightness。
 *************************************************************************************************/
void SupervisorGeneralSetMenu::_displayBrightnessChange(int index)
{
    currentConfig.setNumValue("General|DefaultDisplayBrightness", index);
}

/**************************************************************************************************
 * recording fd slot。
 *************************************************************************************************/
void SupervisorGeneralSetMenu::_recordingFDChange(int index)
{
    currentConfig.setNumValue("General|FullDisclosureRecording", index);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorGeneralSetMenu::~SupervisorGeneralSetMenu()
{
}
