/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/
#include "ConfigEditGeneralMenuContent.h"
#include <QMap>
#include <QLabel>
#include "Button.h"
#include "LanguageManager.h"
#include <QGridLayout>
#include "KeyBoardPanel.h"
#include "ConfigEditMenuWindow.h"
#include <QRegExp>
#include "ConfigManager.h"

#define MAX_CONFIG_NAME_LEN 64

class ConfigEditGeneralMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_CONFIG_NAME = 0,
    };
    ConfigEditGeneralMenuContentPrivate();
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, Button *> btns;
};

ConfigEditGeneralMenuContentPrivate::ConfigEditGeneralMenuContentPrivate()
{
    btns.clear();
}

ConfigEditGeneralMenuContent::ConfigEditGeneralMenuContent():
    MenuContent(trs("General"),
                trs("GeneralDesc")),
    d_ptr(new ConfigEditGeneralMenuContentPrivate)
{}

ConfigEditGeneralMenuContent::~ConfigEditGeneralMenuContent()
{
    delete d_ptr;
}

void ConfigEditGeneralMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditGeneralMenuContentPrivate::loadOptions()
{
    btns[ITEM_BTN_CONFIG_NAME]->setText(ConfigEditMenuWindow
                                        ::getInstance()->getCurrentEditConfigName());
}
void ConfigEditGeneralMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    Button *button;

    label = new QLabel(trs("ConfigName"));
    layout->addWidget(label, d_ptr->btns.count(), 0);
    button = new Button("");
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, d_ptr->btns.count(), 1);
    d_ptr->btns.insert(ConfigEditGeneralMenuContentPrivate::ITEM_BTN_CONFIG_NAME,
                       button);

    layout->setRowStretch(d_ptr->btns.count(), 1);
}

void ConfigEditGeneralMenuContent::onBtnReleasedChanged()
{
    KeyBoardPanel panel;
    panel.setTitleBarText(trs("SetConfigName"));
    panel.setInitString(ConfigEditMenuWindow::getInstance()
                        ->getCurrentEditConfigName());
    panel.setMaxInputLength(MAX_CONFIG_NAME_LEN);
    QString regKeyStr("[a-zA-Z][0-9]|_");
    panel.setBtnEnable(regKeyStr);

    if (1 == panel.exec())
    {
        QString oldStr(d_ptr->btns[ConfigEditGeneralMenuContentPrivate
                                   ::ITEM_BTN_CONFIG_NAME]->text());
        QString newStr(panel.getStrValue());
        if (oldStr != newStr)
        {
            ConfigEditMenuWindow::getInstance()
            ->setCurrentEditConfigName(newStr);
            d_ptr->btns[ConfigEditGeneralMenuContentPrivate
                        ::ITEM_BTN_CONFIG_NAME]->setText(newStr);
        }
    }
}






