/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/5
 **/

#include "DataReviewMenuContent.h"
#include "LanguageManager.h"
#include <QMap>
#include <QGridLayout>
#include "Button.h"
#include <QVariant>
#include "TrendTableWindow.h"
#include "WindowManager.h"
#include "TrendGraphWindow.h"
#include "EventWindow.h"
#include "OxyCRGEventWindow.h"
#include "HistoryDataReviewWindow.h"

class DataReviewMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_TREND_TABLE = 0,
        ITEM_BTN_TREND_GRAPH,
        ITEM_BTN_EVENT_REVIEW,
        ITEM_BTN_OXYCRG_REVIEW,
        ITEM_BTN_HISTORY_REVIEW,
    };

    QMap <MenuItem, Button*> btns;
};

DataReviewMenuContent::DataReviewMenuContent()
                     : MenuContent(trs("DataReviewMenu"),
                                   trs("DataReviewMenuDesc")),
                       d_ptr(new DataReviewMenuContentPrivate)
{
}

DataReviewMenuContent::~DataReviewMenuContent()
{
    delete d_ptr;
}

void DataReviewMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);
    glayout->setVerticalSpacing(20);
    glayout->setHorizontalSpacing(20);

    Button *btn;
    int lastHeight = 50;
    int row = 0;
    int item = 0;

    glayout->setRowMinimumHeight(row, lastHeight);
    row++;

    // trend table
    btn = new Button(trs("TrendTable"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(item));
    item++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    glayout->addWidget(btn, row, 0);
    d_ptr->btns.insert(DataReviewMenuContentPrivate
                       ::ITEM_BTN_TREND_TABLE, btn);

    // trend graph
    btn = new Button(trs("TrendGraph"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(item));
    item++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    glayout->addWidget(btn, row, 2);
    d_ptr->btns.insert(DataReviewMenuContentPrivate
                       ::ITEM_BTN_TREND_GRAPH, btn);

    row++;

    // event review
    btn = new Button(trs("EventReview"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(item));
    item++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    glayout->addWidget(btn, row, 0);
    d_ptr->btns.insert(DataReviewMenuContentPrivate
                       ::ITEM_BTN_EVENT_REVIEW, btn);

    // oxycrg review
    btn = new Button(trs("OxyCRGEventReview"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(item));
    item++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    glayout->addWidget(btn, row, 2);
    d_ptr->btns.insert(DataReviewMenuContentPrivate
                       ::ITEM_BTN_OXYCRG_REVIEW, btn);

    row++;

    //  history  review
    btn = new Button(trs("HistoryTrend"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(item));
    item++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    glayout->addWidget(btn, row, 0);
    d_ptr->btns.insert(DataReviewMenuContentPrivate
                       ::ITEM_BTN_HISTORY_REVIEW, btn);

    row++;

    glayout->setRowMinimumHeight(row, lastHeight);
}

void DataReviewMenuContent::onBtnReleased()
{
    Button *btn = qobject_cast<Button*>(sender());
    if (!btn)
    {
        return;
    }
    int index = btn->property("Item").toInt();

    switch (index)
    {
        case DataReviewMenuContentPrivate::ITEM_BTN_TREND_TABLE:
        {
            TrendTableWindow::getInstance()->setHistoryData(false);
            windowManager.showWindow(TrendTableWindow::getInstance(),
                                     WindowManager::
                                     ShowBehaviorCloseIfVisiable |
                                     WindowManager::
                                     ShowBehaviorCloseOthers);
        }
        break;

        case DataReviewMenuContentPrivate::ITEM_BTN_TREND_GRAPH:
        {
            TrendGraphWindow::getInstance()->setHistoryData(false);
            windowManager.showWindow(TrendGraphWindow::getInstance(),
                                     WindowManager::
                                     ShowBehaviorCloseIfVisiable |
                                     WindowManager::
                                     ShowBehaviorCloseOthers);
        }
        break;

        case DataReviewMenuContentPrivate::ITEM_BTN_EVENT_REVIEW:
        {
            EventWindow::getInstance()->setHistoryData(false);
            windowManager.showWindow(EventWindow::getInstance(),
                                     WindowManager::
                                     ShowBehaviorCloseIfVisiable |
                                     WindowManager::
                                     ShowBehaviorCloseOthers);
        }
        break;

        case DataReviewMenuContentPrivate::ITEM_BTN_OXYCRG_REVIEW:
        {
            OxyCRGEventWindow::getInstance()->setHistoryData(false);
            windowManager.showWindow(OxyCRGEventWindow::getInstance(),
                                     WindowManager::
                                     ShowBehaviorCloseIfVisiable |
                                     WindowManager::
                                     ShowBehaviorCloseOthers);
        }
        break;

        case DataReviewMenuContentPrivate::ITEM_BTN_HISTORY_REVIEW:
        {
            windowManager.showWindow(HistoryDataReviewWindow::getInstance(),
                                     WindowManager::
                                     ShowBehaviorCloseIfVisiable |
                                     WindowManager::
                                     ShowBehaviorCloseOthers);
        }
        break;
    }
}
