/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/26
 **/

#include "SaveUserConfigWindow.h"
#include "LanguageManager.h"
#include <QLabel>
#include "Button.h"
#include <QBoxLayout>
#include "EnglishInputPanel.h"
#include "ConfigManager.h"
#include "MessageBox.h"

#define MaxInputLength 12

class SaveUserConfigWindowPrivate
{
public:
    SaveUserConfigWindowPrivate()
        : cofNameBtn(NULL),
          okBtn(NULL),
          cancelBtn(NULL)
    {}
    ~SaveUserConfigWindowPrivate(){}

    void loadOption();
    Button *cofNameBtn;
    Button *okBtn;
    Button *cancelBtn;
};

SaveUserConfigWindow::SaveUserConfigWindow()
    : Window(), d_ptr(new SaveUserConfigWindowPrivate)
{
    setWindowTitle(trs("SaveAsUserConfig"));
    setFixedSize(510, 300);
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setMargin(10);
    vLayout->setSpacing(20);
    QHBoxLayout *hLayout = new QHBoxLayout;
    QLabel *hint = new QLabel;
    hint->setWordWrap(true);
    hint->setText(trs("SaveCurrentSettingAndConfigurationName"));
    vLayout->addWidget(hint);

    QLabel *cofNameLbl = new QLabel;
    cofNameLbl->setText(trs("ConfigurationName"));
    hLayout->addWidget(cofNameLbl);
    d_ptr->cofNameBtn = new Button;
    d_ptr->cofNameBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->cofNameBtn->setFixedWidth(280);
    hLayout->addWidget(d_ptr->cofNameBtn);
    connect(d_ptr->cofNameBtn, SIGNAL(released()), this, SLOT(onConfigNameBtnReleased()));
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    d_ptr->okBtn = new Button;
    d_ptr->okBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->okBtn->setText(trs("Ok"));
    connect(d_ptr->okBtn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    hLayout = new QHBoxLayout;
    hLayout->addWidget(d_ptr->okBtn);

    d_ptr->cancelBtn = new Button;
    d_ptr->cancelBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->cancelBtn->setText(trs("Cancel"));
    connect(d_ptr->cancelBtn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    hLayout->addWidget(d_ptr->cancelBtn);
    vLayout->addLayout(hLayout);
    setWindowLayout(vLayout);
}

SaveUserConfigWindow::~SaveUserConfigWindow()
{
    delete d_ptr;
}

void SaveUserConfigWindow::showEvent(QShowEvent *e)
{
    d_ptr->loadOption();
    Window::showEvent(e);
}

void SaveUserConfigWindow::onBtnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->okBtn)
    {
        if (configManager.hasExistConfig(d_ptr->cofNameBtn->text()))
        {
            MessageBox message(trs("Prompt"), trs("HasExistConfig"), false);
            message.exec();
        }
        else
        {
            configManager.saveUserDefineConfig(d_ptr->cofNameBtn->text(), &currentConfig);
        }
        this->close();
    }
    else if (btn == d_ptr->cancelBtn)
    {
        this->close();
    }
}

void SaveUserConfigWindow::onConfigNameBtnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    EnglishInputPanel inputPanel;
    inputPanel.setInitString(btn->text());
    inputPanel.setMaxInputLength(MaxInputLength);
    if (inputPanel.exec())
    {
        btn->setText(inputPanel.getStrValue());
        if (btn->text() == "")
        {
            d_ptr->okBtn->setEnabled(false);
        }
        else
        {
            d_ptr->okBtn->setEnabled(true);
        }
    }
}

void SaveUserConfigWindowPrivate::loadOption()
{
    cofNameBtn->setText("");
}
