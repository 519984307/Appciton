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

TrendGraphWindow *TrendGraphWindow::selfObj = NULL;

class TrendGraphWindowPrivate
{
public:
    enum KeyAction
    {
        ACTION_BTN_LEFT_COORDINATE,
        ACTION_BTN_RIGHT_COORDINATE,
        ACTION_BTN_LEFT_CURSOR,
        ACTION_BTN_RIGHT_CURSOR,
        ACTION_BTN_PREVIOUS_EVENT,
        ACTION_BTN_NEXT_EVENT,
        ACTION_BTN_PRINT,
        ACTION_BTN_SET_WIDGET,
        ACTION_BTN_UP_PAGE,
        ACTION_BTN_DOWN_PAGE
    };
    TrendGraphWindowPrivate() : waveWidget(NULL)
    {}

    TrendWaveWidget *waveWidget;

    QMap<KeyAction, Button *> buttons;
};
TrendGraphWindow::~TrendGraphWindow()
{
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
            RecordPageGenerator *pageGenerator = new TrendGraphPageGenerator(trendGraphList);

            recorderManager.addPageGenerator(pageGenerator);
            break;
        }
        case TrendGraphWindowPrivate::ACTION_BTN_SET_WIDGET:
        {
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
    : Window(), d_ptr(new TrendGraphWindowPrivate())
{
    setWindowTitle(trs("TrendGraph"));
    int maxWidth = windowManager.getPopMenuWidth();
    int maxHeight = windowManager.getPopMenuHeight();
    resize(maxWidth, maxHeight);

    d_ptr->waveWidget = new TrendWaveWidget();
    d_ptr->waveWidget->setWidgetSize(maxWidth, maxHeight - 90);

    QHBoxLayout *hLayout = new QHBoxLayout();
    Button *button;
    int buttonID;

    // 左移坐标
    button = new Button();
    button->setIcon(QIcon("/usr/local/nPM/icons/doubleleft.png"));
    button->setButtonStyle(Button::ButtonIconOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_LEFT_COORDINATE);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), d_ptr->waveWidget, SLOT(leftMoveCoordinate()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_LEFT_COORDINATE, button);

    // 左移游标
    button = new Button();
    button->setIcon(QIcon("/usr/local/nPM/icons/left.png"));
    button->setButtonStyle(Button::ButtonIconOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_LEFT_CURSOR);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), d_ptr->waveWidget, SLOT(leftMoveCursor()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_LEFT_CURSOR, button);

    // 右移游标
    button = new Button();
    button->setIcon(QIcon("/usr/local/nPM/icons/right.png"));
    button->setButtonStyle(Button::ButtonIconOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_RIGHT_CURSOR);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), d_ptr->waveWidget, SLOT(rightMoveCursor()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_RIGHT_CURSOR, button);

    // 右移坐标
    button = new Button();
    button->setIcon(QIcon("/usr/local/nPM/icons/doubleright.png"));
    button->setButtonStyle(Button::ButtonIconOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_RIGHT_COORDINATE);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), d_ptr->waveWidget, SLOT(rightMoveCoordinate()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_RIGHT_COORDINATE, button);

    // 上个事件
    button = new Button(trs("PreviousEvent"));
    button->setButtonStyle(Button::ButtonTextOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_PREVIOUS_EVENT);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), d_ptr->waveWidget, SLOT(leftMoveEvent()));
    hLayout->addWidget(button, 2);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_PREVIOUS_EVENT, button);

    // 下个事件
    button = new Button(trs("NextEvent"));
    button->setButtonStyle(Button::ButtonTextOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_NEXT_EVENT);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), d_ptr->waveWidget, SLOT(rightMoveEvent()));
    hLayout->addWidget(button, 2);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_NEXT_EVENT, button);

    // 打印
    button = new Button(trs("Print"));
    button->setButtonStyle(Button::ButtonTextOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_PRINT);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 2);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_PRINT, button);

    // 设置窗口
    button = new Button(trs("Set"));
    button->setButtonStyle(Button::ButtonTextOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_SET_WIDGET);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 2);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_SET_WIDGET, button);

    // 上翻页坐标
    button = new Button();
    button->setIcon(QIcon("/usr/local/nPM/icons/up.png"));
    button->setButtonStyle(Button::ButtonIconOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_UP_PAGE);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_UP_PAGE, button);

    // 下翻页坐标
    button = new Button();
    button->setIcon(QIcon("/usr/local/nPM/icons/down.png"));
    button->setButtonStyle(Button::ButtonIconOnly);
    buttonID = static_cast<int>(TrendGraphWindowPrivate::ACTION_BTN_DOWN_PAGE);
    button->setProperty("Button", qVariantFromValue(buttonID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(button, 1);
    d_ptr->buttons.insert(TrendGraphWindowPrivate::ACTION_BTN_DOWN_PAGE, button);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->waveWidget);
    layout->addLayout(hLayout);

    setWindowLayout(layout);
}
