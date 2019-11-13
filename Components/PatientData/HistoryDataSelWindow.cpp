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
#include "Framework/UI/TableView.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include "HistoryDataSelModel.h"
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "HistoryDataReviewWindow.h"
#include <QScrollBar>
#include "WindowManager.h"

class HistoryDataSelWindowPrivate
{
public:
    explicit HistoryDataSelWindowPrivate(HistoryDataSelWindow * const q_ptr)
        : table(NULL), model(NULL), upPageBtn(NULL),
          downPageBtn(NULL), q_ptr(q_ptr)
    {}

    /**
     * @brief refreshPage 刷新页面信息，页码和按键的使能情况
     */
    void refreshPage(void);
public:
    TableView *table;
    HistoryDataSelModel *model;
    Button *upPageBtn;
    Button *downPageBtn;
    HistoryDataSelWindow *const q_ptr;
};
HistoryDataSelWindow::HistoryDataSelWindow()
    : Dialog(), d_ptr(new HistoryDataSelWindowPrivate(this))
{
    setWindowTitle(trs("HistoryDataList"));
    setFixedSize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());

    d_ptr->table = new TableView();
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
    d_ptr->table->setFixedHeight(d_ptr->model->getEachPageRowCount() * d_ptr->model->getRowHeightHint());
    connect(d_ptr->table, SIGNAL(itemClicked(QModelIndex)), this, SLOT(itemSelReleased(QModelIndex)));
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
    Dialog::showEvent(ev);
    d_ptr->refreshPage();
}

void HistoryDataSelWindow::itemSelReleased(QModelIndex index)
{
    int dataIndex = index.row() * 2 + index.column();
    QString timeStr = d_ptr->model->getDateTimeStr(dataIndex);
    HistoryDataReviewWindow::getInstance()->setHistoryReviewIndex(dataIndex, timeStr);
    close();
}

void HistoryDataSelWindow::upReleased()
{
    int maxValue = d_ptr->table->verticalScrollBar()->maximum();
    int curScroller = d_ptr->table->verticalScrollBar()->value();
    if (curScroller > 0)
    {
        int rowCount = d_ptr->model->getEachPageRowCount();
        QScrollBar *scrollBar = d_ptr->table->verticalScrollBar();
        int position = curScroller - (maxValue * rowCount) / (d_ptr->model->rowCount(QModelIndex()) - rowCount);
        scrollBar->setSliderPosition(position);
    }

    d_ptr->refreshPage();
}

void HistoryDataSelWindow::downReleased()
{
    int maxValue = d_ptr->table->verticalScrollBar()->maximum();
    int curScroller = d_ptr->table->verticalScrollBar()->value();
    if (curScroller < maxValue)
    {
        int rowCount = d_ptr->model->getEachPageRowCount();
        QScrollBar *scrollBar = d_ptr->table->verticalScrollBar();
        int position = curScroller + (maxValue * rowCount) / (d_ptr->model->rowCount(QModelIndex()) - rowCount);
        scrollBar->setSliderPosition(position);
    }

    d_ptr->refreshPage();
}

void HistoryDataSelWindowPrivate::refreshPage()
{
    int totalPage = 1, curPage = 1;
    table->getPageInfo(&curPage, &totalPage);
    curPage = curPage == 0 ? 1 : curPage;
    totalPage = totalPage == 0 ? 1 : totalPage;
    QString title = QString("%1(%2 / %3)").arg(trs("HistoryDataList")).arg(curPage).arg(totalPage);
    q_ptr->setWindowTitle(title);
}
