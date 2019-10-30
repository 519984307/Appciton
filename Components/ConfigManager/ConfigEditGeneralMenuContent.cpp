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
#include <QRegExp>
#include "ConfigManager.h"
#include "KeyInputPanel.h"
#include "MessageBox.h"

#define MAX_CONFIG_NAME_LEN 64

class ConfigEditGeneralMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_CONFIG_NAME = 0,
    };
    explicit ConfigEditGeneralMenuContentPrivate(ConfigEditMenuWindow * const cmw);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, Button *> btns;
    ConfigEditMenuWindow *const cmw;
};

ConfigEditGeneralMenuContentPrivate
    ::ConfigEditGeneralMenuContentPrivate(ConfigEditMenuWindow * const cmw)
    : cmw(cmw)

{
    btns.clear();
}

ConfigEditGeneralMenuContent::ConfigEditGeneralMenuContent(ConfigEditMenuWindow *const cmw):
    MenuContent(trs("General"),
                trs("GeneralDesc")),
    d_ptr(new ConfigEditGeneralMenuContentPrivate(cmw))
{}

ConfigEditGeneralMenuContent::~ConfigEditGeneralMenuContent()
{
    delete d_ptr;
}

void ConfigEditGeneralMenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();
    d_ptr->btns[ConfigEditGeneralMenuContentPrivate
            ::ITEM_BTN_CONFIG_NAME]->setEnabled(!isOnlyToRead);
}

void ConfigEditGeneralMenuContentPrivate::loadOptions()
{
    btns[ITEM_BTN_CONFIG_NAME]->setText(cmw->getCurrentEditConfigName());
}
void ConfigEditGeneralMenuContent::layoutExec()
{
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
    KeyInputPanel panel;
    panel.setWindowTitle(trs("SetConfigName"));
    panel.setInitString(d_ptr->cmw->getCurrentEditConfigName());
    panel.setMaxInputLength(MAX_CONFIG_NAME_LEN);
    QString regKeyStr("[a-zA-Z]|[0-9]|_");
    panel.setBtnEnable(regKeyStr);

    if (QDialog::Accepted == panel.exec())
    {
        QString newStr(panel.getStrValue());
        // 防止命名相同
        if (configManager.hasExistConfig(newStr, d_ptr->cmw->getCurrentConfigType()))
        {
            MessageBox message(trs("Prompt"), trs("HasExistConfig"), false, true);
            message.exec();
            return;
        }

        d_ptr->cmw->setCurrentEditConfigName(newStr);
        d_ptr->btns[ConfigEditGeneralMenuContentPrivate::ITEM_BTN_CONFIG_NAME]->setText(newStr);
    }
}






