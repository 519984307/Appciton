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
#include "ComboBox.h"
#include "MoveButton.h"
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
#include "FontManager.h"
#include "ColorManager.h"
#include <QScrollBar>
#include "RecorderManager.h"
#include "EventPageGenerator.h"
#include "IConfig.h"
#include "EventWaveSetWindow.h"
#include "DataStorageDefine.h"
#include "TableViewItemDelegate.h"
#include "MessageBox.h"

class EventWindowPrivate
{
public:
    EventWindowPrivate()
        : eventTable(NULL), model(NULL), upPageBtn(NULL),
          downPageBtn(NULL), typeCbo(NULL), levelCbo(NULL),
          infoWidget(NULL), trendListWidget(NULL), waveWidget(NULL),
          eventListBtn(NULL), coordinateMoveBtn(NULL), eventMoveBtn(NULL),
          printBtn(NULL),
          setBtn(NULL), upParamBtn(NULL), downParamBtn(NULL),
          tableWidget(NULL), chartWidget(NULL), stackLayout(NULL),
          backend(NULL), curParseIndex(0), eventNum(0), curDisplayEventNum(0),
          curListScroller(0), isHistory(false)
    {
        backend = eventStorageManager.backend();
        curEventType = EventAll;
        curEventLevel = EVENT_LEVEL_ALL;
    }

    // prase the event data from the backend
    bool parseEventData(int dataIndex)
    {
        ctx.reset();
        // clear the wave with empyt wave segments
        waveWidget->setWaveSegments(ctx.waveSegments);
        return ctx.parse(backend, dataIndex);
    }

    /**
     * @brief loadEventData 载入事件回顾数据
     */
    void loadEventData();

    /**
     * @brief levelToPriority 事件等级对应报警等级
     * @return
     */
    AlarmPriority levelToPriority(EventLevel);

    /**
     * @brief eventInfoUpdate   事件信息窗口刷新
     * @param curRow    当前选中行数
     */
    void eventInfoUpdate(int curRow);

    /**
     * @brief eventTrendUpdate 事件趋势数据窗口刷新
     */
    void eventTrendUpdate(void);

    /**
     * @brief eventWaveUpdate 事件波形数据窗口刷新
     */
    void eventWaveUpdate(void);
public:
    TableView *eventTable;
    EventReviewModel *model;
    Button *upPageBtn;
    Button *downPageBtn;
    ComboBox *typeCbo;
    ComboBox *levelCbo;

    EventInfoWidget *infoWidget;
    QListWidget *trendListWidget;
    EventWaveWidget *waveWidget;
    Button *eventListBtn;
    MoveButton *coordinateMoveBtn;
    MoveButton *eventMoveBtn;
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
    int eventNum;                       // 存储总事件数
    int curDisplayEventNum;             // 当前显示事件数
    int curListScroller;
    EventType curEventType;
    EventLevel curEventLevel;
    QList<int> dataIndex;               // 当前选中事件项对应的数据所在索引

    bool isHistory;                     // 历史回顾标志
    QString historyDataPath;            // 历史数据路径
};

EventWindow *EventWindow::getInstance()
{
    static EventWindow *instance = NULL;
    if (!instance)
    {
        instance = new EventWindow;
    }
    return instance;
}

EventWindow::~EventWindow()
{
}

void EventWindow::setWaveSpeed(int speed)
{
    d_ptr->waveWidget->setSweepSpeed((EventWaveWidget::SweepSpeed)speed);
}

void EventWindow::setWaveGain(int gain)
{
    d_ptr->waveWidget->setGain((ECGEventGain)gain);
}

void EventWindow::setHistoryDataPath(QString path)
{
    d_ptr->historyDataPath = path;
}

void EventWindow::setHistoryData(bool flag)
{
    // 动态内存释放
    if (d_ptr->isHistory)
    {
        delete d_ptr->backend;
    }

    d_ptr->isHistory = flag;
    if ((d_ptr->historyDataPath != "") && d_ptr->isHistory)
    {
        d_ptr->backend =  StorageManager::open(d_ptr->historyDataPath + EVENT_DATA_FILE_NAME, QIODevice::ReadOnly);
    }
    else
    {
        d_ptr->backend = eventStorageManager.backend();
    }
}

