/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/25
 **/

#include "AlarmInfoWindow.h"
#include "TableView.h"
#include "AlarmInfoModel.h"
#include "ListView.h"
#include "ListDataModel.h"
#include "TableViewItemDelegate.h"
#include "ListViewItemDelegate.h"
#include "LanguageManager.h"
#include "Button.h"
#include <QLayout>
#include "AlarmIndicator.h"
#include "TimeDate.h"
#include <QHeaderView>
#include "TableHeaderView.h"
#include "Alarm.h"
#include "EventWindow.h"
#include "WindowManager.h"
#include "ThemeManager.h"

#define PATH_ICON_UP "/usr/local/nPM/icons/ArrowUp.png"
#define PATH_ICON_DOWN "/usr/local/nPM/icons/ArrowDown.png"
#define EACH_PAGE_ALARM_COUNT 7    // 一页最大显示数

class AlarmInfoWindowPrivate
{
public:
    AlarmInfoWindowPrivate()
        : dataModel(NULL),
          tableView(NULL),
          totalList(0),
          totalPage(1),
          curPage(1),
          prevBtn(NULL),
          nextBtn(NULL),
          title(QString()),
          refreshData(true)
    {}
    ~AlarmInfoWindowPrivate() {}

    AlarmInfoModel *dataModel;
    TableView *tableView;
    int totalList;      // 报警总条数
    int totalPage;      // 总页数
    int curPage;        //当前页码
    AlarmType alarmType;
    Button *prevBtn;
    Button *nextBtn;
    QString title;
    QList<AlarmInfoNode> nodeList;
    bool refreshData;  // 用于标志是否刷新报警数据

    /**
     * @brief loadOption　加载列表的值
     */
    void loadOption();

    bool hasPrevPage();
    bool hasNextPage();

    /**
     * @brief updateAcknowledgeFlag 更新被确认标志
     */
    void updateAcknowledgeFlag();
};

AlarmInfoWindow::AlarmInfoWindow(const QString &title, AlarmType type)
    : Dialog(),
      d_ptr(new AlarmInfoWindowPrivate)
{
    d_ptr->title = title;
    d_ptr->alarmType = type;
    layout();
}

AlarmInfoWindow::~AlarmInfoWindow()
{
    delete d_ptr;
}

void AlarmInfoWindow::updateData(bool isShowFirstPage)
{
    int total = alarmIndicator.getAlarmCount();

    d_ptr->totalList = total;
    if (isShowFirstPage)
    {
        d_ptr->curPage = 1;
    }
    d_ptr->loadOption();
}

void AlarmInfoWindow::layout()
{
    setWindowTitle(d_ptr->title);
    setFixedSize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());

    QVBoxLayout *vLayout = new QVBoxLayout();
    setWindowLayout(vLayout);

    TableView *tableView = new TableView();
    TableHeaderView *headerView = new TableHeaderView(Qt::Horizontal);
    tableView->verticalHeader()->setVisible(false);
    tableView->setHorizontalHeader(headerView);
    tableView->setShowGrid(false);
    tableView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setIconSize(QSize(24, 24));
    tableView->setItemDelegate(new TableViewItemDelegate(tableView));
    AlarmInfoModel *model = new AlarmInfoModel(this);
    tableView->setModel(model);
    tableView->viewport()->installEventFilter(model);
    tableView->setFixedHeight(model->getRowHeightHint() * (EACH_PAGE_ALARM_COUNT + 1));
    d_ptr->dataModel = model;
    d_ptr->tableView = tableView;
    vLayout->addWidget(tableView);

    if (d_ptr->alarmType == ALARM_TYPE_PHY)
    {
        connect(tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(_accessEventWindow(QModelIndex)));
        connect(tableView, SIGNAL(rowClicked(int)), this, SLOT(_accessEventWindow(int)));
    }

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(2);
    //　上一页按钮
    d_ptr->prevBtn = new Button();
    d_ptr->prevBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->prevBtn->setText(trs("Prev"));
    d_ptr->prevBtn->setFixedWidth(150);
    connect(d_ptr->prevBtn, SIGNAL(released()), this, SLOT(_onBtnRelease()));
    hLayout->addWidget(d_ptr->prevBtn);

    //　下一页按钮
    d_ptr->nextBtn = new Button();
    d_ptr->nextBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->nextBtn->setText(trs("Next"));
    d_ptr->nextBtn->setFixedWidth(150);
    connect(d_ptr->nextBtn, SIGNAL(released()), this, SLOT(_onBtnRelease()));
    hLayout->addWidget(d_ptr->nextBtn);

    vLayout->addStretch();
    vLayout->addLayout(hLayout);
}

void AlarmInfoWindow::showEvent(QShowEvent *ev)
{
    d_ptr->refreshData = true;
    updateData(true);
    Dialog::showEvent(ev);
}

