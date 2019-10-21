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
#include "ThemeManager.h"

#define STOP_PRINT_TIMEOUT          (100)

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
        cursorMoveBtn(NULL), eventMoveBtn(NULL),
        printTimerId(-1),
        waitTimerId(-1),
        isWait(false),
        timeoutNum(0),
        generator(NULL)
    {}

    TrendWaveWidget *waveWidget;

    QMap<KeyAction, Button *> buttons;
    MoveButton *coordinateMoveBtn;
    MoveButton *cursorMoveBtn;
    MoveButton *eventMoveBtn;
    int printTimerId;
    int waitTimerId;
    bool isWait;
    int timeoutNum;
    RecordPageGenerator *generator;
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

void TrendGraphWindow::setSubWidgetRulerLimit(int index, int down, int up, int scale)
{
    d_ptr->waveWidget->setRulerLimit(index, down, up, scale);
}

void TrendGraphWindow::setAllParamAutoRuler()
{
    if (d_ptr->waveWidget)
    {
        d_ptr->waveWidget->setAllParamAutoRuler();
    }
}

void TrendGraphWindow::updateTrendGraph()
{
    d_ptr->waveWidget->trendWaveReset();
    d_ptr->waveWidget->updateTimeRange();
}

void TrendGraphWindow::timeIntervalChange(int timeInterval)
{
    d_ptr->waveWidget->setTimeInterval((ResolutionRatio)timeInterval);
}

void TrendGraphWindow::waveNumberChange(int num)
{
    if (num > 0 && num <= 3)
    {
        d_ptr->waveWidget->setWaveNumber(num);
        trendGraphSetWindow.setCurParam(d_ptr->waveWidget->getCurParamList());
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

void TrendGraphWindow::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->printTimerId == ev->timerId())
    {
        if (!recorderManager.isPrinting() || d_ptr->timeoutNum == 10) // 1000ms超时处理
        {
            if (!recorderManager.isPrinting() && !recorderManager.getPrintStatus())
            {
                recorderManager.addPageGenerator(d_ptr->generator);
            }
            else
            {
                d_ptr->generator->deleteLater();
                d_ptr->generator = NULL;
            }
            killTimer(d_ptr->printTimerId);
            d_ptr->printTimerId = -1;
            d_ptr->timeoutNum = 0;
        }
        d_ptr->timeoutNum++;
    }
    else if (d_ptr->waitTimerId == ev->timerId())
    {
        d_ptr->printTimerId = startTimer(STOP_PRINT_TIMEOUT);
        killTimer(d_ptr->waitTimerId);
        d_ptr->waitTimerId = -1;
        d_ptr->isWait = false;
    }
}

void TrendGraphWindow::showEvent(QShowEvent *ev)
{
    Dialog::showEvent(ev);
    // 更新打印按键状态
    if (recorderManager.isConnected())
    {
        d_ptr->buttons[TrendGraphWindowPrivate::ACTION_BTN_PRINT]->setEnabled(true);
    }
    else
    {
        d_ptr->buttons[TrendGraphWindowPrivate::ACTION_BTN_PRINT]->setEnabled(false);
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
            QList<BlockEntry> eventList = d_ptr->waveWidget->getEventList();
            PatientInfo patientInfo = d_ptr->waveWidget->getPatientInfo();
            RecordPageGenerator *pageGenerator = new TrendGraphPageGenerator(trendGraphList, eventList, patientInfo);

            if (recorderManager.isPrinting() && !d_ptr->isWait)
            {
                if (pageGenerator->getPriority() <= recorderManager.getCurPrintPriority())
                {
                    pageGenerator->deleteLater();
                }
                else
                {
                    recorderManager.stopPrint();
                    d_ptr->generator = pageGenerator;
                    d_ptr->waitTimerId = startTimer(2000); // 等待2000ms
                    d_ptr->isWait = true;
                }
            }
            else if (!recorderManager.getPrintStatus())
            {
                recorderManager.addPageGenerator(pageGenerator);
            }
            else
            {
                pageGenerator->deleteLater();
            }
            break;
        }
        case TrendGraphWindowPrivate::ACTION_BTN_SET_WIDGET:
        {
            windowManager.showWindow(&trendGraphSetWindow, WindowManager::ShowBehaviorHideOthers | WindowManager::ShowBehaviorNoAutoClose);
            trendGraphSetWindow.setCurParam(d_ptr->waveWidget->getCurParamList());
            break;
        }
        case TrendGraphWindowPrivate::ACTION_BTN_UP_PAGE:
        {
            d_ptr->waveWidget->pageUpParam();
            break;
        }
        case TrendGraphWindowPrivate::ACTION_BTN_DOWN_PAGE:
        {
            d_ptr->waveWidget->pageDownParam();
            break;
        }
        default:
            break;
        }
    }
}

