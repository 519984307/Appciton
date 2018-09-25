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
#include "ListView.h"
#include "ListDataModel.h"
#include "ListViewItemDelegate.h"
#include "LanguageManager.h"
#include "Button.h"
#include <QLayout>
#include "AlarmIndicator.h"


#define PATH_ICON_UP "/usr/local/nPM/icons/ArrowUp.png"
#define PATH_ICON_DOWN "/usr/local/nPM/icons/ArrowDown.png"
#define LISTVIEW_MAX_VISIABLE_SIZE 6    // 一页最大显示数

class AlarmInfoWindowPrivate
{
public:
    AlarmInfoWindowPrivate()
        : dataModel(NULL),
          listView(NULL),
          totalList(0),
          totalPage(1),
          curPage(1),
          prevBtn(NULL),
          nextBtn(NULL),
          title(QString())
    {}
    ~AlarmInfoWindowPrivate() {}

    ListDataModel *dataModel;
    ListView *listView;
    int totalList;      // 报警总条数
    int totalPage;      // 总页数
    int curPage;        //当前页码
    AlarmType alarmType;
    Button *prevBtn;
    Button *nextBtn;
    QString title;

    /**
     * @brief loadOption　加载列表的值
     */
    void loadOption();

    bool hasPrevPage();
    bool hasNextPage();

    enum pageButton
    {
        PAGE_BUTTON_PREV,
        PAGE_BUTTON_NEXT
    };
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

void AlarmInfoWindow::updateData()
{
    int total = alarmIndicator.getAlarmCount();
    if (total <= 1)
    {
        d_ptr->totalList = total;
        hide();
        return;
    }

    d_ptr->totalList = total;
    d_ptr->loadOption();
}

void AlarmInfoWindow::layout()
{
    setWindowTitle(d_ptr->title);
    setFixedSize(480, 480);

    QVBoxLayout *vLayout = new QVBoxLayout();
    setWindowLayout(vLayout);

    ListView *listView = new ListView();
    vLayout->addWidget(listView);
    listView->setItemDelegate(new ListViewItemDelegate);
    ListDataModel *model = new ListDataModel(this);
    listView->setModel(model);
    listView->setFixedHeight(model->getRowHeightHint() * LISTVIEW_MAX_VISIABLE_SIZE
                             + listView->spacing() * (LISTVIEW_MAX_VISIABLE_SIZE * 2));
    listView->setDrawIcon(false);
    d_ptr->dataModel = model;
    d_ptr->listView = listView;

    vLayout->addWidget(listView);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(2);
    hLayout->setSpacing(2);
    //　上一页按钮
    d_ptr->prevBtn = new Button();
    d_ptr->prevBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->prevBtn->setText(trs("Prev"));
    int index = static_cast<int>(AlarmInfoWindowPrivate::PAGE_BUTTON_PREV);
    d_ptr->prevBtn->setProperty("Item", qVariantFromValue(index));
    connect(d_ptr->prevBtn, SIGNAL(released()), this, SLOT(_onBtnRelease()));
    hLayout->addWidget(d_ptr->prevBtn);

    //　下一页按钮
    d_ptr->nextBtn = new Button();
    d_ptr->nextBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->nextBtn->setText(trs("Next"));
    index = static_cast<int>(AlarmInfoWindowPrivate::PAGE_BUTTON_NEXT);
    d_ptr->nextBtn->setProperty("Item", qVariantFromValue(index));
    connect(d_ptr->nextBtn, SIGNAL(released()), this, SLOT(_onBtnRelease()));
    hLayout->addWidget(d_ptr->nextBtn);

    vLayout->addStretch();
    vLayout->addLayout(hLayout);
}

void AlarmInfoWindow::showEvent(QShowEvent *ev)
{
    updateData();
    Window::showEvent(ev);
}

void AlarmInfoWindow::_onBtnRelease()
{
    Button *btn = qobject_cast<Button *>(sender());
    AlarmInfoWindowPrivate::pageButton index =
            static_cast<AlarmInfoWindowPrivate::pageButton>(btn->property("Item").toInt());
    switch (index)
    {
    case AlarmInfoWindowPrivate::PAGE_BUTTON_PREV:
    {
        if (d_ptr->hasPrevPage())
        {
            d_ptr->curPage--;
            d_ptr->loadOption();
        }
        break;
    }
    case AlarmInfoWindowPrivate::PAGE_BUTTON_NEXT:
    {
        if (d_ptr->hasNextPage())
        {
            d_ptr->curPage++;
            d_ptr->loadOption();
        }
        break;
    }
    default:
        break;
    }
}

void AlarmInfoWindowPrivate::loadOption()
{
    AlarmInfoNode node;
    QList<AlarmInfoNode> highnode;
    QList<AlarmInfoNode> midnode;
    QList<AlarmInfoNode> lownode;
    QList<AlarmInfoNode> promptnode;

    for (int i = totalList - 1; i >= 0; --i)
    {
        alarmIndicator.getAlarmInfo(i, node);
        if (node.alarmType != alarmType)
        {
            continue;
        }

        switch (node.alarmPriority)
        {
            case ALARM_PRIO_HIGH:
                highnode.append(node);
                break;
            case ALARM_PRIO_MED:
                midnode.append(node);
                break;
            case ALARM_PRIO_LOW:
                lownode.append(node);
                break;
            default:
                promptnode.append(node);
                break;
        }
    }

    highnode.append(midnode);
    highnode.append(lownode);
    highnode.append(promptnode);

    int start = 0, end = 0;
    int count = highnode.count();

    // 计算当前页码和总页数
    int pageTemp = (0 == count % LISTVIEW_MAX_VISIABLE_SIZE) ? (count / LISTVIEW_MAX_VISIABLE_SIZE)
                                                              : (count / LISTVIEW_MAX_VISIABLE_SIZE + 1);
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

    QStringList alarmList;
    for (int i = start; i < end; ++i)
    {
        if (i < count)
        {
            node = highnode.at(i);
            alarmList.append(trs(node.alarmMessage));
        }
    }

    dataModel->setStringList(alarmList);
    highnode.clear();
    midnode.clear();
    lownode.clear();
    promptnode.clear();

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
