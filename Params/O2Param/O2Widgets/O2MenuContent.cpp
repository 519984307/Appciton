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
#include "Framework/Language/LanguageManager.h"
#include <QGridLayout>
#include "Button.h"
#include "ParamInfo.h"
#include "AlarmLimitWindow.h"
#include "WindowManager.h"
#include "ComboBox.h"
#include "O2Param.h"
#include "SpinBox.h"
#include <QLabel>

class O2MenuContentPrivate
{
public:
    O2MenuContentPrivate(){}
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
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    // 添加报警设置链接
    QLabel *label = new QLabel();
    layout->addWidget(label, 0, 0);
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, 0, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(1, 1);
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
