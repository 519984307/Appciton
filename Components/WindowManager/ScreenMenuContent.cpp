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
#include "ParaColorWindow.h"
#include "LayoutManager.h"
#include "BigFontLayoutWindow.h"

class ScreenMenuContentPrivate
{
public:
    ScreenMenuContentPrivate()
        :layoutCbo(NULL)
    {}

    void loadOptions();

    ComboBox *layoutCbo;

    enum ButtonItem
    {
        BUTTON_SCREEN_STANDARD_LAYOUT,
        BUTTON_SCREEN_BIG_FONT_LAYOUT,
        BUTTON_PARA_COLOR,
        BUTTON_NR
    };
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

    Button *button;
    button = new Button(trs("StandardScreenLayout"));
    button->setButtonStyle(Button::ButtonTextOnly);
    int item = static_cast<int>(ScreenMenuContentPrivate::BUTTON_SCREEN_STANDARD_LAYOUT);
    button->setProperty("Item", qVariantFromValue(item));
    layout->addWidget(button, count++, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(onBtnClick()));

    button = new Button(trs("BigFontScreenLayout"));
    button->setButtonStyle(Button::ButtonTextOnly);
    item = static_cast<int>(ScreenMenuContentPrivate::BUTTON_SCREEN_BIG_FONT_LAYOUT );
    button->setProperty("Item", qVariantFromValue(item));
    layout->addWidget(button, count++, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(onBtnClick()));

    button = new Button(trs("ParameterColor"));
    button->setButtonStyle(Button::ButtonTextOnly);
    item = static_cast<int>(ScreenMenuContentPrivate::BUTTON_PARA_COLOR);
    button->setProperty("Item", qVariantFromValue(item));
    layout->addWidget(button, count++, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(onBtnClick()));

    layout->setRowStretch(count, 1);
}

void ScreenMenuContent::onComboxIndexChanged(int index)
{
    UserFaceType type = static_cast<UserFaceType>(index);
    if (type > UFACE_NR)
    {
        return;
    }
    layoutManager.setUFaceType(type);
}

void ScreenMenuContent::onBtnClick()
{
    Button *btn = static_cast<Button *>(sender());
    ScreenMenuContentPrivate::ButtonItem item =
            static_cast<ScreenMenuContentPrivate::ButtonItem>(btn->property("Item").toInt());
    switch (item)
    {
    case ScreenMenuContentPrivate::BUTTON_SCREEN_STANDARD_LAYOUT:
        windowManager.showWindow(ScreenLayoutWindow::getInstance(), WindowManager::ShowBehaviorCloseOthers);
        break;
    case ScreenMenuContentPrivate::BUTTON_SCREEN_BIG_FONT_LAYOUT:
        windowManager.showWindow(&bigFontLayoutWindow, WindowManager::ShowBehaviorCloseOthers);
        break;
    case ScreenMenuContentPrivate::BUTTON_PARA_COLOR:
        windowManager.showWindow(new ParaColorWindow, WindowManager::ShowBehaviorCloseIfVisiable);
        break;
    default:
        break;
    }
}
