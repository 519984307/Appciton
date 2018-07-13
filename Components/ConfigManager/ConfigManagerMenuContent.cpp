/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#include "ConfigManagerMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include <QList>
#include "SoundManager.h"
#include "SystemManager.h"
#include "IConfig.h"
#include "PasswordWidget.h"
#include <QVBoxLayout>


class ConfigManagerMenuContentPrivate
{
public:
    ConfigManagerMenuContentPrivate() : passwordWidget(NULL)
    {
    }

    PasswordWidget *passwordWidget;
};

ConfigManagerMenuContent::ConfigManagerMenuContent()
    : MenuContent(trs("ConfigManager"), trs("ConfigManagerDesc")),
      d_ptr(new ConfigManagerMenuContentPrivate)
{
}

ConfigManagerMenuContent::~ConfigManagerMenuContent()
{
    delete d_ptr;
}

void ConfigManagerMenuContent::readyShow()
{
}

void ConfigManagerMenuContent::layoutExec()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QString password;
    systemConfig.getStrValue("General|ConfigManagerPassword", password);
    d_ptr->passwordWidget = new PasswordWidget(trs("ConfigManagerPassword"), password);
    layout->addWidget(d_ptr->passwordWidget, Qt::AlignCenter);
    connect(d_ptr->passwordWidget, SIGNAL(correctPassword()), this, SLOT(userInputCorrect()));
}

void ConfigManagerMenuContent::userInputCorrect()
{
}