void EventWindow::findEventIndex(SubParamID id, unsigned time)
{
    int eventNum = d_ptr->backend->getBlockNR();
    int index = -1;
    for (int i = eventNum - 1; i >= 0; i --)
    {
        if (d_ptr->parseEventData(i))
        {
            if (d_ptr->ctx.infoSegment->timestamp == time &&
                    d_ptr->ctx.almSegment->subParamID == id)
            {
                index = i;
            }
        }
    }
    int pos = -1;
    for (int j = 0; j < d_ptr->dataIndex.count(); j++)
    {
        if (d_ptr->dataIndex.at(j) == index)
        {
            pos = j;
        }
    }
    if (pos == -1)
    {
        this->hide();
        MessageBox messageBox(trs("Prompt"), trs("SelectAlarmNotExist"), false);
        messageBox.exec();
    }
    else
    {
        waveInfoReleased(pos);
    }
}

void EventWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);

    d_ptr->stackLayout->setCurrentIndex(0);
    d_ptr->loadEventData();
    if (d_ptr->eventTable->model()->rowCount() == 0)
    {
        d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);
    }
    else
    {
        d_ptr->eventTable->setFocusPolicy(Qt::StrongFocus);
    }

    int curScroller = d_ptr->eventTable->verticalScrollBar()->value();
    int maxValue = d_ptr->eventTable->verticalScrollBar()->maximum();
    bool hasBtn = curScroller > 0;
    d_ptr->upPageBtn->setEnabled(hasBtn);
    hasBtn = curScroller < maxValue;
    d_ptr->downPageBtn->setEnabled(hasBtn);
}

void EventWindow::waveInfoReleased(QModelIndex index)
{
    waveInfoReleased(index.row());
}

void EventWindow::waveInfoReleased(int index)
{
    d_ptr->stackLayout->setCurrentIndex(1);
    if (!d_ptr->backend->getBlockNR())
    {
        return;
    }

    d_ptr->eventTable->selectRow(index);
    d_ptr->parseEventData(d_ptr->dataIndex.at(index));
    d_ptr->eventInfoUpdate(index);
    d_ptr->eventTrendUpdate();
    d_ptr->eventWaveUpdate();
    d_ptr->eventListBtn->setFocus();
}

void EventWindow::eventTypeSelect(int index)
{
    d_ptr->curEventType = (EventType)index;
    d_ptr->loadEventData();
    if (d_ptr->dataIndex.count())
    {
        d_ptr->eventTable->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);
    }
}

void EventWindow::eventLevelSelect(int index)
{
    d_ptr->curEventLevel = (EventLevel)index;
    d_ptr->loadEventData();
    if (d_ptr->dataIndex.count())
    {
        d_ptr->eventTable->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);
    }
}

void EventWindow::upPageReleased()
{
    int maxValue = d_ptr->eventTable->verticalScrollBar()->maximum();
    d_ptr->curListScroller = d_ptr->eventTable->verticalScrollBar()->value();
    if (d_ptr->curListScroller > 0)
    {
        QScrollBar *scrollBar = d_ptr->eventTable->verticalScrollBar();
        int positon = d_ptr->curListScroller - (maxValue * 15) / (d_ptr->curDisplayEventNum - 15);
        scrollBar->setSliderPosition(positon);
    }

    // 上下翻页事件按钮使能
    int curScroller = d_ptr->eventTable->verticalScrollBar()->value();
    bool hasBtn = curScroller > 0;
    d_ptr->upPageBtn->setEnabled(hasBtn);
    hasBtn = curScroller < maxValue;
    d_ptr->downPageBtn->setEnabled(hasBtn);
}

void EventWindow::downPageReleased()
{
    int maxValue = d_ptr->eventTable->verticalScrollBar()->maximum();
    d_ptr->curListScroller = d_ptr->eventTable->verticalScrollBar()->value();
    if (d_ptr->curListScroller < maxValue && d_ptr->curDisplayEventNum != 15)
    {
        QScrollBar *scrollBar = d_ptr->eventTable->verticalScrollBar();
        int positon = d_ptr->curListScroller + (maxValue * 15) / (d_ptr->curDisplayEventNum - 15);
        scrollBar->setSliderPosition(positon);
    }

    // 上下翻页事件按钮使能
    int curScroller = d_ptr->eventTable->verticalScrollBar()->value();
    bool hasBtn = curScroller > 0;
    d_ptr->upPageBtn->setEnabled(hasBtn);
    hasBtn = curScroller < maxValue;
    d_ptr->downPageBtn->setEnabled(hasBtn);
    if (!hasBtn)
    {
        d_ptr->upPageBtn->setFocus();
    }
}

void EventWindow::eventListReleased()
{
    d_ptr->stackLayout->setCurrentIndex(0);
}