void AlarmInfoWindow::_onBtnRelease()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->prevBtn)
    {
        if (d_ptr->hasPrevPage())
        {
            d_ptr->curPage--;
            d_ptr->loadOption();
        }
    }
    else if (btn == d_ptr->nextBtn)
    {
        if (d_ptr->hasNextPage())
        {
            d_ptr->curPage++;
            d_ptr->loadOption();
        }
    }
}

void AlarmInfoWindow::_accessEventWindow(QModelIndex index)
{
    _accessEventWindow(index.row());
}

void AlarmInfoWindow::_accessEventWindow(int index)
{
    if (d_ptr->nodeList.count() == 0)
    {
        return;
    }
    AlarmInfoNode node = d_ptr->nodeList.at(index);
    SubParamID id = node.alarmSource->getSubParamID(node.alarmID);
    unsigned time = node.alarmTime;

    windowManager.showWindow(EventWindow::getInstance(), WindowManager::ShowBehaviorCloseIfVisiable
                             | WindowManager::ShowBehaviorCloseOthers);
    EventWindow::getInstance()->findEventIndex(id, time);
}

void AlarmInfoWindowPrivate::loadOption()
{
    AlarmInfoNode node;
    if (refreshData)
    {
        // 显示窗口后，不再获取新的报警数据。
        nodeList.clear();
        for (int i = totalList - 1; i >= 0; --i)
        {
            alarmIndicator.getAlarmInfo(i, node);
            if (node.alarmType != alarmType)
            {
                continue;
            }
            nodeList.append(node);
        }
        refreshData = false;
    }

    int start = 0, end = 0;
    int count = nodeList.count();

    // 计算当前页码和总页数
    int pageTemp = (0 == count % EACH_PAGE_ALARM_COUNT) ? (count / EACH_PAGE_ALARM_COUNT)
                                                              : (count / EACH_PAGE_ALARM_COUNT + 1);
    if (pageTemp == 0)
    {
        return;
    }
    if (totalPage != pageTemp)
    {
        totalPage = pageTemp;
        if (curPage > totalPage)
        {
            curPage = totalPage;
        }
    }

    if (count < 1)
    {
        return;
    }
    start = (curPage - 1) * EACH_PAGE_ALARM_COUNT;
    if (curPage < totalPage)
    {
        end = (curPage - 1) * EACH_PAGE_ALARM_COUNT + EACH_PAGE_ALARM_COUNT;
    }
    else if (curPage == totalPage)
    {
        end = count;
    }

    QStringList alarmList;
    QStringList timeList;
    QStringList priorityList;
    for (int i = start; i < end; ++i)
    {
        if (i < count)
        {
            node = nodeList.at(i);
            alarmList.append(trs(node.alarmMessage));

            QString priorityStr;
            switch (node.alarmSource->getAlarmPriority(node.alarmID))
            {
            case ALARM_PRIO_LOW:
                priorityStr = trs("Low");
                break;
            case ALARM_PRIO_MED:
                priorityStr = trs("Medium");
                break;
            case ALARM_PRIO_HIGH:
                priorityStr = trs("High");
                break;
            default:
                break;
            }
            priorityList.append(priorityStr);
            QString dateStr;
            QString timeStr;
            QString str;
            timeDate.getTime(node.alarmTime, timeStr, true);
            timeDate.getDate(node.alarmTime, dateStr, true);
            str = dateStr + " " + timeStr;
            timeList.append(str);
        }
    }
    dataModel->setStringList(alarmList, timeList, priorityList);

    if (hasNextPage())
    {
        nextBtn->setEnabled(true);
    }
    else
    {
        nextBtn->setEnabled(false);
    }
    if (hasPrevPage())
    {
        prevBtn->setEnabled(true);
        prevBtn->setFocus();
    }
    else
    {
        prevBtn->setEnabled(false);
    }
    updateAcknowledgeFlag();
}

bool AlarmInfoWindowPrivate::hasPrevPage()
{
    return curPage > 1;
}

bool AlarmInfoWindowPrivate::hasNextPage()
{
    return curPage < totalPage;
}

void AlarmInfoWindowPrivate::updateAcknowledgeFlag()
{
    for (int i = 0; i < nodeList.count(); i++)
    {
        if (i >= (curPage - 1) * EACH_PAGE_ALARM_COUNT && i < (curPage) * EACH_PAGE_ALARM_COUNT
                && nodeList.at(i).acknowledge)
        {
            // 只刷新当前页的确认标志
            int row = i % EACH_PAGE_ALARM_COUNT;
            QModelIndex index = dataModel->index(row, 0);
            if (index.isValid())
            {
                dataModel->setData(index, themeManger.getIcon(ThemeManager::IconChecked, QSize(24, 24)),
                                   Qt::DecorationRole);
            }
        }
    }
}
