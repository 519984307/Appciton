/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/20
 **/

#include "TrendTableWindow.h"
#include "ThemeManager.h"
#include "LanguageManager.h"
#include "TableView.h"
#include "TableHeaderView.h"
#include "TrendTableModel.h"
#include "Button.h"
#include "IMoveButton.h"
#include "ComboBox.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "ParamManager.h"
#include "TimeDate.h"
#include "TrendDataSetWidget.h"
#include "TimeManager.h"
#include "TrendDataStorageManager.h"
#include "IBPParam.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QStylePainter>
#include <QApplication>
#include <QScrollBar>
#include <QTableWidgetItem>
#include "RecorderManager.h"
#include "TrendTablePageGenerator.h"
#include "IConfig.h"
#include "TrendPrintWidget.h"
#include "EventStorageManager.h"
#include "EventDataParseContext.h"
#include "DataStorageDefine.h"
#include "TrendDataSymbol.h"
#include "TableViewItemDelegate.h"
#include "TrendTableSetWindow.h"
#include "TrendPrintWindow.h"

#define ITEM_HEIGHT             30
#define ITEM_WIDTH              100
#define TABLE_ROW_NR            6
#define TABLE_COL_NR            7
#define TABLE_ITEM_WIDTH        65
#define TABLE_ITEM_HEIGHT       35

class TrendTableWindowPrivate
{
public:
    TrendTableWindowPrivate()
        : model(NULL), table(NULL), upBtn(NULL), downBtn(NULL),
          leftBtn(NULL), rightBtn(NULL), previousEventBtn(NULL), nextEventBtn(NULL),
          printParamBtn(NULL), setBtn(NULL), timeInterval(RESOLUTION_RATIO_5_SECOND),
          curSecCol(0)
    {}

public:
    TrendTableModel *model;
    TableView *table;
    Button *upBtn;
    Button *downBtn;
    Button *leftBtn;
    Button *rightBtn;
    Button *previousEventBtn;
    Button *nextEventBtn;
    Button *printParamBtn;
    Button *setBtn;
    ResolutionRatio timeInterval;          // 时间间隔

    int curSecCol;                         // 当前选中列
};

TrendTableWindow *TrendTableWindow::getInstance()
{
    static TrendTableWindow *instance = NULL;
    if (!instance)
    {
        instance = new TrendTableWindow;
    }
    return instance;
}

TrendTableWindow::~TrendTableWindow()
{
}

void TrendTableWindow::setTimeInterval(int t)
{
    d_ptr->model->setTimeInterval(t);
}

void TrendTableWindow::setTrendGroup(int g)
{
    d_ptr->model->loadCurParam(g);
}

void TrendTableWindow::setHistoryDataPath(QString path)
{
    d_ptr->model->setHistoryDataPath(path);
}

void TrendTableWindow::setHistoryData(bool flag)
{
    d_ptr->model->setHistoryData(flag);
}

void TrendTableWindow::printTrendData(unsigned startTime, unsigned endTime)
{
    d_ptr->model->printTrendData(startTime, endTime);
}

void TrendTableWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);
    d_ptr->model->updateData();

    // 重新打开窗口时总是选择最后一列
    int totalCol = d_ptr->model->columnCount(QModelIndex());
    d_ptr->curSecCol = totalCol - 1;
    d_ptr->table->selectColumn(d_ptr->curSecCol);
}

