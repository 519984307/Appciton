/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/7
 **/

#include "HistoryDataSelWindow.h"
#include "TableView.h"
#include "Button.h"
#include "HistoryDataSelModel.h"
#include "LanguageManager.h"
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "HistoryDataReviewWindow.h"
#include <QScrollBar>

#define ROW_HEIGHT_HINT (themeManger.getAcceptableControlHeight())
#define ROW_COUNT       9

class HistoryDataSelWindowPrivate
{
public:
    HistoryDataSelWindowPrivate()
        : table(NULL), model(NULL), upPageBtn(NULL),
          downPageBtn(NULL)
    {}

    void pageBtnEnable(void);
public:
    TableView *table;
    HistoryDataSelModel *model;
    Button *upPageBtn;
    Button *downPageBtn;
};
HistoryDataSelWindow::HistoryDataSelWindow()
    : Window(), d_ptr(new HistoryDataSelWindowPrivate())
{
    setWindowTitle(trs("HistoryDataList"));
    setFixedSize(800, 580);

    d_ptr->table = new TableView();
    d_ptr->table->setFocusPolicy(Qt::NoFocus);
    d_ptr->table->horizontalHeader()->setVisible(false);
    d_ptr->table->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->table->verticalHeader()->setVisible(false);
    d_ptr->table->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->table->setSelectionBehavior(QAbstractItemView::SelectItems);
    d_ptr->table->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->table->setShowGrid(false);
    d_ptr->model = new HistoryDataSelModel();
    d_ptr->table->setModel(d_ptr->model);
    d_ptr->table->setFixedHeight(ROW_COUNT * d_ptr->model->getRowHeightHint());
    connect(d_ptr->table, SIGNAL(clicked(QModelIndex)), this, SLOT(itemSelReleased(QModelIndex)));

    d_ptr->upPageBtn = new Button("", QIcon("/usr/local/nPM/icons/up.png"));
    d_ptr->upPageBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->upPageBtn, SIGNAL(released()), this, SLOT(upReleased()));

    d_ptr->downPageBtn = new Button("", QIcon("/usr/local/nPM/icons/down.png"));
    d_ptr->downPageBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->downPageBtn, SIGNAL(released()), this, SLOT(downReleased()));

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addStretch(6);
    hLayout->addWidget(d_ptr->upPageBtn, 1);
    hLayout->addWidget(d_ptr->downPageBtn, 1);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->table);
    layout->addLayout(hLayout);

    d_ptr->model->updateData();

    setWindowLayout(layout);
}

HistoryDataSelWindow::~HistoryDataSelWindow()
{
}

void HistoryDataSelWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);
    d_ptr->pageBtnEnable();
}

void HistoryDataSelWindow::itemSelReleased(QModelIndex index)
{
    int dataIndex = index.row() * 2 + index.column();
    QString timeStr = d_ptr->model->getDateTimeStr(dataIndex);
    historyDataReviewWindow.setHistoryReviewIndex(dataIndex, timeStr);
    close();
}

void HistoryDataSelWindow::upReleased()
{
    int maxValue = d_ptr->table->verticalScrollBar()->maximum();
    int curScroller = d_ptr->table->verticalScrollBar()->value();
    if (curScroller > 0)
    {
        QScrollBar *scrollBar = d_ptr->table->verticalScrollBar();
        int position = curScroller - (maxValue * ROW_COUNT) / (d_ptr->model->rowCount(QModelIndex()) - ROW_COUNT);
        scrollBar->setSliderPosition(position);
    }

    d_ptr->pageBtnEnable();
    if (!d_ptr->upPageBtn->isEnabled())
    {
        d_ptr->downPageBtn->setFocus();
    }
}

void HistoryDataSelWindow::downReleased()
{
    int maxValue = d_ptr->table->verticalScrollBar()->maximum();
    int curScroller = d_ptr->table->verticalScrollBar()->value();
    if (curScroller < maxValue)
    {
        QScrollBar *scrollBar = d_ptr->table->verticalScrollBar();
        int position = curScroller + (maxValue * ROW_COUNT) / (d_ptr->model->rowCount(QModelIndex()) - ROW_COUNT);
        scrollBar->setSliderPosition(position);
    }

    d_ptr->pageBtnEnable();
    if (!d_ptr->downPageBtn->isEnabled())
    {
        d_ptr->upPageBtn->setFocus();
    }
}

void HistoryDataSelWindowPrivate::pageBtnEnable()
{
    int curScroller = table->verticalScrollBar()->value();
    int maxValue = table->verticalScrollBar()->maximum();
    bool hasBtn = curScroller > 0;
    upPageBtn->setEnabled(hasBtn);
    hasBtn = curScroller < maxValue;
    downPageBtn->setEnabled(hasBtn);
}
