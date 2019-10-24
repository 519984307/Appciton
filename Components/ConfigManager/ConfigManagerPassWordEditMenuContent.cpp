/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/17
 **/

#include "ConfigManagerPassWordEditMenuContent.h"
#include "LanguageManager.h"
#include "Button.h"
#include <QGridLayout>
#include "KeyInputPanel.h"
#include "IConfig.h"
#include "MessageBox.h"
#include "WindowManager.h"

class ConfigManagerPassWordEditMenuContentPrivate
{
  public:
    ConfigManagerPassWordEditMenuContentPrivate()
    {
    }
};

ConfigManagerPassWordEditMenuContent::ConfigManagerPassWordEditMenuContent()
                                    : MenuContent(trs("ChangePassword"),
                                                  trs("ChangePasswordDesc")),
                                      d_ptr(new ConfigManagerPassWordEditMenuContentPrivate)
{
}

ConfigManagerPassWordEditMenuContent::~ConfigManagerPassWordEditMenuContent()
{
    delete d_ptr;
}

void ConfigManagerPassWordEditMenuContent::layoutExec()
{
    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);

    Button *btn = new Button(QString("%1 >>").arg(trs("ChangePassword")));
    btn->setButtonStyle(Button::ButtonTextOnly);
    glayout->addWidget(btn, 0, 1);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 1);
    glayout->setRowStretch(1, 1);

    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
}

static bool checkPasswordValue(const QString &password)
{
    if (password.length() != 8)
    {
        return false;
    }
    return true;
}

void ConfigManagerPassWordEditMenuContent::onBtnReleased()
{
    // 调用数字键盘
    KeyInputPanel numberPad(KeyInputPanel::KEY_TYPE_NUMBER);
    // 使能键盘的有效字符
    QString rec("[0-9]");
    numberPad.setBtnEnable(rec);
    // 设置键盘标题
    numberPad.setWindowTitle(trs("ChangePassword"));
    // 最大输入长度
    numberPad.setMaxInputLength(8);
    // 固定为数字键盘
    numberPad.setKeytypeSwitchEnable(false);
    numberPad.setSymbolEnable(false);
    numberPad.setSpaceEnable(false);
    // 设置初始字符串 placeholder模式
    QString lastPassword;
    systemConfig.getStrValue("General|ConfigManagerPassword", lastPassword);
    lastPassword = QString("%1:%2").arg(trs("OldPassword")).arg(lastPassword);
    numberPad.setInitString(lastPassword, true);
    numberPad.setCheckValueHook(checkPasswordValue);
    QString invalidStr = trs("Input8DigitsPassword");
    numberPad.setInvalidHint(invalidStr);

    windowManager.showWindow(&numberPad,
                             WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
    if (numberPad.result() == QDialog::Rejected)
    {
        return;
    }
    QString prePassword = numberPad.getStrValue();
    if (systemConfig.setStrValue("General|ConfigManagerPassword", prePassword))
    {
        MessageBox message(trs("ChangePassword"), trs("EditPasswordSuccess"), false);
        message.exec();
    }
    else
    {
        MessageBox message(trs("ChangePassword"), trs("EditPasswordNotSuccess"), false);
        message.exec();
    }
}
