/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/1
 **/

#include "EventWindow.h"
#include "TableView.h"
#include "Button.h"
#include "DropList.h"
#include "EventInfoWidget.h"
#include <QListWidget>
#include "EventWaveWidget.h"
#include <QStackedLayout>
#include "EventDataParseContext.h"
#include "EventReviewModel.h"
#include "TableHeaderView.h"
#include "EventDataSymbol.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "EventStorageManager.h"
#include "EventTrendItemDelegate.h"
#include "WindowManager.h"
#include "AlarmConfig.h"
#include "AlarmParamIFace.h"
#include "TimeDate.h"
#include "Alarm.h"
#include "ParamInfo.h"
#include "IBPSymbol.h"
#include "IBPParam.h"
#include "ParamManager.h"
#include "Utility.h"

EventWindow *EventWindow::selfObj = NULL;

class EventWindowPrivate
{
public:
    EventWindowPrivate()
        : eventTable(NULL), model(NULL), upPageBtn(NULL),
          downPageBtn(NULL), typeDpt(NULL), levelDpt(NULL),
          infoWidget(NULL), trendListWidget(NULL), waveWidget(NULL),
          eventListBtn(NULL), leftCoordinateBtn(NULL), rightCoordinateBtn(NULL),
          prvEventBtn(NULL), nextEventBtn(NULL), printBtn(NULL),
          setBtn(NULL), upParamBtn(NULL), downParamBtn(NULL),
          tableWidget(NULL), chartWidget(NULL), stackLayout(NULL),
          backend(NULL), curParseIndex(0), eventNum(0),
          curListScroller(0), isHistory(false)
    {
        backend = eventStorageManager.backend();
    }

    // prase the event data from the backend
    bool parseEventData(int dataIndex)
    {
        ctx.reset();
        // clear the wave with empyt wave segments
        waveWidget->setWaveSegments(ctx.waveSegments);
        return ctx.parse(backend, dataIndex);
    }

    void loadEventData();
    AlarmPriority levelToPriority(EventLevel);
public:
    TableView *eventTable;
    EventReviewModel *model;
    Button *upPageBtn;
    Button *downPageBtn;
    DropList *typeDpt;
    DropList *levelDpt;

    EventInfoWidget *infoWidget;
    QListWidget *trendListWidget;
    EventWaveWidget *waveWidget;
    Button *eventListBtn;
    Button *leftCoordinateBtn;
    Button *rightCoordinateBtn;
    Button *prvEventBtn;
    Button *nextEventBtn;
    Button *printBtn;
    Button *setBtn;
    Button *upParamBtn;
    Button *downParamBtn;

    QWidget *tableWidget;
    QWidget *chartWidget;
    QStackedLayout *stackLayout;

    EventDataPraseContext ctx;
    IStorageBackend *backend;
    int curParseIndex;
    int eventNum;                       // 总事件数
    int curListScroller;
    EventType curEventType;
    EventLevel curEventLevel;
    QList<int> dataIndex;               // 当前选中事件项对应的数据所在索引

    bool isHistory;                     // 历史回顾标志
    QString historyDataPath;            // 历史数据路径
};

EventWindow::~EventWindow()
{
}

void EventWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);

    d_ptr->stackLayout->setCurrentIndex(0);
    d_ptr->loadEventData();
}