void EventWindow::leftMoveCoordinate()
{
    int durationBefore;
    int durationAfter;
    Config &conf =  configManager.getCurConfig();
    conf.getNumValue("Event|WaveLengthBefore", durationBefore);
    conf.getNumValue("Event|WaveLengthAfter", durationAfter);
    EventWaveWidget::SweepSpeed speed;
    speed = d_ptr->waveWidget->getSweepSpeed();
    int medSecond = d_ptr->waveWidget->getCurrentWaveMedSecond();
    switch (speed)
    {
    case EventWaveWidget::SWEEP_SPEED_62_5:
        if (medSecond == -durationBefore + 8)
        {
            return;
        }
        medSecond--;
        break;
    case EventWaveWidget::SWEEP_SPEED_125:
        if (medSecond == -durationBefore + 4)
        {
            return;
        }
        medSecond--;
        break;
    case EventWaveWidget::SWEEP_SPEED_250:
        if (medSecond == -durationBefore + 2)
        {
            return;
        }
        medSecond--;
        break;
    case EventWaveWidget::SWEEP_SPEED_500:
        if (medSecond == -durationBefore + 1)
        {
            return;
        }
        medSecond--;
        break;
    default:
        break;
    }
    d_ptr->waveWidget->setWaveMedSecond(medSecond);
}

void EventWindow::rightMoveCoordinate()
{
    int durationBefore;
    int durationAfter;
    Config &conf =  configManager.getCurConfig();
    conf.getNumValue("Event|WaveLengthBefore", durationBefore);
    conf.getNumValue("Event|WaveLengthAfter", durationAfter);
    EventWaveWidget::SweepSpeed speed;
    speed = d_ptr->waveWidget->getSweepSpeed();
    int medSecond = d_ptr->waveWidget->getCurrentWaveMedSecond();
    switch (speed)
    {
    case EventWaveWidget::SWEEP_SPEED_62_5:
        if (medSecond == durationAfter - 8)
        {
            return;
        }
        medSecond++;
        break;
    case EventWaveWidget::SWEEP_SPEED_125:
        if (medSecond == durationAfter - 4)
        {
            return;
        }
        medSecond++;
        break;
    case EventWaveWidget::SWEEP_SPEED_250:
        if (medSecond == durationAfter - 2)
        {
            return;
        }
        medSecond++;
        break;
    case EventWaveWidget::SWEEP_SPEED_500:
        if (medSecond == durationAfter - 1)
        {
            return;
        }
        medSecond++;
        break;
    default:
        break;
    }
    d_ptr->waveWidget->setWaveMedSecond(medSecond);
}

void EventWindow::leftMoveEvent()
{
    int curIndex = d_ptr->eventTable->currentIndex().row();
    if (curIndex != 0)
    {
        curIndex--;
        d_ptr->eventTable->selectRow(curIndex);
    }
    if (!d_ptr->backend->getBlockNR())
    {
        return;
    }

    if (curIndex >= d_ptr->dataIndex.count())
    {
        return;
    }

    d_ptr->parseEventData(d_ptr->dataIndex.at(curIndex));
    d_ptr->eventInfoUpdate(curIndex);
    d_ptr->eventTrendUpdate();
    d_ptr->eventWaveUpdate();
}

void EventWindow::rightMoveEvent()
{
    int curIndex = d_ptr->eventTable->currentIndex().row();
    if (curIndex != d_ptr->curDisplayEventNum - 1)
    {
        curIndex++;
        d_ptr->eventTable->selectRow(curIndex);
    }
    if (!d_ptr->backend->getBlockNR())
    {
        return;
    }

    d_ptr->parseEventData(d_ptr->dataIndex.at(curIndex));
    d_ptr->eventInfoUpdate(curIndex);
    d_ptr->eventTrendUpdate();
    d_ptr->eventWaveUpdate();
}

void EventWindow::printRelease()
{
    int curIndex = d_ptr->eventTable->currentIndex().row();
    if (curIndex < d_ptr->dataIndex.size() &&
            curIndex >= 0)
    {
        recorderManager.addPageGenerator(new EventPageGenerator(d_ptr->backend, d_ptr->dataIndex.at(curIndex)));
    }
}

void EventWindow::setReleased()
{
    windowManager.showWindow(&eventWaveSetWindow, WindowManager::ShowBehaviorModal);
}

