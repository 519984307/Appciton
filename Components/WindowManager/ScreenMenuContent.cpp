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
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include <ScreenLayoutWindow.h>
#include <WindowManager.h>
#include "ParaColorWindow.h"
#include "LayoutManager.h"
#include "BigFontLayoutWindow.h"
#include "IConfig.h"
#include "ECGParam.h"

class ScreenMenuContentPrivate
{
public:
    ScreenMenuContentPrivate()
        : interfaceCbo(NULL),
#ifndef HIDE_SCREEN_LAYOUT
          layoutCbo(NULL),
#endif
          paraColorBtn(NULL)
    {
        spo2Enable = systemManager.isSupport(CONFIG_SPO2) &&
                     systemManager.isSupport(CONFIG_SPO2_HIGH_CONFIGURE);
    }

    void loadOptions();

    void reloadScreenType();

    ComboBox *interfaceCbo;     // 界面选择
#ifndef HIDE_SCREEN_LAYOUT
    ComboBox *layoutCbo;        // 布局设置
#endif
    Button *paraColorBtn;
    bool spo2Enable;

    enum ScreenLayoutItem
    {
        SCREEN_LAYOUT_STANDARD = 0,
        SCREEN_LAYOUT_BIGFONT
    };
};

void ScreenMenuContentPrivate::loadOptions()
{
    reloadScreenType();
}

void ScreenMenuContentPrivate::reloadScreenType()
{
    QStringList screenTypeTextList;
    QList<int> screenTypeList;
    screenTypeTextList.append(trs(SystemSymbol::convert(UFACE_MONITOR_STANDARD)));
    screenTypeList.append(UFACE_MONITOR_STANDARD);
    if (ecgParam.getLeadMode() != ECG_LEAD_MODE_3)
    {
        screenTypeTextList.append(trs(SystemSymbol::convert(UFACE_MONITOR_ECG_FULLSCREEN)));
        screenTypeList.append(UFACE_MONITOR_ECG_FULLSCREEN);
    }
#ifndef HIDE_MONITOR_OXYCRG
    screenTypeTextList.append(trs(SystemSymbol::convert(UFACE_MONITOR_OXYCRG)));
    screenTypeList.append(UFACE_MONITOR_OXYCRG);
#endif
#ifndef HIDE_MONITOR_TREND
    screenTypeTextList.append(trs(SystemSymbol::convert(UFACE_MONITOR_TREND)));
    screenTypeList.append(UFACE_MONITOR_TREND);
#endif
    screenTypeTextList.append(trs(SystemSymbol::convert(UFACE_MONITOR_BIGFONT)));
    screenTypeList.append(UFACE_MONITOR_BIGFONT);
    if (spo2Enable)
    {
        screenTypeTextList.append(trs(SystemSymbol::convert(UFACE_MONITOR_SPO2)));
        screenTypeList.append(UFACE_MONITOR_SPO2);
    }

    interfaceCbo->blockSignals(true);
    interfaceCbo->clear();
    int index = 0;
    int type = UFACE_MONITOR_STANDARD;
    systemConfig.getNumValue("UserFaceType", type);
    UserFaceType faceType = static_cast<UserFaceType>(type);
    for (int i = 0; i< screenTypeList.count(); ++i)
    {
        interfaceCbo->addItem(screenTypeTextList.at(i), screenTypeList.at(i));
        if (trs(SystemSymbol::convert(faceType)) == screenTypeTextList.at(i))
        {
            // 获取当前的索引
            index =  interfaceCbo->count() - 1;
        }
    }

    interfaceCbo->blockSignals(false);
    interfaceCbo->setCurrentIndex(index);

#ifndef HIDE_SCREEN_LAYOUT
    layoutCbo->blockSignals(true);
    if (type == UFACE_MONITOR_BIGFONT)
    {
        layoutCbo->setCurrentIndex(SCREEN_LAYOUT_BIGFONT);
    }
    else
    {
        layoutCbo->setCurrentIndex(SCREEN_LAYOUT_STANDARD);
    }
    layoutCbo->blockSignals(false);
#endif
}

ScreenMenuContent::ScreenMenuContent()
    : MenuContent(trs("ScreenSetting"), trs("ScreenSettingDesc")),
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

#ifndef HIDE_SCREEN_LAYOUT
    label = new QLabel(trs("ScreenLayout"));
    layout->addWidget(label, count, 0);
    d_ptr->layoutCbo = new ComboBox;
    d_ptr->layoutCbo->addItems(QStringList()
                               << trs("StandardScreenLayout")
                               << trs("BigFontScreenLayout"));
    layout->addWidget(d_ptr->layoutCbo, count++, 1);
    connect(d_ptr->layoutCbo, SIGNAL(activated(int)), this, SLOT(onComboxIndexChanged(int)));
#endif

    d_ptr->paraColorBtn = new Button(trs("ParameterColor"));
    d_ptr->paraColorBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->paraColorBtn, count++, 1);
    connect(d_ptr->paraColorBtn, SIGNAL(clicked()), this, SLOT(onBtnClick()));

    layout->setRowStretch(count, 1);
}

void ScreenMenuContent::hideEvent(QHideEvent *ev)
{
#ifndef HIDE_SCREEN_LAYOUT
    d_ptr->layoutCbo->blockSignals(true);
#endif
    MenuContent::hideEvent(ev);
}

void ScreenMenuContent::onComboxIndexChanged(int index)
{
#ifdef HIDE_SCREEN_LAYOUT
    Q_UNUSED(index)
#endif
    ComboBox *cbo = qobject_cast<ComboBox *>(sender());
    if (cbo == d_ptr->interfaceCbo)
    {
        // 通过比较类型字符串查找匹配界面
        UserFaceType type = UFACE_MONITOR_STANDARD;
        QString text = d_ptr->interfaceCbo->currentText();
        for (int i = UFACE_MONITOR_STANDARD; i < UFACE_NR; i++)
        {
            UserFaceType faceType = static_cast<UserFaceType>(i);
            if (trs(SystemSymbol::convert(faceType)) == text)
            {
                type = faceType;
                break;
            }
        }
        if (type >= UFACE_NR)
        {
            return;
        }
        layoutManager.setUFaceType(type);
#ifndef HIDE_SCREEN_LAYOUT
        d_ptr->layoutCbo->blockSignals(true);
        if (type == UFACE_MONITOR_BIGFONT)
        {
            d_ptr->layoutCbo->setCurrentIndex(ScreenMenuContentPrivate::SCREEN_LAYOUT_BIGFONT);
        }
        else
        {
            d_ptr->layoutCbo->setCurrentIndex(ScreenMenuContentPrivate::SCREEN_LAYOUT_STANDARD);
        }
        d_ptr->layoutCbo->blockSignals(false);
#endif
    }
#ifndef HIDE_SCREEN_LAYOUT
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
#endif
}

void ScreenMenuContent::onBtnClick()
{
    windowManager.showWindow(new ParaColorWindow, WindowManager::ShowBehaviorCloseIfVisiable);
}