EventWindow::EventWindow()
    : Window(),
      d_ptr(new EventWindowPrivate())
{
    setWindowTitle(trs("EventReview"));

    // 事件表格窗口
    d_ptr->eventTable = new TableView();
    TableHeaderView *horizontalHeader = new TableHeaderView(Qt::Horizontal);
    d_ptr->eventTable->setHorizontalHeader(horizontalHeader);
    horizontalHeader->setResizeMode(QHeaderView::Stretch);
    d_ptr->eventTable->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->eventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ptr->eventTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d_ptr->eventTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);

    d_ptr->typeDpt = new DropList(trs("Type"));
    for (int i = 0; i < EventTypeMax; i ++)
    {
        d_ptr->typeDpt->addItem(trs(EventDataSymbol::convert((EventType)i)));
    }

    d_ptr->levelDpt = new DropList(trs("Level"));
    for (int i = 0; i < EVENT_LEVEL_NR; i ++)
    {
        d_ptr->levelDpt->addItem(trs(EventDataSymbol::convert((EventLevel)i)));
    }


    d_ptr->upPageBtn = new Button("", QIcon("/usr/local/nPM/icons/up.png"));
    d_ptr->upPageBtn->setButtonStyle(Button::ButtonIconOnly);

    d_ptr->downPageBtn = new Button("", QIcon("/usr/local/nPM/icons/down.png"));
    d_ptr->downPageBtn->setButtonStyle(Button::ButtonIconOnly);

    QHBoxLayout *hTableLayout = new QHBoxLayout();
    hTableLayout->addWidget(d_ptr->typeDpt, 2);
    hTableLayout->addWidget(d_ptr->levelDpt, 2);
    hTableLayout->addWidget(d_ptr->upPageBtn, 1);
    hTableLayout->addWidget(d_ptr->downPageBtn, 1);

    QVBoxLayout *vTableLayout = new QVBoxLayout();
    vTableLayout->addWidget(d_ptr->eventTable);
    vTableLayout->addLayout(hTableLayout);

    d_ptr->tableWidget = new QWidget();
    d_ptr->tableWidget->setLayout(vTableLayout);

    // 事件波形窗口
    d_ptr->infoWidget = new EventInfoWidget();

    QHBoxLayout *hLayout = new QHBoxLayout();
    d_ptr->waveWidget = new EventWaveWidget();
    d_ptr->waveWidget->setWaveSegments(d_ptr->ctx.waveSegments);
    hLayout->addWidget(d_ptr->waveWidget, 3);

    d_ptr->trendListWidget = new QListWidget();
    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    d_ptr->trendListWidget->setPalette(pal);
    d_ptr->trendListWidget->setFrameShape(QFrame::NoFrame);
    d_ptr->trendListWidget->setItemDelegate(new EventTrendItemDelegate(d_ptr->trendListWidget));
    d_ptr->trendListWidget->setResizeMode(QListWidget::Adjust);
    d_ptr->trendListWidget->setFocusPolicy(Qt::NoFocus);
    hLayout->addWidget(d_ptr->trendListWidget, 1);

    d_ptr->eventListBtn = new Button(trs("EventList"));
    d_ptr->eventListBtn->setButtonStyle(Button::ButtonTextOnly);

    d_ptr->leftCoordinateBtn = new Button("", QIcon("/usr/local/nPM/icons/doubleleft.png"));
    d_ptr->leftCoordinateBtn->setButtonStyle(Button::ButtonIconOnly);

    d_ptr->rightCoordinateBtn = new Button("", QIcon("/usr/local/nPM/icons/doubleright.png"));
    d_ptr->rightCoordinateBtn->setButtonStyle(Button::ButtonIconOnly);

    d_ptr->prvEventBtn = new Button(trs("PreviousEvent"));
    d_ptr->prvEventBtn->setButtonStyle(Button::ButtonTextOnly);

    d_ptr->nextEventBtn = new Button(trs("NextEvent"));
    d_ptr->nextEventBtn->setButtonStyle(Button::ButtonTextOnly);

    d_ptr->printBtn = new Button(trs("Print"));
    d_ptr->printBtn->setButtonStyle(Button::ButtonTextOnly);

    d_ptr->setBtn = new Button(trs("Set"));
    d_ptr->setBtn->setButtonStyle(Button::ButtonTextOnly);

    d_ptr->upParamBtn = new Button("", QIcon("/usr/local/nPM/icons/up.png"));
    d_ptr->upParamBtn->setButtonStyle(Button::ButtonIconOnly);

    d_ptr->downParamBtn = new Button("", QIcon("/usr/local/nPM/icons/down.png"));
    d_ptr->downParamBtn->setButtonStyle(Button::ButtonIconOnly);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(d_ptr->eventListBtn, 2);
    btnLayout->addWidget(d_ptr->leftCoordinateBtn, 1);
    btnLayout->addWidget(d_ptr->rightCoordinateBtn, 1);
    btnLayout->addWidget(d_ptr->prvEventBtn, 2);
    btnLayout->addWidget(d_ptr->nextEventBtn, 2);
    btnLayout->addWidget(d_ptr->printBtn, 2);
    btnLayout->addWidget(d_ptr->setBtn, 2);
    btnLayout->addWidget(d_ptr->upParamBtn, 1);
    btnLayout->addWidget(d_ptr->downParamBtn, 1);

    QVBoxLayout *vWaveLayout = new QVBoxLayout();
    vWaveLayout->addWidget(d_ptr->infoWidget, 1);
    vWaveLayout->addLayout(hLayout, 15);
    vWaveLayout->addLayout(btnLayout);

    d_ptr->chartWidget = new QWidget();
    d_ptr->chartWidget->setLayout(vWaveLayout);

    d_ptr->stackLayout = new QStackedLayout();
    d_ptr->stackLayout->addWidget(d_ptr->tableWidget);
    d_ptr->stackLayout->addWidget(d_ptr->chartWidget);

    setWindowLayout(d_ptr->stackLayout);

    int width = windowManager.getPopMenuWidth();
    int height = windowManager.getPopMenuHeight();
    setFixedSize(width, height);
}

