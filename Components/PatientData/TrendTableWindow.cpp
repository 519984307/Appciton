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
#include "MoveButton.h"
#include "ComboBox.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "ParamManager.h"
#include "TimeDate.h"
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
    explicit TrendTableWindowPrivate(TrendTableWindow *parent)
        : model(NULL), table(NULL), upBtn(NULL), downBtn(NULL),
           pagingBtn(NULL), eventBtn(NULL),
          printParamBtn(NULL), setBtn(NULL), timeInterval(RESOLUTION_RATIO_5_SECOND),
          curSecCol(0), q_ptr(parent)
    {}

    void updateTable(void);

public:
    TrendTableModel *model;
    TableView *table;
    Button *upBtn;
    Button *downBtn;
    MoveButton *pagingBtn;
    MoveButton *eventBtn;
    Button *printParamBtn;
    Button *setBtn;
    ResolutionRatio timeInterval;          // 时间间隔

    int curSecCol;                         // 当前选中列
    TrendTableWindow *q_ptr;
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
    d_ptr->timeInterval = (ResolutionRatio)t;
    d_ptr->updateTable();
}

void TrendTableWindow::setTrendGroup(int g)
{
    d_ptr->model->loadCurParam(g);
    d_ptr->updateTable();
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

void TrendTableWindow::updatePages()
{
    unsigned startTime = 0;
    unsigned endTime = 0;
    unsigned rightTime = 0;
    unsigned allPagesNum = 1;
    unsigned curPageNum = 1;

    d_ptr->model->getDataTimeRange(startTime, endTime);

    unsigned timeInterval = TrendDataSymbol::convertValue(d_ptr->timeInterval);

    // 初始状态下起始时间与结束时间相同时显示页数1/1
    if (endTime == startTime)
    {
        allPagesNum = curPageNum = 1;
        setWindowTitle(QString("%1 ( %2 / %3 %4 )")
                       .arg(trs("TrendTable"))
                       .arg(curPageNum)
                       .arg(allPagesNum)
                       .arg(trs("Page")));
        return;
    }

    if (startTime % timeInterval != 0)
    {
        startTime = startTime + (timeInterval - startTime % timeInterval);
    }
    endTime = endTime - endTime % timeInterval;
    rightTime = d_ptr->model->getRightTime();

    if (endTime <= startTime)
    {
        allPagesNum = curPageNum = 1;
    }
    else
    {
        // 获取数据的总页数
        allPagesNum = 1 + (endTime - startTime) * 1.0 / timeInterval / d_ptr->model->getColumnCount();
        // 获取数据的当前页数
        curPageNum = 1 + (rightTime - startTime) * 1.0 / ((endTime - startTime) * 1.0) * (allPagesNum - 1);
    }
    setWindowTitle(QString("%1 ( %2 / %3 %4 )")
                   .arg(trs("TrendTable"))
                   .arg(curPageNum)
                   .arg(allPagesNum)
                   .arg(trs("Page")));
}

void TrendTableWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);
    d_ptr->updateTable();
    QAbstractButton *btn = d_ptr->table->findChild<QAbstractButton *>();
    if (btn)
    {
        QString dataTime;
        timeDate.getDate(dataTime);
        btn->setText(dataTime);
        btn->installEventFilter(this);
        QStyleOptionHeader opt;
        opt.text = btn->text();
        QSize s = (btn->style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), btn).expandedTo(QApplication::globalStrut()));
        if (s.isValid())
        {
            d_ptr->table->verticalHeader()->setMinimumWidth(s.width());
        }
    }
}

bool TrendTableWindow::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::Paint)
    {
        QAbstractButton* btn = qobject_cast<QAbstractButton*>(o);
        if (btn)
        {
            // paint by hand (borrowed from QTableCornerButton)

            QStyleOptionHeader opt;
            opt.rect = btn->rect();
            opt.text = btn->text(); // this line is the only difference to QTableCornerButton

            QPainter painter(btn);
            QPalette pal = palette();
            QColor textColor = themeManger.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementText,
                                                    ThemeManager::StateInactive);
            pal.setColor(QPalette::Background,
                         themeManger.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                                                    ThemeManager::StateDisabled));
            painter.fillRect(opt.rect, pal.background());
            painter.setPen(QPen(textColor, 1, Qt::SolidLine));
            painter.drawText(opt.rect, Qt::AlignCenter, btn->text());
            return true;
        }
    }
    return false;
}

