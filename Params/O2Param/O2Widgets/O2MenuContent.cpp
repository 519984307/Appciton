/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/14
 **/

#include "O2MenuContent.h"
#include "LanguageManager.h"
#include <QGridLayout>
#include "Button.h"
#include "ParamInfo.h"
#include "AlarmLimitWindow.h"
#include "WindowManager.h"

class O2MenuContentPrivate
{
public:
    O2MenuContentPrivate() {}
};
O2MenuContent::O2MenuContent()
    : MenuContent(trs("O2Menu"),
                  trs("O2MenuDesc")),
      d_ptr(new O2MenuContentPrivate())
{
}

O2MenuContent::~O2MenuContent()
{
    delete d_ptr;
}

void O2MenuContent::layoutExec()
{
    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);

    Button  *btn = new Button(QString("%1%2").
                              arg(trs("AlarmSettingUp")).
                              arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    glayout->addWidget(btn, 0, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 1);
    glayout->setRowStretch(1, 1);
}

void O2MenuContent::readyShow()
{
}

void O2MenuContent::onAlarmBtnReleased()
{
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_O2, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}
