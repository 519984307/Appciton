/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/24
 **/

#include "ScreenMenuContent.h"
#include "ComboBox.h"
#include "Button.h"
#include "LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include <ScreenLayoutWindow.h>
#include <WindowManager.h>

class ScreenMenuContentPrivate
{
public:
    ScreenMenuContentPrivate()
        :layoutCbo(NULL),
          configBtn(NULL)
    {}

    void loadOptions();

    ComboBox *layoutCbo;
    Button *configBtn;
};

void ScreenMenuContentPrivate::loadOptions()
{
}

ScreenMenuContent::ScreenMenuContent()
    : MenuContent(trs("ScreenConfig"), trs("ScreenConfigDesc")),
      d_ptr(new ScreenMenuContentPrivate)
{
}

ScreenMenuContent::~ScreenMenuContent()
{
    delete d_ptr;
}

void ScreenMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ScreenMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    QLabel *label;
    ComboBox *comboBox;
    int count = 0;

    // screen select
    label = new QLabel(trs("CurrentScreen"));
    layout->addWidget(label, count, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Standard")
                       << trs("_12Lead")
                       << trs("OxyCRG")
                       << trs("Trend")
                       << trs("BigFont")
                      );
    layout->addWidget(comboBox, count++, 1);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    d_ptr->layoutCbo = comboBox;

    d_ptr->configBtn = new Button(trs("ScreenLayout"));
    d_ptr->configBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->configBtn, count++, 1);
    connect(d_ptr->configBtn, SIGNAL(clicked()), this, SLOT(settingLayout()));

    layout->setRowStretch(count, 1);
}

void ScreenMenuContent::onComboxIndexChanged(int index)
{
}

void ScreenMenuContent::settingLayout()
{
    windowManager.showWindow(ScreenLayoutWindow::getInstance(), WindowManager::ShowBehaviorCloseOthers);
}
