#include "HistoryDataReviewWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "DataStorageDirManager.h"
#include "LanguageManager.h"
#include "TimeDate.h"
#include "HistoryDataSelWidget.h"
#include "TrendDataWidget.h"
#include "TrendGraphWidget.h"
#include "EventReviewWindow.h"
#include "OxyCRGEventWidget.h"

#define     ITEM_HEIGHT                 30

HistoryDataReviewWidget *HistoryDataReviewWidget::_selfObj = NULL;

HistoryDataReviewWidget::~HistoryDataReviewWidget()
{

}

void HistoryDataReviewWidget::setHistoryReviewIndex(int index, QString timeStr)
{
    _selHistoryDataPath = DATA_STORE_PATH;
    _selHistoryDataPath += dataStorageDirManager.getRescueDataDir(index);

    _historyTimeBtn->setText(timeStr);
}

void HistoryDataReviewWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

    _historyTimeBtn->setFocus();

}

void HistoryDataReviewWidget::_historyTimeReleased()
{
    HistoryDataSelWidget selWidget;
    selWidget.exec();
}

void HistoryDataReviewWidget::_trendTableBtnReleased()
{
    trendDataWidget.setHistoryDataPath(_selHistoryDataPath);
    trendDataWidget.setHistoryData(true);
    trendDataWidget.autoShow();
}

void HistoryDataReviewWidget::_trendGraphBtnReleased()
{
    trendGraphWidget.setHistoryDataPath(_selHistoryDataPath);
    trendGraphWidget.setHistoryData(true);
    trendGraphWidget.autoShow();
}

void HistoryDataReviewWidget::_eventReviewBtnReleased()
{
    eventReviewWindow.setHistoryDataPath(_selHistoryDataPath);
    eventReviewWindow.setHistoryData(true);
    eventReviewWindow.autoShow();
}

void HistoryDataReviewWidget::_oxyCRGEventBtnReleased()
{
    oxyCRGEventWidget.setHistoryDataPath(_selHistoryDataPath);
    oxyCRGEventWidget.setHistoryData(true);
    oxyCRGEventWidget.autoShow();
}

HistoryDataReviewWidget::HistoryDataReviewWidget() : PopupWidget(), _selHistoryDataPath(DATA_STORE_PATH)
{
    setTitleBarText(trs("HistoryTrend"));

    int maxW = windowManager.getPopMenuWidth();
    int maxH = windowManager.getPopMenuHeight();
    setFixedSize(maxW / 4, maxH / 2);

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);

    _trendTableBtn = new IButton(trs("TrendTable"));
    _trendTableBtn->setFixedHeight(ITEM_HEIGHT);
    _trendTableBtn->setFont(font);
    connect(_trendTableBtn, SIGNAL(realReleased()), this, SLOT(_trendTableBtnReleased()));

    _trendGraphBtn = new IButton(trs("TrendGraph"));
    _trendGraphBtn->setFixedHeight(ITEM_HEIGHT);
    _trendGraphBtn->setFont(font);
    connect(_trendGraphBtn, SIGNAL(realReleased()), this, SLOT(_trendGraphBtnReleased()));

    _eventReviewBtn = new IButton(trs("EventReview"));
    _eventReviewBtn->setFixedHeight(ITEM_HEIGHT);
    _eventReviewBtn->setFont(font);
    connect(_eventReviewBtn, SIGNAL(realReleased()), this, SLOT(_eventReviewBtnReleased()));

    _oxyCRGEventBtn = new IButton(trs("OxyCRGEventReview"));
    _oxyCRGEventBtn->setFixedHeight(ITEM_HEIGHT);
    _oxyCRGEventBtn->setFont(font);
    connect(_oxyCRGEventBtn, SIGNAL(realReleased()), this, SLOT(_oxyCRGEventBtnReleased()));

    _historyTimeBtn = new IButton("---------");
    _historyTimeBtn->setFixedHeight(ITEM_HEIGHT);
    _historyTimeBtn->setFont(font);
    connect(_historyTimeBtn, SIGNAL(realReleased()), this, SLOT(_historyTimeReleased()));

    contentLayout->setSpacing(10);
    contentLayout->addWidget(_trendTableBtn);
    contentLayout->addWidget(_trendGraphBtn);
    contentLayout->addWidget(_eventReviewBtn);
    contentLayout->addWidget(_oxyCRGEventBtn);
    contentLayout->addStretch();
    contentLayout->addWidget(_historyTimeBtn);
}