void EventWindow::upReleased()
{
    int maxValue = d_ptr->trendListWidget->verticalScrollBar()->maximum();
    int curScroller = d_ptr->trendListWidget->verticalScrollBar()->value();
    if (curScroller > 0)
    {
        QScrollBar *scrollBar = d_ptr->trendListWidget->verticalScrollBar();
        int position = curScroller - (maxValue * 4) / (d_ptr->trendListWidget->count() - 4);
        scrollBar->setSliderPosition(position);
    }

    // 上下翻页参数按钮使能
    curScroller = d_ptr->trendListWidget->verticalScrollBar()->value();
    bool hasBtn = curScroller > 0;
    d_ptr->upParamBtn->setEnabled(hasBtn);
    hasBtn = curScroller < maxValue;
    d_ptr->downParamBtn->setEnabled(hasBtn);
}

void EventWindow::downReleased()
{
    int maxValue = d_ptr->trendListWidget->verticalScrollBar()->maximum();
    int curScroller = d_ptr->trendListWidget->verticalScrollBar()->value();
    if (curScroller < maxValue && d_ptr->trendListWidget->count() != 4)
    {
        QScrollBar *scrollBar = d_ptr->trendListWidget->verticalScrollBar();
        int position = curScroller + (maxValue * 4) / (d_ptr->trendListWidget->count() - 4);
        scrollBar->setSliderPosition(position);
    }

    // 上下翻页参数按钮使能
    curScroller = d_ptr->trendListWidget->verticalScrollBar()->value();
    bool hasBtn = curScroller > 0;
    d_ptr->upParamBtn->setEnabled(hasBtn);
    hasBtn = curScroller < maxValue;
    d_ptr->downParamBtn->setEnabled(hasBtn);
    if (!hasBtn)
    {
        d_ptr->upParamBtn->setFocus();
    }
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
    horizontalHeader->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->eventTable->verticalHeader()->setVisible(false);
    d_ptr->eventTable->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->eventTable->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->eventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ptr->eventTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->eventTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->eventTable->setShowGrid(false);
    d_ptr->model = new EventReviewModel();
    d_ptr->eventTable->setModel(d_ptr->model);
    connect(d_ptr->eventTable, SIGNAL(clicked(QModelIndex)), this, SLOT(waveInfoReleased(QModelIndex)));
    connect(d_ptr->eventTable, SIGNAL(rowClicked(int)), this, SLOT(waveInfoReleased(int)));

    QLabel *typeLabel = new QLabel(trs("Type"));
    d_ptr->typeCbo = new ComboBox();
    for (int i = 0; i < EventTypeMax - 1; i ++)
    {
        d_ptr->typeCbo->addItem(trs(EventDataSymbol::convert((EventType)i)));
    }
    connect(d_ptr->typeCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(eventTypeSelect(int)));

    QLabel *levelLabel = new QLabel(trs("Level"));
    d_ptr->levelCbo = new ComboBox();
    for (int i = 0; i < EVENT_LEVEL_NR; i ++)
    {
        d_ptr->levelCbo->addItem(trs(EventDataSymbol::convert((EventLevel)i)));
    }
    connect(d_ptr->levelCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(eventLevelSelect(int)));

    d_ptr->upPageBtn = new Button("", QIcon("/usr/local/nPM/icons/up.png"));
    d_ptr->upPageBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->upPageBtn, SIGNAL(released()), this, SLOT(upPageReleased()));

    d_ptr->downPageBtn = new Button("", QIcon("/usr/local/nPM/icons/down.png"));
    d_ptr->downPageBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->downPageBtn, SIGNAL(released()), this, SLOT(downPageReleased()));

    QHBoxLayout *hTableLayout = new QHBoxLayout();
    hTableLayout->addStretch(1);
    hTableLayout->addWidget(typeLabel, 1);
    hTableLayout->addWidget(d_ptr->typeCbo, 3);
    hTableLayout->addStretch(1);
    hTableLayout->addWidget(levelLabel, 1);
    hTableLayout->addWidget(d_ptr->levelCbo, 3);
    hTableLayout->addStretch(1);
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
    d_ptr->trendListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->trendListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    hLayout->addWidget(d_ptr->trendListWidget, 1);
    hLayout->setSpacing(0);

    d_ptr->eventListBtn = new Button(trs("EventList"));
    d_ptr->eventListBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->eventListBtn, SIGNAL(released()), this, SLOT(eventListReleased()));

    d_ptr->coordinateMoveBtn = new MoveButton(trs("Time"));
    d_ptr->coordinateMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->coordinateMoveBtn, SIGNAL(leftMove()), this, SLOT(leftMoveCoordinate()));
    connect(d_ptr->coordinateMoveBtn, SIGNAL(rightMove()), this, SLOT(rightMoveCoordinate()));

    d_ptr->eventMoveBtn = new MoveButton(trs("Event"));
    d_ptr->eventMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->eventMoveBtn, SIGNAL(leftMove()), this, SLOT(leftMoveEvent()));
    connect(d_ptr->eventMoveBtn, SIGNAL(rightMove()), this, SLOT(rightMoveEvent()));

    d_ptr->printBtn = new Button(trs("Print"));
    d_ptr->printBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->printBtn, SIGNAL(released()), this, SLOT(printRelease()));

    d_ptr->setBtn = new Button(trs("Set"));
    d_ptr->setBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->setBtn, SIGNAL(released()), this, SLOT(setReleased()));

    d_ptr->upParamBtn = new Button("", QIcon("/usr/local/nPM/icons/up.png"));
    d_ptr->upParamBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->upParamBtn, SIGNAL(released()), this, SLOT(upReleased()));

    d_ptr->downParamBtn = new Button("", QIcon("/usr/local/nPM/icons/down.png"));
    d_ptr->downParamBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->downParamBtn, SIGNAL(released()), this, SLOT(downReleased()));

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(d_ptr->eventListBtn, 2);
    btnLayout->addWidget(d_ptr->coordinateMoveBtn, 2);
    btnLayout->addWidget(d_ptr->eventMoveBtn, 2);
    btnLayout->addWidget(d_ptr->printBtn, 2);
    btnLayout->addWidget(d_ptr->setBtn, 2);
    btnLayout->addWidget(d_ptr->upParamBtn, 1);
    btnLayout->addWidget(d_ptr->downParamBtn, 1);

    QVBoxLayout *vWaveLayout = new QVBoxLayout();
    vWaveLayout->addWidget(d_ptr->infoWidget, 1);
    vWaveLayout->addLayout(hLayout, 15);
    vWaveLayout->addLayout(btnLayout);
    vWaveLayout->setSpacing(0);

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

            if (ctx.infoSegment->type != curEventType && curEventType != EventAll)
            {
                continue;
            }
            t = ctx.infoSegment->timestamp;
            // 事件时间
            timeDate.getDate(t, dateStr, true);
            timeDate.getTime(t, timeStr, true);
            QString timeItemStr = dateStr + " " + timeStr;

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
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            case EventCodeMarker:
            {
                infoStr = (QString)ctx.codeMarkerSegment->codeName;
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            case EventRealtimePrint:
            {
                infoStr = trs("RealtimePrintSegment");
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            case EventNIBPMeasurement:
            {
                infoStr = trs("NibpMeasurement");
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            case EventWaveFreeze:
            {
                infoStr = trs("WaveFreeze");
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            default:
                break;
            }
            dataIndex.append(i);
        }
    }
    curDisplayEventNum = timeList.count();
    model->updateEvent(timeList, eventList);
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

void EventWindowPrivate::eventInfoUpdate(int curRow)
{
    QString infoStr;
    QString timeInfoStr;
    QString indexStr;

    switch (ctx.infoSegment->type)
    {
    case EventPhysiologicalAlarm:
    {
        SubParamID subId = (SubParamID)(ctx.almSegment->subParamID);
        AlarmLimitIFace *alarmLimit = alertor.getAlarmLimitIFace(subId);
        unsigned char alarmId = ctx.almSegment->alarmType;
        unsigned char alarmInfo = ctx.almSegment->alarmInfo;
        AlarmPriority priority;
        if (alarmLimit)
        {
            priority = alarmLimit->getAlarmPriority(alarmId);
        }
        else
        {
            return;
        }

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
        else
        {
            infoStr = "";
        }

        ParamID paramId = paramInfo.getParamID(subId);
        infoStr += " ";
        infoStr += trs(Alarm::getPhyAlarmMessage(paramId,
                       alarmId,
                       alarmInfo & 0x1));

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
        break;
    }
    case EventCodeMarker:
    {
        infoStr = (QString)ctx.codeMarkerSegment->codeName;
        break;
    }
    case EventRealtimePrint:
    {
        break;
    }
    case EventNIBPMeasurement:
    {
        break;
    }
    case EventWaveFreeze:
    {
        break;
    }
    default:
        break;
    }


    unsigned t = 0;
    QString timeStr;
    QString dateStr;
    t = ctx.infoSegment->timestamp;
    timeDate.getDate(t, dateStr, true);
    timeDate.getTime(t, timeStr, true);
    timeInfoStr = dateStr + " " + timeStr;

    indexStr = QString::number(curRow + 1) + "/" + QString::number(curDisplayEventNum);

    infoWidget->loadDataInfo(infoStr, timeInfoStr, indexStr);
}

void EventWindowPrivate::eventTrendUpdate()
{
    trendListWidget->clear();
    QListWidgetItem *item;
    SubParamID subId;
    QColor color;
    QFont valueFont;
    QFont titleFont = fontManager.textFont(20);
    QFont unitFont = fontManager.textFont(15);

    QString sys;
    QString dia;
    QString map;
    QString et;
    QString fi;
    QString valueStr;
    QString titleStr;
    int paramNum = ctx.trendSegment->trendValueNum;
    for (int i = 0; i < paramNum; i ++)
    {
        QString dataStr;
        if (ctx.trendSegment->values[i].value == InvData())
        {
            dataStr = "-.-";
        }
        else
        {
            dataStr = QString::number(ctx.trendSegment->values[i].value);
        }
        subId = (SubParamID)ctx.trendSegment->values[i].subParamId;
        switch (subId)
        {
        case SUB_PARAM_NIBP_SYS:
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP2_SYS:
            sys = dataStr;
            continue;
        case SUB_PARAM_NIBP_DIA:
        case SUB_PARAM_ART_DIA:
        case SUB_PARAM_PA_DIA:
        case SUB_PARAM_AUXP1_DIA:
        case SUB_PARAM_AUXP2_DIA:
            dia = dataStr;
            continue;
        case SUB_PARAM_NIBP_MAP:
        case SUB_PARAM_ART_MAP:
        case SUB_PARAM_PA_MAP:
        case SUB_PARAM_AUXP1_MAP:
        case SUB_PARAM_AUXP2_MAP:
            map = dataStr;
            valueStr = sys + "/" + dia + "(" + map + ")";
            titleStr = paramInfo.getSubParamName(subId);
            titleStr = titleStr.left(titleStr.length() - 4);
            valueFont = fontManager.numFont(25);
            break;
        case SUB_PARAM_ART_PR:
        case SUB_PARAM_PA_PR:
        case SUB_PARAM_CVP_PR:
        case SUB_PARAM_LAP_PR:
        case SUB_PARAM_RAP_PR:
        case SUB_PARAM_ICP_PR:
        case SUB_PARAM_AUXP1_PR:
        case SUB_PARAM_AUXP2_PR:
            continue;
        case SUB_PARAM_ETCO2:
        case SUB_PARAM_ETN2O:
        case SUB_PARAM_ETAA1:
        case SUB_PARAM_ETAA2:
        case SUB_PARAM_ETO2:
            et = dataStr;
            continue;
        case SUB_PARAM_FICO2:
        case SUB_PARAM_FIN2O:
        case SUB_PARAM_FIAA1:
        case SUB_PARAM_FIAA2:
        case SUB_PARAM_FIO2:
            fi = dataStr;
            valueStr = et + "/" + fi;
            titleStr = paramInfo.getSubParamName(subId);
            titleStr = titleStr.right(titleStr.length() - 2);
            valueFont = fontManager.numFont(31);
            break;
        default:
            valueStr = dataStr;
            titleStr = paramInfo.getSubParamName(subId);
            valueFont = fontManager.numFont(37);
            break;
        }

        item = new QListWidgetItem(trendListWidget);
        item->setData(EventTrendItemDelegate::ValueFontRole, qVariantFromValue(valueFont));
        item->setData(EventTrendItemDelegate::TitleFontRole, qVariantFromValue(titleFont));
        item->setData(EventTrendItemDelegate::UnitFontRole, qVariantFromValue(unitFont));

        item->setData(EventTrendItemDelegate::ValueTextRole, valueStr);
        item->setData(EventTrendItemDelegate::TitleTextRole, titleStr);
        item->setData(EventTrendItemDelegate::UnitTextRole, Unit::getSymbol(paramInfo.getUnitOfSubParam(subId)));


        item->setData(EventTrendItemDelegate::TrendAlarmRole, ctx.trendSegment->values[i].alarmFlag);
        color = colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID(subId)));
        if (color != QColor(0, 0, 0))
        {
            item->setTextColor(color);
        }
        else
        {
            item->setTextColor(Qt::red);
        }
        item->setFlags(Qt::NoItemFlags);
    }
}

void EventWindowPrivate::eventWaveUpdate()
{
    waveWidget->setWaveSegments(ctx.waveSegments);
}