void EventWindowPrivate::loadEventData()
{
    dataIndex.clear();
    eventNum = backend->getBlockNR();

    unsigned t = 0;
    QString timeStr;
    QString dateStr;
    QString infoStr;
    unsigned char alarmInfo;
    SubParamID subId;
    unsigned char alarmId;
    AlarmPriority priority;
    AlarmPriority curPriority;
    AlarmLimitIFace *alarmLimit;
    QList<QString> timeList;
    QList<QString> eventList;
    for (int i = eventNum - 1; i >= 0; i --)
    {
        priority = ALARM_PRIO_PROMPT;
        if (parseEventData(i))
        {
            if (ctx.infoSegment->type == EventOxyCRG)
            {
                continue;
            }

            t = ctx.infoSegment->timestamp;
            // 事件时间
            timeDate.getDate(t, dateStr, true);
            timeDate.getTime(t, timeStr, true);
            QString timeItemStr = dateStr + " " + timeStr;
            timeList.append(timeItemStr);

            switch (ctx.infoSegment->type)
            {
            case EventPhysiologicalAlarm:
            {
                subId = (SubParamID)(ctx.almSegment->subParamID);
                alarmId = ctx.almSegment->alarmType;
                alarmInfo = ctx.almSegment->alarmInfo;
                alarmLimit = alertor.getAlarmLimitIFace(subId);
                if (alarmLimit)
                {
                    priority = alarmLimit->getAlarmPriority(alarmId);
                }

                if (curEventType != EventAll)
                {
                    if (curEventType != ctx.infoSegment->type)
                    {
                        continue;
                    }
                }

                if (curEventLevel != EVENT_LEVEL_ALL)
                {
                    curPriority = levelToPriority(curEventLevel);
                    if (curPriority != priority)
                    {
                        continue;
                    }
                }

                // 事件内容
                if (priority == ALARM_PRIO_LOW)
                {
                    infoStr = "*";
                }
                else if (priority == ALARM_PRIO_MED)
                {
                    infoStr = "**";
                }
                else if (priority == ALARM_PRIO_HIGH)
                {
                    infoStr = "***";
                }

                ParamID paramId = paramInfo.getParamID(subId);
                infoStr += " ";

                if (paramId == PARAM_IBP)
                {
                    infoStr += IBPSymbol::convert(ibpParam.getPressureName(subId));
                    infoStr += " ";
                }
                infoStr += trs(Alarm::getPhyAlarmMessage(paramId, alarmId, alarmInfo & 0x1));

                if (!(alarmInfo & 0x1))
                {
                    if (alarmInfo & 0x2)
                    {
                        infoStr += " > ";
                    }
                    else
                    {
                        infoStr += " < ";
                    }
                    UnitType unit = paramManager.getSubParamUnit(paramId, subId);
                    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subId, unit);

                    infoStr += Util::convertToString(ctx.almSegment->alarmLimit, config.scale);
                }
                eventList.append(infoStr);
                break;
            }
            case EventCodeMarker:
            {
                infoStr = (QString)ctx.codeMarkerSegment->codeName;
                eventList.append(infoStr);
                break;
            }
            case EventRealtimePrint:
                // TODO
                break;
            case EventNIBPMeasurement:
                // TODO
                break;
            case EventWaveFreeze:
                // TODO
                break;
            default:
                break;
            }
            dataIndex.append(i);
        }
    }
}

AlarmPriority EventWindowPrivate::levelToPriority(EventLevel level)
{
    if (level == EVENT_LEVEL_LOW)
    {
        return ALARM_PRIO_LOW;
    }
    else if (level == EVENT_LEVEL_MED)
    {
        return ALARM_PRIO_MED;
    }
    else if (level == EVENT_LEVEL_High)
    {
        return ALARM_PRIO_HIGH;
    }
    else if (level == EVENT_LEVEL_HINT)
    {
        return ALARM_PRIO_PROMPT;
    }
    else
    {
        return ALARM_PRIO_PROMPT;
    }
}
