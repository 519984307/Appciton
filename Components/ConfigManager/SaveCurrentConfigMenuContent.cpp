/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/26
 **/

#include "SaveCurrentConfigMenuContent.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QGridLayout>
#include "SaveUserConfigWindow.h"
#include "WindowManager.h"
#include "ConfigManager.h"
#include "MessageBox.h"

class SaveCurrentConfigMenuContentPrivate
{
public:
    SaveCurrentConfigMenuContentPrivate(){}
    ~SaveCurrentConfigMenuContentPrivate(){}
};

SaveCurrentConfigMenuContent::SaveCurrentConfigMenuContent()
    : MenuContent(trs("SaveCurrentSettingAs"), trs("SaveCurrentSettingAsDesc")),
      d_ptr(new SaveCurrentConfigMenuContentPrivate)
{
}

SaveCurrentConfigMenuContent::~SaveCurrentConfigMenuContent()
{
}

void SaveCurrentConfigMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    Button *btn = new Button;
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setText(QString("%1 >>").arg(trs("SaveCurrentSetting")));
    layout->addWidget(btn, 0 , 1);
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
}

void SaveCurrentConfigMenuContent::onBtnReleased()
{
    if (configManager.getUserDefineConfigInfos().count()
            >= configManager.getUserDefineConfigMaxLen())
    {
        MessageBox message(trs("Prompt"),
                           trs("reachedTheMaximunOfConfigurations"),
                           false,
                           true);
        message.setFixedSize(360, 220);
        windowManager.showWindow(&message,
                                 WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
    }
    else
    {
        SaveUserConfigWindow win;
        windowManager.showWindow(&win,
                                 WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
    }
}
