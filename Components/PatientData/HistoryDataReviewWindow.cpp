/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/7
 **/

#include "HistoryDataReviewWindow.h"
#include "LanguageManager.h"
#include "Button.h"
#include <QVBoxLayout>
#include <QVariant>
#include "DataStorageDirManager.h"
#include "TrendTableWindow.h"
#include "TrendGraphWindow.h"
#include "EventWindow.h"
#include "OxyCRGEventWindow.h"
#include "WindowManager.h"
#include "HistoryDataSelWindow.h"
#include "WindowManager.h"

HistoryDataReviewWindow *HistoryDataReviewWindow::selfObj = NULL;

class HistoryDataReviewWindowPrivate
{
public:
    enum ButtonItem
    {
        ITEM_TREND_TABLE,
        ITEM_TREND_GRAPH,
        ITEM_EVENT_REVIEW,
        ITEM_OXYCRGEVENT_REVIEW,
        ITEM_HISTORY_TIME
    };
    HistoryDataReviewWindowPrivate()
        : selHistoryDataPath(DATA_STORE_PATH)
    {}

    QMap<ButtonItem, Button *> buttons;
    QString selHistoryDataPath;
};

HistoryDataReviewWindow::~HistoryDataReviewWindow()
{
}

void HistoryDataReviewWindow::setHistoryReviewIndex(int index, QString timeStr)
{
    d_ptr->selHistoryDataPath = DATA_STORE_PATH;
    d_ptr->selHistoryDataPath += dataStorageDirManager.getRescueDataDir(index);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_TREND_TABLE]->setEnabled(true);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_TREND_GRAPH]->setEnabled(true);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_EVENT_REVIEW]->setEnabled(true);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW]->setEnabled(true);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_HISTORY_TIME]->setText(timeStr);
}

void HistoryDataReviewWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_TREND_TABLE]->setEnabled(false);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_TREND_GRAPH]->setEnabled(false);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_EVENT_REVIEW]->setEnabled(false);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW]->setEnabled(false);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_HISTORY_TIME]->setText(trs("SelectHistoryData"));
}

void HistoryDataReviewWindow::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        HistoryDataReviewWindowPrivate::ButtonItem item
                = (HistoryDataReviewWindowPrivate::ButtonItem)button->property("Item").toInt();
        switch (item)
        {
        case HistoryDataReviewWindowPrivate::ITEM_TREND_TABLE:
            trendTableWindow.setHistoryDataPath(d_ptr->selHistoryDataPath);
            trendTableWindow.setHistoryData(true);
            windowManager.showWindow(&trendTableWindow);
            break;
        case HistoryDataReviewWindowPrivate::ITEM_TREND_GRAPH:
            trendGraphWindow.setHistoryDataPath(d_ptr->selHistoryDataPath);
            trendGraphWindow.setHistoryData(true);
            windowManager.showWindow(&trendGraphWindow);
            break;
        case HistoryDataReviewWindowPrivate::ITEM_EVENT_REVIEW:
            eventWindow.setHistoryDataPath(d_ptr->selHistoryDataPath);
            eventWindow.setHistoryData(true);
            windowManager.showWindow(&eventWindow);
            break;
        case HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW:
            oxyCRGEventWindow.setHistoryDataPath(d_ptr->selHistoryDataPath);
            oxyCRGEventWindow.setHistoryData(true);
            windowManager.showWindow(&oxyCRGEventWindow);
            break;
        case HistoryDataReviewWindowPrivate::ITEM_HISTORY_TIME:
        {
            HistoryDataSelWindow *window = new HistoryDataSelWindow();
            windowManager.showWindow(window);
            break;
        }
        default:
            break;
        }
    }
}

HistoryDataReviewWindow::HistoryDataReviewWindow()
    : Window(), d_ptr(new HistoryDataReviewWindowPrivate())
{
    setWindowTitle(trs("HistoryTrend"));

    QVBoxLayout *layout = new QVBoxLayout();
    Button *button;
    int itemID;


    button = new Button(trs("SelectHistoryData"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(HistoryDataReviewWindowPrivate::ITEM_HISTORY_TIME);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, 1);
    d_ptr->buttons.insert(HistoryDataReviewWindowPrivate::ITEM_HISTORY_TIME, button);

    layout->addStretch(3);

    button = new Button(trs("TrendTable"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(HistoryDataReviewWindowPrivate::ITEM_TREND_TABLE);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, 1);
    d_ptr->buttons.insert(HistoryDataReviewWindowPrivate::ITEM_TREND_TABLE, button);

    button = new Button(trs("TrendGraph"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(HistoryDataReviewWindowPrivate::ITEM_TREND_GRAPH);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, 1);
    d_ptr->buttons.insert(HistoryDataReviewWindowPrivate::ITEM_TREND_GRAPH, button);

    button = new Button(trs("EventReview"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(HistoryDataReviewWindowPrivate::ITEM_EVENT_REVIEW);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, 1);
    d_ptr->buttons.insert(HistoryDataReviewWindowPrivate::ITEM_EVENT_REVIEW, button);

    button = new Button(trs("OxyCRGEventReview"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, 1);
    d_ptr->buttons.insert(HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW, button);

    setWindowLayout(layout);

    setFixedSize(400, 400);
}
