/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/9
 **/

#include "ErrorLogEntranceContent.h"
#include "Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QHBoxLayout>
#include "ErrorLogWindow.h"
#include "WindowManager.h"

class ErrorLogEntranceContentPrivate
{
public:
    ErrorLogEntranceContentPrivate()
        : entranceErrLogBtn(NULL)
    {}

    Button *entranceErrLogBtn;
};
ErrorLogEntranceContent::ErrorLogEntranceContent()
    : MenuContent(trs("ServiceErrorLogEntrance"), trs("ServiceErrorLogEntranceDesc")),
      d_ptr(new ErrorLogEntranceContentPrivate)
{
}

ErrorLogEntranceContent::~ErrorLogEntranceContent()
{
}

void ErrorLogEntranceContent::layoutExec()
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    d_ptr->entranceErrLogBtn = new Button(QString("%1%2").arg(trs("ServiceErrorLogEntrance")).arg(">>"));
    d_ptr->entranceErrLogBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addStretch(1);
    layout->addWidget(d_ptr->entranceErrLogBtn, 1, Qt::AlignTop);
    connect(d_ptr->entranceErrLogBtn, SIGNAL(released()), this, SLOT(onButtonReleased()));
}

void ErrorLogEntranceContent::onButtonReleased()
{
    windowManager.showWindow(ErrorLogWindow::getInstance(),
                             WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
}
