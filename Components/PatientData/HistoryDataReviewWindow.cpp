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
#include "Framework/Language/LanguageManager.h"
#include "Framework/UI/Button.h"
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

class HistoryDataReviewWindowPrivate
{
public:
    enum ButtonItem
    {
        ITEM_TREND_TABLE,
        ITEM_TREND_GRAPH,
        ITEM_EVENT_REVIEW,
#ifndef HIDE_OXYCRG_REVIEW_FUNCTION
        ITEM_OXYCRGEVENT_REVIEW,
#endif
        ITEM_HISTORY_TIME
    };
    HistoryDataReviewWindowPrivate()
        : selHistoryDataPath(DATA_STORE_PATH)
    {}

    QMap<ButtonItem, Button *> buttons;
    QString selHistoryDataPath;
};

HistoryDataReviewWindow *HistoryDataReviewWindow::getInstance()
{
    static HistoryDataReviewWindow *instance = NULL;
    if (!instance)
    {
        instance = new HistoryDataReviewWindow;
    }
    return instance;
}

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
#ifndef  HIDE_OXYCRG_REVIEW_FUNCTION
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW]->setEnabled(true);
#endif
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_HISTORY_TIME]->setText(timeStr);
}

void HistoryDataReviewWindow::showEvent(QShowEvent *ev)
{
    Dialog::showEvent(ev);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_TREND_TABLE]->setEnabled(false);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_TREND_GRAPH]->setEnabled(false);
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_EVENT_REVIEW]->setEnabled(false);
#ifndef  HIDE_OXYCRG_REVIEW_FUNCTION
    d_ptr->buttons[HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW]->setEnabled(false);
#endif
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
            TrendTableWindow::getInstance()->setHistoryDataPath(d_ptr->selHistoryDataPath);
            TrendTableWindow::getInstance()->setHistoryData(true);
            windowManager.showWindow(TrendTableWindow::getInstance(), WindowManager::ShowBehaviorNone |
                                     WindowManager::ShowBehaviorNoAutoClose);
            break;
        case HistoryDataReviewWindowPrivate::ITEM_TREND_GRAPH:
            TrendGraphWindow::getInstance()->setHistoryDataPath(d_ptr->selHistoryDataPath);
            TrendGraphWindow::getInstance()->setHistoryData(true);
            windowManager.showWindow(TrendGraphWindow::getInstance(), WindowManager::ShowBehaviorNone |
                                     WindowManager::ShowBehaviorNoAutoClose);
            break;
        case HistoryDataReviewWindowPrivate::ITEM_EVENT_REVIEW:
            EventWindow::getInstance()->setHistoryDataPath(d_ptr->selHistoryDataPath);
            EventWindow::getInstance()->setHistoryData(true);
            windowManager.showWindow(EventWindow::getInstance(), WindowManager::ShowBehaviorNone);
            break;
#ifndef  HIDE_OXYCRG_REVIEW_FUNCTION
        case HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW:
            OxyCRGEventWindow::getInstance()->setHistoryDataPath(d_ptr->selHistoryDataPath);
            OxyCRGEventWindow::getInstance()->setHistoryData(true);
            windowManager.showWindow(OxyCRGEventWindow::getInstance(), WindowManager::ShowBehaviorNone);
            break;
#endif
        case HistoryDataReviewWindowPrivate::ITEM_HISTORY_TIME:
        {
            HistoryDataSelWindow *window = new HistoryDataSelWindow();
            windowManager.showWindow(window, WindowManager::ShowBehaviorNone);
            break;
        }
        default:
            break;
        }
    }
}

HistoryDataReviewWindow::HistoryDataReviewWindow()
    : Dialog(), d_ptr(new HistoryDataReviewWindowPrivate())
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

#ifndef  HIDE_OXYCRG_REVIEW_FUNCTION
    button = new Button(trs("OxyCRGEventReview"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, 1);
    d_ptr->buttons.insert(HistoryDataReviewWindowPrivate::ITEM_OXYCRGEVENT_REVIEW, button);
#endif
    setWindowLayout(layout);

    setFixedSize(400, 400);
}