TrendTableWindow::TrendTableWindow()
    : Window(), d_ptr(new TrendTableWindowPrivate(this))
{
    resize(825, 580);

    d_ptr->table = new TableView();
    TableHeaderView *horizontalHeader = new TableHeaderView(Qt::Horizontal);
    TableHeaderView *verticalHeader = new TableHeaderView(Qt::Vertical);
    d_ptr->table->setHorizontalHeader(horizontalHeader);
    d_ptr->table->setVerticalHeader(verticalHeader);
    horizontalHeader->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->table->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->table->setSelectionBehavior(QAbstractItemView::SelectColumns);
    d_ptr->table->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    d_ptr->table->setFocusPolicy(Qt::ClickFocus);
    d_ptr->table->setShowGrid(false);
    d_ptr->table->setCornerButtonEnabled(false);
    d_ptr->model = new TrendTableModel();
    d_ptr->table->installEventFilter(d_ptr->model);
    d_ptr->table->setModel(d_ptr->model);
    d_ptr->table->setFixedHeight(d_ptr->model->getHeaderHeightHint()
                                 + d_ptr->model->getRowHeightHint() * TABLE_ROW_NR);
    d_ptr->table->setFixedWidth(825);
    d_ptr->table->setItemDelegate(new TableViewItemDelegate(this));

    d_ptr->upBtn = new Button("", QIcon("/usr/local/nPM/icons/up.png"));
    d_ptr->upBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->upBtn, SIGNAL(released()), this, SLOT(upReleased()));

    d_ptr->downBtn = new Button("", QIcon("/usr/local/nPM/icons/down.png"));
    d_ptr->downBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->downBtn, SIGNAL(released()), this, SLOT(downReleased()));

    d_ptr->pagingBtn = new MoveButton(trs("Page"));
    d_ptr->pagingBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->pagingBtn, SIGNAL(leftMove()), this, SLOT(leftReleased()));
    connect(d_ptr->pagingBtn, SIGNAL(rightMove()), this, SLOT(rightReleased()));

    d_ptr->eventBtn = new MoveButton(trs("Event"));
    d_ptr->eventBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->eventBtn, SIGNAL(leftMove()), this, SLOT(leftMoveEvent()));
    connect(d_ptr->eventBtn, SIGNAL(rightMove()), this, SLOT(rightMoveEvent()));

    d_ptr->printParamBtn = new Button(trs("Print"));
    d_ptr->printParamBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->printParamBtn, SIGNAL(released()), this, SLOT(printWidgetRelease()));

    d_ptr->setBtn = new Button(trs("Set"));
    d_ptr->setBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->setBtn, SIGNAL(released()), this, SLOT(trendDataSetReleased()));

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addWidget(d_ptr->upBtn, 1);
    hLayout->addWidget(d_ptr->downBtn, 1);
    hLayout->addWidget(d_ptr->pagingBtn, 2);
    hLayout->addWidget(d_ptr->eventBtn, 2);
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

    updatePages();
}

void TrendTableWindow::leftReleased()
{
    d_ptr->model->leftPage(d_ptr->curSecCol);
    updatePages();
}

void TrendTableWindow::rightReleased()
{
    d_ptr->model->rightPage(d_ptr->curSecCol);
    updatePages();
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
    updatePages();
}

void TrendTableWindow::rightMoveEvent()
{
    d_ptr->model->rightMoveEvent(d_ptr->curSecCol);
    updatePages();
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
    windowManager.showWindow(&trendTableSetWindow, WindowManager::ShowBehaviorModal);
    updatePages();
}

void TrendTableWindowPrivate::updateTable()
{
    model->updateData();
    q_ptr->updatePages();
}
