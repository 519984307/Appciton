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
#include "PatientManager.h"
#include "WindowManager.h"

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
    : Dialog(), d_ptr(new SaveUserConfigWindowPrivate)
{
    setWindowTitle(trs("SaveAsUserConfig"));
    setFixedSize(510, 300);
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setMargin(10);
    vLayout->setSpacing(20);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    QLabel *hint = new QLabel;
    hint->setText(QString("%1\n%2").arg(trs("SaveTheCurrentSetting")).arg(trs("NameTheConfigurationName")));
    vLayout->addWidget(hint);

    QLabel *cofNameLbl = new QLabel;
    cofNameLbl->setText(trs("ConfigurationName"));
    hLayout->addWidget(cofNameLbl, 1);
    d_ptr->cofNameBtn = new Button;
    d_ptr->cofNameBtn->setButtonStyle(Button::ButtonTextOnly);
    hLayout->addWidget(d_ptr->cofNameBtn, 1);
    connect(d_ptr->cofNameBtn, SIGNAL(released()), this, SLOT(onConfigNameBtnReleased()));
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    d_ptr->okBtn = new Button;
    d_ptr->okBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->okBtn->setText(trs("Ok"));
    connect(d_ptr->okBtn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
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
    Dialog::showEvent(e);
}

void SaveUserConfigWindow::onBtnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->okBtn)
    {
        if (configManager.hasExistConfig(d_ptr->cofNameBtn->text(), patientManager.getType()))
        {
            MessageBox message(trs("Prompt"), trs("HasExistConfig"), false);
            windowManager.showWindow(&message, WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
        }
        else
        {
            configManager.saveUserDefineConfig(d_ptr->cofNameBtn->text(), &currentConfig, patientManager.getType());
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
    windowManager.showWindow(&inputPanel, WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
    if (inputPanel.result() == QDialog::Accepted)
    {
        btn->setText(inputPanel.getStrValue());
        if (btn->text().isEmpty())
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
    okBtn->setEnabled(false);
}
