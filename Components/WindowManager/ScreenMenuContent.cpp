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
#include "IConfig.h"
#include "ThemeManager.h"
#include "ECGParam.h"

class ScreenMenuContentPrivate
{
public:
    ScreenMenuContentPrivate()
        : interfaceCbo(NULL),
          layoutCbo(NULL),
          paraColorBtn(NULL)
    {}

    void loadOptions();

    void reloadScreenType();

    ComboBox *interfaceCbo;     // 界面选择
    ComboBox *layoutCbo;        // 布局设置
    Button *paraColorBtn;

    enum ScreenLayoutItem
    {
        SCREEN_LAYOUT_STANDARD = 1,
        SCREEN_LAYOUT_BIGFONT
    };
};

void ScreenMenuContentPrivate::loadOptions()
{
    layoutCbo->setCurrentIndex(0);
    reloadScreenType();
    int type = UFACE_MONITOR_STANDARD;
    systemConfig.getNumValue("UserFaceType", type);
    int index = 0;
    for (; index < interfaceCbo->count(); ++index)
    {
        if (interfaceCbo->itemData(index).toInt() == type)
        {
            type = index;
            break;
        }
    }
    interfaceCbo->setCurrentIndex(type);
}

void ScreenMenuContentPrivate::reloadScreenType()
{
    QStringList screenTypeTextList;
    QList<int> screenTypeList;
    if (ecgParam.getLeadMode() == ECG_LEAD_MODE_3)
    {
        screenTypeTextList<< trs("Standard")
                       << trs("OxyCRG")
                       << trs("Trend")
                       << trs("BigFont");
        screenTypeList << UFACE_MONITOR_STANDARD
                       << UFACE_MONITOR_OXYCRG
                       << UFACE_MONITOR_TREND
                       << UFACE_MONITOR_BIGFONT;
    }
    else
    {
        screenTypeTextList<< trs("Standard")
                       << trs("ECGFullScreen")
                       << trs("OxyCRG")
                       << trs("Trend")
                       << trs("BigFont");
        screenTypeList << UFACE_MONITOR_STANDARD
                       << UFACE_MONITOR_ECG_FULLSCREEN
                       << UFACE_MONITOR_OXYCRG
                       << UFACE_MONITOR_TREND
                       << UFACE_MONITOR_BIGFONT;
    }

    interfaceCbo->blockSignals(true);
    interfaceCbo->clear();
    for (int i = 0; i< screenTypeList.count(); ++i)
    {
        interfaceCbo->addItem(screenTypeTextList.at(i), screenTypeList.at(i));
    }
    interfaceCbo->blockSignals(false);
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
    layout->setMargin(10);
    layout->setHorizontalSpacing(0);

    QLabel *label;
    ComboBox *comboBox;
    int count = 0;

    // screen select
    label = new QLabel(trs("CurrentScreen"));
    layout->addWidget(label, count, 0);
    comboBox = new ComboBox();
    layout->addWidget(comboBox, count++, 1);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    d_ptr->interfaceCbo = comboBox;

    label = new QLabel(trs("ScreenLayout"));
    layout->addWidget(label, count, 0);
    d_ptr->layoutCbo = new ComboBox;
    d_ptr->layoutCbo->addItems(QStringList()
                               << QString()
                               << trs("StandardScreenLayout")
                               << trs("BigFontScreenLayout")
                               );
    layout->addWidget(d_ptr->layoutCbo, count++, 1);
    connect(d_ptr->layoutCbo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onComboxIndexChanged(int)));

    d_ptr->paraColorBtn = new Button(trs("ParameterColor"));
    d_ptr->paraColorBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->paraColorBtn, count++, 1);
    connect(d_ptr->paraColorBtn, SIGNAL(clicked()), this, SLOT(onBtnClick()));

    layout->setRowStretch(count, 1);
}

void ScreenMenuContent::onComboxIndexChanged(int index)
{
    ComboBox *cbo = qobject_cast<ComboBox *>(sender());
    if (cbo == d_ptr->interfaceCbo)
    {
        UserFaceType type = static_cast<UserFaceType>(d_ptr->interfaceCbo->itemData(index).toInt());
        if (type > UFACE_NR)
        {
            return;
        }
        if (type != UFACE_MONITOR_ECG_FULLSCREEN)
        {
            ecgParam.updateGain();
        }
        layoutManager.setUFaceType(type);
    }
    else if (cbo == d_ptr->layoutCbo)
    {
        if (index == ScreenMenuContentPrivate::SCREEN_LAYOUT_STANDARD)
        {
            windowManager.showWindow(ScreenLayoutWindow::getInstance(),
                                     WindowManager::ShowBehaviorNoAutoClose |
                                     WindowManager::ShowBehaviorCloseOthers);
        }
        else if (index == ScreenMenuContentPrivate::SCREEN_LAYOUT_BIGFONT)
        {
            windowManager.showWindow(BigFontLayoutWindow::getInstance(),
                                     WindowManager::ShowBehaviorNoAutoClose |
                                     WindowManager::ShowBehaviorCloseOthers);
        }
    }
}

void ScreenMenuContent::onBtnClick()
{
    windowManager.showWindow(new ParaColorWindow, WindowManager::ShowBehaviorCloseIfVisiable);
}