TrendGraphWindow::TrendGraphWindow()
    : Dialog(), d_ptr(new TrendGraphWindowPrivate())
{
    setWindowTitle(trs("TrendGraph"));
    int maxWidth = windowManager.getPopWindowWidth();
    int maxHeight = windowManager.getPopWindowHeight();
    setFixedSize(maxWidth, maxHeight);
    int margin = 5;
    int spacing = 5;
    int itemHeight = themeManger.getAcceptableControlHeight();

    d_ptr->waveWidget = new TrendWaveWidget();
    int margins = contentsMargins().left() * 2 + margin * 2;
    d_ptr->waveWidget->setWidgetSize(maxWidth - margins,
                                     maxHeight - spacing - margins - getTitleHeight() - itemHeight);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(spacing);
    hLayout->setMargin(0);
    Button *button;
    int buttonID;

    d_ptr->coordinateMoveBtn = new MoveButton(trs("Time"));
    d_ptr->coordinateMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->coordinateMoveBtn->setFixedHeight(itemHeight);
    connect(d_ptr->coordinateMoveBtn, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveCoordinate()));
    connect(d_ptr->coordinateMoveBtn, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveCoordinate()));
    hLayout->addWidget(d_ptr->coordinateMoveBtn, 2);

    d_ptr->cursorMoveBtn = new MoveButton(trs("Scroll"));
    d_ptr->cursorMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->cursorMoveBtn->setFixedHeight(itemHeight);
    connect(d_ptr->cursorMoveBtn, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveCursor()));
    connect(d_ptr->cursorMoveBtn, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveCursor()));
    hLayout->addWidget(d_ptr->cursorMoveBtn, 2);

    d_ptr->eventMoveBtn = new MoveButton(trs("Event"));
    d_ptr->eventMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->eventMoveBtn->setFixedHeight(itemHeight);
    connect(d_ptr->eventMoveBtn, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveEvent()));
    connect(d_ptr->eventMoveBtn, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveEvent()));
    hLayout->addWidget(d_ptr->eventMoveBtn, 2);

    // 打印
    button = new Button(trs("Print"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setFixedHeight(itemHeight);
    buttonID = TrendGraphWindowPrivate::ACTION_BTN_PRINT;
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 2);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_PRINT, button);

    // 设置窗口
    button = new Button(trs("Set"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setFixedHeight(itemHeight);
    buttonID = TrendGraphWindowPrivate::ACTION_BTN_SET_WIDGET;
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 2);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_SET_WIDGET, button);

    // 上翻页坐标
    QIcon ico = themeManger.getIcon(ThemeManager::IconUp, QSize(32, 32));
    button = new Button("", ico);
    button->setIconSize(QSize(32, 32));
    button->setButtonStyle(Button::ButtonIconOnly);
    button->setFixedHeight(itemHeight);
    buttonID = TrendGraphWindowPrivate::ACTION_BTN_UP_PAGE;
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_UP_PAGE, button);

    // 下翻页坐标
    button = new Button();
    ico = themeManger.getIcon(ThemeManager::IconDown, QSize(32, 32));
    button->setIcon(ico);
    button->setIconSize(QSize(32, 32));
    button->setButtonStyle(Button::ButtonIconOnly);
    button->setFixedHeight(itemHeight);
    buttonID = TrendGraphWindowPrivate::ACTION_BTN_DOWN_PAGE;
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_DOWN_PAGE, button);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(margin);
    layout->setSpacing(spacing);
    layout->addWidget(d_ptr->waveWidget);
    layout->addLayout(hLayout);

    setWindowLayout(layout);
}
