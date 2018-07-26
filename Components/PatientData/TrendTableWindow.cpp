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

#define ITEM_HEIGHT             30
#define ITEM_WIDTH              100
#define TABLE_ROW_NR            6
#define TABLE_COL_NR            7
#define TABLE_ITEM_WIDTH        65
#define TABLE_ITEM_HEIGHT       35

TrendTableWindow *TrendTableWindow::_selfObj = NULL;

class TrendTableWindowPrivate
{
public:
    TrendTableWindowPrivate()
        : model(NULL), table(NULL), up(NULL), down(NULL),
          left(NULL), right(NULL), incidentMove(NULL), incident(NULL),
          printParam(NULL), set(NULL), timeInterval(RESOLUTION_RATIO_5_SECOND),
          curSecCol(0)
    {}

public:
    TrendTableModel *model;
    TableView *table;
    Button *up;
    Button *down;
    Button *left;
    Button *right;
    IMoveButton *incidentMove;
    Button *incident;
    Button *printParam;
    Button *set;
    ResolutionRatio timeInterval;          // 时间间隔

    int curSecCol;                         // 当前选中列
};

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

    d_ptr->model = new TrendTableModel();
    d_ptr->table->setModel(d_ptr->model);
    d_ptr->table->setFixedHeight(d_ptr->model->getHeaderHeightHint()
                                 + d_ptr->model->getRowHeightHint() * TABLE_ROW_NR);
    d_ptr->table->setFixedWidth(800);
    d_ptr->table->setItemDelegate(new TableViewItemDelegate(this));

    QIcon upIcon = themeManger.getIcon(ThemeManager::IconUp);
    d_ptr->up = new Button("", upIcon);
    d_ptr->up->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->up, SIGNAL(released()), this, SLOT(upReleased()));

    QIcon downIcon = themeManger.getIcon(ThemeManager::IconDown);
    d_ptr->down = new Button("", downIcon);
    d_ptr->down->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->down, SIGNAL(released()), this, SLOT(downReleased()));

    QIcon leftIcon = themeManger.getIcon(ThemeManager::IconLeft);
    d_ptr->left = new Button("", leftIcon);
    d_ptr->left->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->left, SIGNAL(released()), this, SLOT(leftReleased()));

    QIcon rightIcon = themeManger.getIcon(ThemeManager::IconRight);
    d_ptr->right = new Button("", rightIcon);
    d_ptr->right->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->right, SIGNAL(released()), this, SLOT(rightReleased()));

    d_ptr->incidentMove = new IMoveButton(trs("IncidentMove"));
    d_ptr->incidentMove->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->incidentMove->setFont(fontManager.textFont(fontManager.getFontSize(1)));
    connect(d_ptr->incidentMove, SIGNAL(leftMove()), this, SLOT(leftMoveEvent()));
    connect(d_ptr->incidentMove, SIGNAL(rightMove()), this, SLOT(rightMoveEvent()));

    d_ptr->incident = new Button(trs("Incident"));
    d_ptr->incident->setButtonStyle(Button::ButtonTextOnly);

    d_ptr->printParam = new Button(trs("Print"));
    d_ptr->printParam->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->printParam, SIGNAL(released()), this, SLOT(printWidgetRelease()));

    d_ptr->set = new Button(trs("Set"));
    d_ptr->set->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->set, SIGNAL(released()), this, SLOT(trendDataSetReleased()));

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addWidget(d_ptr->up);
    hLayout->addWidget(d_ptr->down);
    hLayout->addWidget(d_ptr->left);
    hLayout->addWidget(d_ptr->right);
    hLayout->addWidget(d_ptr->incidentMove);
    hLayout->addWidget(d_ptr->incident);
    hLayout->addWidget(d_ptr->printParam);
    hLayout->addWidget(d_ptr->set);

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

void TrendTableWindow::trendDataSetReleased()
{
    hide();
    trendTableSetWindow.show();
}