TrendTableWindow::TrendTableWindow()
    : Window(), d_ptr(new TrendTableWindowPrivate())
{
    setWindowTitle(trs("TrendTable"));
    resize(800, 580);

    d_ptr->table = new TableView();
    TableHeaderView *horizontalHeader = new TableHeaderView(Qt::Horizontal);
    TableHeaderView *verticalHeader = new TableHeaderView(Qt::Vertical);
    d_ptr->table->setHorizontalHeader(horizontalHeader);
    d_ptr->table->setVerticalHeader(verticalHeader);
    horizontalHeader->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->table->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->table->setSelectionBehavior(QAbstractItemView::SelectColumns);
    d_ptr->table->setShowGrid(false);
    d_ptr->table->setCornerButtonEnabled(false);
    d_ptr->model = new TrendTableModel();
    d_ptr->table->installEventFilter(d_ptr->model);
    d_ptr->table->setModel(d_ptr->model);
    d_ptr->table->setFixedHeight(d_ptr->model->getHeaderHeightHint()
                                 + d_ptr->model->getRowHeightHint() * TABLE_ROW_NR);
    d_ptr->table->setFixedWidth(800);
    d_ptr->table->setItemDelegate(new TableViewItemDelegate(this));

    d_ptr->upBtn = new Button("", QIcon("/usr/local/nPM/icons/up.png"));
    d_ptr->upBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->upBtn, SIGNAL(released()), this, SLOT(upReleased()));

    d_ptr->downBtn = new Button("", QIcon("/usr/local/nPM/icons/down.png"));
    d_ptr->downBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->downBtn, SIGNAL(released()), this, SLOT(downReleased()));

    d_ptr->leftBtn = new Button("", QIcon("/usr/local/nPM/icons/left.png"));
    d_ptr->leftBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->leftBtn, SIGNAL(released()), this, SLOT(leftReleased()));

    d_ptr->rightBtn = new Button("", QIcon("/usr/local/nPM/icons/right.png"));
    d_ptr->rightBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->rightBtn, SIGNAL(released()), this, SLOT(rightReleased()));

    d_ptr->previousEventBtn = new Button(trs("PreviousEvent"));
    d_ptr->previousEventBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->previousEventBtn, SIGNAL(released()), this, SLOT(leftMoveEvent()));

    d_ptr->nextEventBtn = new Button(trs("NextEvent"));
    d_ptr->nextEventBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->nextEventBtn, SIGNAL(released()), this, SLOT(rightMoveEvent()));

    d_ptr->printParamBtn = new Button(trs("Print"));
    d_ptr->printParamBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->printParamBtn, SIGNAL(released()), this, SLOT(printWidgetRelease()));

    d_ptr->setBtn = new Button(trs("Set"));
    d_ptr->setBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->setBtn, SIGNAL(released()), this, SLOT(trendDataSetReleased()));

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addWidget(d_ptr->upBtn, 1);
    hLayout->addWidget(d_ptr->downBtn, 1);
    hLayout->addWidget(d_ptr->leftBtn, 1);
    hLayout->addWidget(d_ptr->rightBtn, 1);
    hLayout->addWidget(d_ptr->previousEventBtn, 2);
    hLayout->addWidget(d_ptr->nextEventBtn, 2);
    hLayout->addWidget(d_ptr->printParamBtn, 2);
    hLayout->addWidget(d_ptr->setBtn, 2);

    QVBoxLayout *layout = new QVBoxLayout();

    layout->addStretch();
    layout->addWidget(d_ptr->table);
    layout->addStretch();
    layout->addLayout(hLayout);

    setWindowLayout(layout);

    QString prefix = "TrendTable|";
    int index = 0;
    QString ratioPrefix = prefix + "ResolutionRatio";
    systemConfig.getNumValue(ratioPrefix, index);
    d_ptr->timeInterval = (ResolutionRatio)index;
    index = 0;
    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
}

void TrendTableWindow::leftReleased()
{
    d_ptr->model->leftPage(d_ptr->curSecCol);
    d_ptr->table->selectColumn(d_ptr->curSecCol);
}

void TrendTableWindow::rightReleased()
{
    d_ptr->model->rightPage(d_ptr->curSecCol);
    d_ptr->table->selectColumn(d_ptr->curSecCol);
}

void TrendTableWindow::upReleased()
{
    d_ptr->table->scrollToPreviousPage();
}

void TrendTableWindow::downReleased()
{
    d_ptr->table->scrollToNextPage();
}

void TrendTableWindow::leftMoveEvent()
{
    d_ptr->model->leftMoveEvent(d_ptr->curSecCol);
    d_ptr->table->selectColumn(d_ptr->curSecCol);
}

void TrendTableWindow::rightMoveEvent()
{
    d_ptr->model->rightMoveEvent(d_ptr->curSecCol);
    d_ptr->table->selectColumn(d_ptr->curSecCol);
}

void TrendTableWindow::printWidgetRelease()
{
    unsigned startLimit = 0;
    unsigned endLimit = 0;
    if (d_ptr->model->getDataTimeRange(startLimit, endLimit))
    {
        TrendPrintWindow printWindow;
        unsigned startTime = 0;
        unsigned endTime = 0;
        d_ptr->model->displayDataTimeRange(startTime, endTime);
        printWindow.printTimeRange(startLimit, endLimit);
        printWindow.initPrintTime(startTime, endTime);
        printWindow.exec();
    }
}

void TrendTableWindow::trendDataSetReleased()
{
    hide();
    windowManager.showWindow(&trendTableSetWindow);
}
