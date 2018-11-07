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
#include "TableItemDelegate.h"
#include "ListViewItemDelegate.h"
#include "LanguageManager.h"
#include "Button.h"
#include <QLayout>
#include "AlarmIndicator.h"
#include "TimeDate.h"
#include <QHeaderView>
#include "Alarm.h"
#include "EventWindow.h"
#include "WindowManager.h"

#define PATH_ICON_UP "/usr/local/nPM/icons/ArrowUp.png"
#define PATH_ICON_DOWN "/usr/local/nPM/icons/ArrowDown.png"
#define LISTVIEW_MAX_VISIABLE_SIZE 6    // 一页最大显示数

class AlarmInfoWindowPrivate
{
public:
    AlarmInfoWindowPrivate()
        : dataModel(NULL),
          tableView(NULL),
          listModel(NULL),
          listView(NULL),
          totalList(0),
          totalPage(1),
          curPage(1),
          prevBtn(NULL),
          nextBtn(NULL),
          title(QString())
    {}
    ~AlarmInfoWindowPrivate() {}

    AlarmInfoModel *dataModel;
    TableView *tableView;
    ListDataModel *listModel;
    ListView *listView;
    int totalList;      // 报警总条数
    int totalPage;      // 总页数
    int curPage;        //当前页码
    AlarmType alarmType;
    Button *prevBtn;
    Button *nextBtn;
    QString title;
    QList<AlarmInfoNode> nodeList;

    /**
     * @brief loadOption　加载列表的值
     */
    void loadOption();

    bool hasPrevPage();
    bool hasNextPage();
};

AlarmInfoWindow::AlarmInfoWindow(const QString &title, AlarmType type)
    : Window(),
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
    if (total <= 1)
    {
        d_ptr->totalList = total;
        hide();
        return;
    }

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
    setFixedSize(480, 450);

    QVBoxLayout *vLayout = new QVBoxLayout();
    setWindowLayout(vLayout);

    if (d_ptr->alarmType == ALARM_TYPE_PHY)
    {
        TableView *tableView = new TableView();
        tableView->verticalHeader()->setVisible(false);
        tableView->horizontalHeader()->setVisible(false);
        tableView->setShowGrid(false);
        tableView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
        tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setItemDelegate(new TableItemDelegate);
        AlarmInfoModel *model = new AlarmInfoModel(this);
        tableView->setModel(model);
        tableView->setFixedHeight(model->getRowHeightHint() * LISTVIEW_MAX_VISIABLE_SIZE);
        d_ptr->dataModel = model;
        d_ptr->tableView = tableView;

        vLayout->addStretch();
        vLayout->addWidget(tableView);
        connect(tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(_accessEventWindow(QModelIndex)));
        connect(tableView, SIGNAL(rowClicked(int)), this, SLOT(_accessEventWindow(int)));
    }
    else if (d_ptr->alarmType == ALARM_TYPE_TECH)
    {
        ListView *listView = new ListView();
        listView->setItemDelegate(new ListViewItemDelegate);
        listView->setSelectionMode(QAbstractItemView::NoSelection);
        ListDataModel *model = new ListDataModel(this);
        listView->setModel(model);
        listView->setFixedHeight(model->getRowHeightHint() * LISTVIEW_MAX_VISIABLE_SIZE);
        listView->setDrawIcon(false);
        listView->setSpacing(0);
        listView->setFocusPolicy(Qt::NoFocus);
        d_ptr->listModel = model;
        d_ptr->listView = listView;

        vLayout->addStretch();
        vLayout->addWidget(listView);
    }

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(2);
    hLayout->setSpacing(2);
    //　上一页按钮
    d_ptr->prevBtn = new Button();
    d_ptr->prevBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->prevBtn->setText(trs("Prev"));
    connect(d_ptr->prevBtn, SIGNAL(released()), this, SLOT(_onBtnRelease()));
    hLayout->addWidget(d_ptr->prevBtn);

    //　下一页按钮
    d_ptr->nextBtn = new Button();
    d_ptr->nextBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->nextBtn->setText(trs("Next"));
    connect(d_ptr->nextBtn, SIGNAL(released()), this, SLOT(_onBtnRelease()));
    hLayout->addWidget(d_ptr->nextBtn);

    vLayout->addStretch();
    vLayout->addLayout(hLayout);
}

void AlarmInfoWindow::showEvent(QShowEvent *ev)
{
    updateData(true);
    Window::showEvent(ev);
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

    int start = 0, end = 0;
    int count = nodeList.count();

    // 计算当前页码和总页数
    int pageTemp = (0 == count % LISTVIEW_MAX_VISIABLE_SIZE) ? (count / LISTVIEW_MAX_VISIABLE_SIZE)
                                                              : (count / LISTVIEW_MAX_VISIABLE_SIZE + 1);
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
    start = (curPage - 1) * LISTVIEW_MAX_VISIABLE_SIZE;
    if (count > LISTVIEW_MAX_VISIABLE_SIZE)
    {
        end = start + LISTVIEW_MAX_VISIABLE_SIZE;
        if (end > count)
        {
            end = count;
            start = count - LISTVIEW_MAX_VISIABLE_SIZE;
        }
    }
    else
    {
        end = LISTVIEW_MAX_VISIABLE_SIZE;
    }

    if (alarmType == ALARM_TYPE_PHY)
    {
        QStringList alarmList;
        QStringList timeList;
        for (int i = start; i < end; ++i)
        {
            if (i < count)
            {
                node = nodeList.at(i);
                QString nameStr;
                switch (node.alarmSource->getAlarmPriority(node.alarmID))
                {
                case ALARM_PRIO_LOW:
                    nameStr = "*";
                    break;
                case ALARM_PRIO_MED:
                    nameStr = "**";
                    break;
                case ALARM_PRIO_HIGH:
                    nameStr = "***";
                    break;
                default:
                    break;
                }
                nameStr += " ";
                nameStr += trs(node.alarmMessage);
                alarmList.append(nameStr);

                QString dateStr;
                QString timeStr;
                QString str;
                timeDate.getTime(node.alarmTime, timeStr, true);
                timeDate.getDate(node.alarmTime, dateStr, true);
                str = dateStr + " " + timeStr;
                timeList.append(str);
            }
        }
        dataModel->setStringList(alarmList, timeList);
    }
    else if (alarmType == ALARM_TYPE_TECH)
    {
        QStringList alarmList;
        for (int i = start; i < end; ++i)
        {
            if (i < count)
            {
                node = nodeList.at(i);
                alarmList.append(trs(node.alarmMessage));
            }
        }

        listModel->setStringList(alarmList);
    }

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
}

bool AlarmInfoWindowPrivate::hasPrevPage()
{
    return curPage > 1;
}

bool AlarmInfoWindowPrivate::hasNextPage()
{
    return curPage < totalPage;
}
