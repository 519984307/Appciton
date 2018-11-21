/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/27
 **/

#include "TrendGraphWindow.h"
#include "LanguageManager.h"
#include "Button.h"
#include "TrendWaveWidget.h"
#include <QVariant>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "WindowManager.h"
#include "TrendGraphPageGenerator.h"
#include "RecorderManager.h"
#include "TrendGraphSetWindow.h"
#include "MoveButton.h"

class TrendGraphWindowPrivate
{
public:
    enum KeyAction
    {
        ACTION_BTN_PRINT,
        ACTION_BTN_SET_WIDGET,
        ACTION_BTN_UP_PAGE,
        ACTION_BTN_DOWN_PAGE
    };
    TrendGraphWindowPrivate() : waveWidget(NULL), coordinateMoveBtn(NULL),
        cursorMoveBtn(NULL), eventMoveBtn(NULL)
    {}

    void checkPageEnabled();

    TrendWaveWidget *waveWidget;

    QMap<KeyAction, Button *> buttons;
    MoveButton *coordinateMoveBtn;
    MoveButton *cursorMoveBtn;
    MoveButton *eventMoveBtn;
};
TrendGraphWindow *TrendGraphWindow::getInstance()
{
    static TrendGraphWindow *instance = NULL;
    if (!instance)
    {
        instance = new TrendGraphWindow;
    }
    return instance;
}

TrendGraphWindow::~TrendGraphWindow()
{
}

void TrendGraphWindow::setSubWidgetRulerLimit(SubParamID id, int down, int up, int scale)
{
    d_ptr->waveWidget->setRulerLimit(id, down, up, scale);
}

void TrendGraphWindow::setSubWidgetAutoRuler(SubParamID id, bool isAuto)
{
    d_ptr->waveWidget->setAutoRuler(id, isAuto);
}

void TrendGraphWindow::updateTrendGraph()
{
    d_ptr->waveWidget->trendWaveReset();
    d_ptr->waveWidget->updateTimeRange();
    d_ptr->checkPageEnabled();
}

void TrendGraphWindow::timeIntervalChange(int timeInterval)
{
    d_ptr->waveWidget->setTimeInterval((ResolutionRatio)timeInterval);
    d_ptr->checkPageEnabled();
}

void TrendGraphWindow::waveNumberChange(int num)
{
    if (num > 0 && num <= 3)
    {
        d_ptr->waveWidget->setWaveNumber(num);
    }
}

void TrendGraphWindow::setHistoryDataPath(QString path)
{
    if (d_ptr->waveWidget)
    {
        d_ptr->waveWidget->setHistoryDataPath(path);
    }
}

void TrendGraphWindow::setHistoryData(bool flag)
{
    if (d_ptr->waveWidget)
    {
        d_ptr->waveWidget->setHistoryData(flag);
    }
}

void TrendGraphWindow::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        TrendGraphWindowPrivate::KeyAction action
                 = (TrendGraphWindowPrivate::KeyAction)button->property("Button").toInt();
        switch (action)
        {
        case TrendGraphWindowPrivate::ACTION_BTN_PRINT:
        {
            QList<TrendGraphInfo>  trendGraphList = d_ptr->waveWidget->getTrendGraphPrint();
            QList<unsigned> eventTimeList = d_ptr->waveWidget->getEventList();
            RecordPageGenerator *pageGenerator = new TrendGraphPageGenerator(trendGraphList, eventTimeList);

            recorderManager.addPageGenerator(pageGenerator);
            break;
        }
        case TrendGraphWindowPrivate::ACTION_BTN_SET_WIDGET:
        {
            windowManager.showWindow(&trendGraphSetWindow, WindowManager::ShowBehaviorHideOthers);
            break;
        }
        case TrendGraphWindowPrivate::ACTION_BTN_UP_PAGE:
        {
            d_ptr->waveWidget->pageUpParam();
            d_ptr->checkPageEnabled();
            break;
        }
        case TrendGraphWindowPrivate::ACTION_BTN_DOWN_PAGE:
        {
            d_ptr->waveWidget->pageDownParam();
            d_ptr->checkPageEnabled();
            break;
        }
        default:
            break;
        }
    }
}

TrendGraphWindow::TrendGraphWindow()
    : Window(), d_ptr(new TrendGraphWindowPrivate())
{
    setWindowTitle(trs("TrendGraph"));
    int maxWidth = 800;
    int maxHeight = 560;
    resize(maxWidth, maxHeight);

    d_ptr->waveWidget = new TrendWaveWidget();
    d_ptr->waveWidget->setWidgetSize(maxWidth, maxHeight - 90);

    QHBoxLayout *hLayout = new QHBoxLayout();
    Button *button;
    int buttonID;

    d_ptr->coordinateMoveBtn = new MoveButton(trs("Time"));
    d_ptr->coordinateMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->coordinateMoveBtn, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveCoordinate()));
    connect(d_ptr->coordinateMoveBtn, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveCoordinate()));
    hLayout->addWidget(d_ptr->coordinateMoveBtn, 2);

    d_ptr->cursorMoveBtn = new MoveButton(trs("Scroll"));
    d_ptr->cursorMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->cursorMoveBtn, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveCursor()));
    connect(d_ptr->cursorMoveBtn, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveCursor()));
    hLayout->addWidget(d_ptr->cursorMoveBtn, 2);

    d_ptr->eventMoveBtn = new MoveButton(trs("Event"));
    d_ptr->eventMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->eventMoveBtn, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveEvent()));
    connect(d_ptr->eventMoveBtn, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveEvent()));
    hLayout->addWidget(d_ptr->eventMoveBtn, 2);

    // 打印
    button = new Button(trs("Print"));
    button->setButtonStyle(Button::ButtonTextOnly);
    buttonID = TrendGraphWindowPrivate::ACTION_BTN_PRINT;
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 2);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_PRINT, button);

    // 设置窗口
    button = new Button(trs("Set"));
    button->setButtonStyle(Button::ButtonTextOnly);
    buttonID = TrendGraphWindowPrivate::ACTION_BTN_SET_WIDGET;
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 2);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_SET_WIDGET, button);

    // 上翻页坐标
    button = new Button();
    button->setIcon(QIcon("/usr/local/nPM/icons/up.png"));
    button->setButtonStyle(Button::ButtonIconOnly);
    buttonID = TrendGraphWindowPrivate::ACTION_BTN_UP_PAGE;
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_UP_PAGE, button);

    // 下翻页坐标
    button = new Button();
    button->setIcon(QIcon("/usr/local/nPM/icons/down.png"));
    button->setButtonStyle(Button::ButtonIconOnly);
    buttonID = TrendGraphWindowPrivate::ACTION_BTN_DOWN_PAGE;
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_DOWN_PAGE, button);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->waveWidget);
    layout->addLayout(hLayout);

    setWindowLayout(layout);
}

void TrendGraphWindowPrivate::checkPageEnabled()
{
    bool hasUpBtn = waveWidget->hasUpPage();
    buttons[ACTION_BTN_UP_PAGE]->setEnabled(hasUpBtn);

    bool hasDownBtn = waveWidget->hasDownPage();
    buttons[ACTION_BTN_DOWN_PAGE]->setEnabled(hasDownBtn);

    if (!hasUpBtn)
    {
        buttons[ACTION_BTN_DOWN_PAGE]->setFocus();
    }
    if (!hasDownBtn)
    {
        buttons[ACTION_BTN_UP_PAGE]->setFocus();
    }
}
