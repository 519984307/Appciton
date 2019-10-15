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
#include "CO2Param.h"
#include "EventListPageGenerator.h"
#include "NIBPSymbol.h"
#include "ThemeManager.h"
#include "PatientManager.h"
#include "LanguageManager.h"

#define TABLE_SPACING               (4)
#define PAGE_ROW_COUNT               7      // 每页多少行
#define STOP_PRINT_TIMEOUT          (100)

class EventWindowPrivate
{
public:
    explicit EventWindowPrivate(EventWindow * const q_ptr)
        : eventTable(NULL), model(NULL), upPageBtn(NULL),
          downPageBtn(NULL), typeCbo(NULL), listPrintBtn(NULL),
          infoWidget(NULL), trendListWidget(NULL), waveWidget(NULL),
          eventListBtn(NULL), coordinateMoveBtn(NULL), eventMoveBtn(NULL),
          printBtn(NULL),
          setBtn(NULL), upParamBtn(NULL), downParamBtn(NULL),
          tableWidget(NULL), chartWidget(NULL), stackLayout(NULL),
          backend(NULL), eventNum(0), curDisplayEventNum(0),
          isHistory(false), q_ptr(q_ptr),
          printTimerId(-1),
          waitTimerId(-1),
          isWait(false),
          timeoutNum(0),
          generator(NULL), ecgGain(3),
          curPage(1), totalPage(1), curIndex(-1)
    {
        backend = eventStorageManager.backend();
        patientInfo = patientManager.getPatientInfo();
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

    /**
     * @brief refreshPageInfo 刷新窗口标题页面信息
     */
    void refreshPageInfo();

    /**
     * @brief updateLevelStatus 更新事件级别状态
     */
    void updateLevelStatus();

    /**
     * @brief eventTypeOrLevelChange 事件类型或事件级别发生改变
     */
    void eventTypeOrLevelChange();

    /**
     * @brief calculationPage 计算当前页和总页数
     */
    void calculationPage();

    /**
     * @brief refreshEventList 刷新事件列表
     */
    void refreshEventList();
public:
    TableView *eventTable;
    EventReviewModel *model;
    Button *upPageBtn;
    Button *downPageBtn;
    ComboBox *typeCbo;
    Button *listPrintBtn;

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
    int eventNum;                       // 存储总事件数
    int curDisplayEventNum;             // 当前显示事件数
    EventType curEventType;
    EventLevel curEventLevel;
    QList<int> dataIndex;               // 当前选中事件项对应的数据所在索引

    bool isHistory;                     // 历史回顾标志
    QString historyDataPath;            // 历史数据路径

    EventWindow *q_ptr;

    int printTimerId;
    int waitTimerId;
    bool isWait;
    int timeoutNum;
    RecordPageGenerator *generator;

    QStringList printList;              // 事件列表打印
    PatientInfo patientInfo;            // 病人信息
    int ecgGain;

    QVector<BlockEntry> blockList;        // 块数据信息
    int curPage;
    int totalPage;
    int curIndex;
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
    d_ptr->ecgGain = gain;
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
        d_ptr->backend = NULL;
    }

    d_ptr->isHistory = flag;
    if ((d_ptr->historyDataPath != "") && d_ptr->isHistory)
    {
        d_ptr->backend =  StorageManager::open(d_ptr->historyDataPath + EVENT_DATA_FILE_NAME, QIODevice::ReadOnly);
        d_ptr->patientInfo = patientManager.getHistoryPatientInfo(d_ptr->historyDataPath + PATIENT_INFO_FILE_NAME);
    }
    else
    {
        d_ptr->backend = eventStorageManager.backend();
        d_ptr->patientInfo = patientManager.getPatientInfo();
    }
}

void EventWindow::findEventIndex(SubParamID id, unsigned time)
{
    Q_UNUSED(id)
    int eventNum = d_ptr->blockList.count();
    int index = -1;
    for (int i = eventNum - 1; i >= 0; i --)
    {
        if (d_ptr->blockList.at(i).extraData == time)
        {
            index = i;
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
    Dialog::showEvent(ev);

    d_ptr->stackLayout->setCurrentIndex(0);
    d_ptr->loadEventData();
    d_ptr->eventTypeOrLevelChange();
    d_ptr->calculationPage();
    d_ptr->refreshEventList();
    if (d_ptr->eventTable->model()->rowCount() == 0)
    {
        d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);
    }
    else
    {
        d_ptr->eventTable->setFocusPolicy(Qt::StrongFocus);
    }

    // 更新打印按键状态
    if (recorderManager.isConnected() && !d_ptr->printList.isEmpty())
    {
        d_ptr->printBtn->setEnabled(true);
        d_ptr->listPrintBtn->setEnabled(true);
    }
    else
    {
        d_ptr->printBtn->setEnabled(false);
        d_ptr->listPrintBtn->setEnabled(false);
    }
}

void EventWindow::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->printTimerId == ev->timerId())
    {
        if (!recorderManager.isPrinting() || d_ptr->timeoutNum == 10)
        {
            if (!recorderManager.isPrinting() && !recorderManager.getPrintStatus())
            {
                recorderManager.addPageGenerator(d_ptr->generator);
            }
            else
            {
                d_ptr->generator->deleteLater();
                d_ptr->generator = NULL;
            }
            killTimer(d_ptr->printTimerId);
            d_ptr->printTimerId = -1;
            d_ptr->timeoutNum = 0;
        }
        d_ptr->timeoutNum++;
    }
    else if (d_ptr->waitTimerId == ev->timerId())
    {
        d_ptr->printTimerId = startTimer(STOP_PRINT_TIMEOUT);
        killTimer(d_ptr->waitTimerId);
        d_ptr->waitTimerId = -1;
        d_ptr->isWait = false;
    }
}

void EventWindow::waveInfoReleased(QModelIndex index)
{
    waveInfoReleased(index.row());
}

void EventWindow::waveInfoReleased(int index)
{
    d_ptr->stackLayout->setCurrentIndex(1);
    setWindowTitle(trs("EventReview"));
    if (!d_ptr->backend->getBlockNR())
    {
        return;
    }

    d_ptr->curIndex = index + (d_ptr->curPage - 1) * PAGE_ROW_COUNT;
    d_ptr->parseEventData(d_ptr->dataIndex.at(d_ptr->curIndex));
    d_ptr->eventInfoUpdate(d_ptr->curIndex);
    d_ptr->eventTrendUpdate();
    d_ptr->eventWaveUpdate();
    d_ptr->eventListBtn->setFocus();
}

void EventWindow::eventTypeSelect(int index)
{
    d_ptr->curEventType = (EventType)index;
    d_ptr->eventTypeOrLevelChange();
    d_ptr->calculationPage();
    d_ptr->refreshEventList();
    if (d_ptr->dataIndex.count())
    {
        d_ptr->eventTable->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);
    }
    // 更新打印按键状态
    if (recorderManager.isConnected() && !d_ptr->printList.isEmpty())
    {
        d_ptr->printBtn->setEnabled(true);
        d_ptr->listPrintBtn->setEnabled(true);
    }
    else
    {
        d_ptr->printBtn->setEnabled(false);
        d_ptr->listPrintBtn->setEnabled(false);
    }
}

void EventWindow::eventListPrintReleased()
{
    RecordPageGenerator *generator = new EventListPageGenerator(d_ptr->printList, d_ptr->patientInfo);
    if (recorderManager.isPrinting() && !d_ptr->isWait)
    {
        if (generator->getPriority() <= recorderManager.getCurPrintPriority())
        {
            generator->deleteLater();
        }
        else
        {
            recorderManager.stopPrint();
            d_ptr->generator = generator;
            d_ptr->waitTimerId = startTimer(2000);
            d_ptr->isWait = true;
        }
    }
    else if (!recorderManager.getPrintStatus())
    {
        recorderManager.addPageGenerator(generator);
    }
    else
    {
        generator->deleteLater();
    }
}

void EventWindow::upPageReleased()
{
    if (d_ptr->curPage > 1)
    {
        d_ptr->curPage--;
    }
    d_ptr->refreshEventList();
    d_ptr->refreshPageInfo();
}

void EventWindow::downPageReleased()
{
    if (d_ptr->curPage < d_ptr->totalPage)
    {
        d_ptr->curPage++;
    }
    d_ptr->refreshEventList();
    d_ptr->refreshPageInfo();
}

void EventWindow::eventListReleased()
{
    d_ptr->refreshEventList();
    d_ptr->refreshPageInfo();
    d_ptr->eventTable->selectRow(d_ptr->curIndex % PAGE_ROW_COUNT);
    d_ptr->stackLayout->setCurrentIndex(0);
}

void EventWindow::leftMoveCoordinate()
{
    int waveLength = d_ptr->ctx.infoSegment->duration_after;
    EventWaveWidget::SweepSpeed speed;
    speed = d_ptr->waveWidget->getSweepSpeed();
    int medSecond = d_ptr->waveWidget->getCurrentWaveMedSecond();
    switch (speed)
    {
    case EventWaveWidget::SWEEP_SPEED_62_5:
        if (medSecond == -waveLength + 8)
        {
            return;
        }
        medSecond--;
        break;
    case EventWaveWidget::SWEEP_SPEED_125:
        if (medSecond == -waveLength + 4)
        {
            return;
        }
        medSecond--;
        break;
    case EventWaveWidget::SWEEP_SPEED_250:
        if (medSecond == -waveLength + 2)
        {
            return;
        }
        medSecond--;
        break;
    case EventWaveWidget::SWEEP_SPEED_500:
        if (medSecond == -waveLength + 1)
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
    int waveLength = d_ptr->ctx.infoSegment->duration_after;
    EventWaveWidget::SweepSpeed speed;
    speed = d_ptr->waveWidget->getSweepSpeed();
    int medSecond = d_ptr->waveWidget->getCurrentWaveMedSecond();
    switch (speed)
    {
    case EventWaveWidget::SWEEP_SPEED_62_5:
        if (medSecond == waveLength - 8)
        {
            return;
        }
        medSecond++;
        break;
    case EventWaveWidget::SWEEP_SPEED_125:
        if (medSecond == waveLength - 4)
        {
            return;
        }
        medSecond++;
        break;
    case EventWaveWidget::SWEEP_SPEED_250:
        if (medSecond == waveLength - 2)
        {
            return;
        }
        medSecond++;
        break;
    case EventWaveWidget::SWEEP_SPEED_500:
        if (medSecond == waveLength - 1)
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
    if (d_ptr->curIndex != 0)
    {
        int prvPage = d_ptr->curIndex / PAGE_ROW_COUNT;
        d_ptr->curIndex--;
        int curPage = d_ptr->curIndex / PAGE_ROW_COUNT;
        // 事件列表翻页判断
        if (prvPage != curPage)
        {
            d_ptr->curPage--;
        }
        if (d_ptr->curIndex >= d_ptr->dataIndex.count())
        {
            return;
        }

        d_ptr->parseEventData(d_ptr->dataIndex.at(d_ptr->curIndex));
        d_ptr->eventInfoUpdate(d_ptr->curIndex);
        d_ptr->eventTrendUpdate();
        d_ptr->eventWaveUpdate();
    }
}

void EventWindow::rightMoveEvent()
{
    if (d_ptr->curIndex != d_ptr->curDisplayEventNum - 1)
    {
        int prvPage = d_ptr->curIndex / PAGE_ROW_COUNT;
        d_ptr->curIndex++;
        int curPage = d_ptr->curIndex / PAGE_ROW_COUNT;
        // 事件列表翻页判断
        if (prvPage != curPage)
        {
            d_ptr->curPage++;
        }
        if (d_ptr->curIndex >= d_ptr->dataIndex.count())
        {
            return;
        }

        d_ptr->parseEventData(d_ptr->dataIndex.at(d_ptr->curIndex));
        d_ptr->eventInfoUpdate(d_ptr->curIndex);
        d_ptr->eventTrendUpdate();
        d_ptr->eventWaveUpdate();
    }
}

void EventWindow::printRelease()
{
    int curIndex = d_ptr->curIndex;
    if (curIndex < d_ptr->dataIndex.size() &&
            curIndex >= 0)
    {
        RecordPageGenerator *gen = new EventPageGenerator(d_ptr->backend, d_ptr->dataIndex.at(curIndex),
                                                          d_ptr->patientInfo, d_ptr->ecgGain);
        if (recorderManager.isPrinting() && !d_ptr->isWait)
        {
            if (gen->getPriority() <= recorderManager.getCurPrintPriority())
            {
                gen->deleteLater();
            }
            else
            {
                recorderManager.stopPrint();
                d_ptr->generator = gen;
                d_ptr->waitTimerId = startTimer(2000);
                d_ptr->isWait = true;
            }
        }
        else if (!recorderManager.getPrintStatus())
        {
            recorderManager.addPageGenerator(gen);
        }
        else
        {
            gen->deleteLater();
        }
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
}

EventWindow::EventWindow()
    : Dialog(),
      d_ptr(new EventWindowPrivate(this))
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
    d_ptr->eventTable->setFixedHeight((PAGE_ROW_COUNT + 1) * d_ptr->model->getHeightHint());
    d_ptr->eventTable->setItemDelegate(new TableViewItemDelegate(this));
    connect(d_ptr->eventTable, SIGNAL(clicked(QModelIndex)), this, SLOT(waveInfoReleased(QModelIndex)));
    connect(d_ptr->eventTable, SIGNAL(rowClicked(int)), this, SLOT(waveInfoReleased(int)));

    QLabel *typeLabel = new QLabel(trs("Type"));
    d_ptr->typeCbo = new ComboBox();
    for (int i = 0; i < EventTypeMax - 1; i ++)
    {
        d_ptr->typeCbo->addItem(trs(EventDataSymbol::convert((EventType)i)));
    }
    connect(d_ptr->typeCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(eventTypeSelect(int)));

    d_ptr->listPrintBtn = new Button(trs("PrintList"));
    d_ptr->listPrintBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->listPrintBtn, SIGNAL(released()), this, SLOT(eventListPrintReleased()));

    QSize iconsize(32, 32);
    d_ptr->upPageBtn = new Button("", themeManger.getIcon(ThemeManager::IconUp, iconsize));
    d_ptr->upPageBtn->setIconSize(iconsize);
    d_ptr->upPageBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->upPageBtn, SIGNAL(released()), this, SLOT(upPageReleased()));

    d_ptr->downPageBtn = new Button("", themeManger.getIcon(ThemeManager::IconDown, iconsize));
    d_ptr->downPageBtn->setIconSize(iconsize);
    d_ptr->downPageBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->downPageBtn, SIGNAL(released()), this, SLOT(downPageReleased()));

    QHBoxLayout *hTableLayout = new QHBoxLayout();
    hTableLayout->addStretch(1);
    hTableLayout->addWidget(typeLabel, 1);
    hTableLayout->addWidget(d_ptr->typeCbo, 6);
    hTableLayout->addStretch(1);
    hTableLayout->addWidget(d_ptr->listPrintBtn, 4);
    hTableLayout->addStretch(1);
    hTableLayout->addWidget(d_ptr->upPageBtn, 2);
    hTableLayout->addWidget(d_ptr->downPageBtn, 2);

    QVBoxLayout *vTableLayout = new QVBoxLayout();
    vTableLayout->addSpacing(TABLE_SPACING);
    vTableLayout->addWidget(d_ptr->eventTable);
    vTableLayout->addSpacing(TABLE_SPACING);
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

    d_ptr->upParamBtn = new Button("", themeManger.getIcon(ThemeManager::IconUp, iconsize));
    d_ptr->upParamBtn->setIconSize(iconsize);
    d_ptr->upParamBtn->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->upParamBtn, SIGNAL(released()), this, SLOT(upReleased()));

    d_ptr->downParamBtn = new Button("", themeManger.getIcon(ThemeManager::IconDown, iconsize));
    d_ptr->downParamBtn->setIconSize(iconsize);
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
    vWaveLayout->setSpacing(0);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(vWaveLayout);
    vLayout->addLayout(btnLayout);

    d_ptr->chartWidget = new QWidget();
    d_ptr->chartWidget->setLayout(vLayout);

    d_ptr->stackLayout = new QStackedLayout();
    d_ptr->stackLayout->addWidget(d_ptr->tableWidget);
    d_ptr->stackLayout->addWidget(d_ptr->chartWidget);

    setWindowLayout(d_ptr->stackLayout);

    int width = windowManager.getPopWindowWidth();
    int height = windowManager.getPopWindowHeight();
    setFixedSize(width, height);
}

void EventWindowPrivate::loadEventData()
{
    blockList.clear();
    blockList = backend->getBlockEntryList();
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
        unsigned char alarmId = ctx.almSegment->alarmType;
        unsigned char alarmInfo = ctx.almSegment->alarmInfo;
        AlarmPriority priority;
        if (alarmInfo & 0x01)   // oneshot 报警事件
        {
            AlarmOneShotIFace *alarmOneShot = alertor.getAlarmOneShotIFace(subId);
            if (alarmOneShot)
            {
                priority = alarmOneShot->getAlarmPriority(alarmId);
            }
        }
        else
        {
            AlarmLimitIFace *alarmLimit = alertor.getAlarmLimitIFace(subId);
            if (alarmLimit)
            {
                priority = alarmLimit->getAlarmPriority(alarmId);
            }
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
        // oneshot 报警
        if (alarmInfo & 0x01)
        {
            // 将参数ID转换为oneshot报警对应的参数ID
            if (paramId == PARAM_DUP_ECG)
            {
                paramId = PARAM_ECG;
            }
            else if (paramId == PARAM_DUP_RESP)
            {
                paramId = PARAM_RESP;
            }
        }
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
        infoStr = trs("RealtimePrintSegment");
        break;
    }
    case EventNIBPMeasurement:
    {
        infoStr = trs("NibpMeasurement");
        if (ctx.measureSegment->measureResult == NIBP_ONESHOT_NONE)
        {
            infoStr = trs("NIBPMEASURE") + trs("ServiceSuccess");
        }
        else
        {
            infoStr = trs(NIBPSymbol::convert((NIBPOneShotType)(ctx.measureSegment->measureResult)));
        }
        break;
    }
    case EventWaveFreeze:
    {
        infoStr = trs("WaveFreeze");
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
    QString t1;
    QString t2;
    QString td;
    QString valueStr;
    QString titleStr;
    bool multiSubParamAlarm = false;
    bool subParamAlarm = false;
    int paramNum = ctx.trendSegment->trendValueNum;
    for (int i = 0; i < paramNum; i ++)
    {
        QString dataStr;
        subId = (SubParamID)ctx.trendSegment->values[i].subParamId;
        UnitType type = paramManager.getSubParamUnit(paramInfo.getParamID(subId), subId);
        if (ctx.trendSegment->values[i].value == InvData())
        {
            dataStr = InvStr();
        }
        else
        {
            if (paramInfo.getParamID(subId) == PARAM_CO2)
            {
                dataStr = Unit::convert(type, UNIT_PERCENT,
                                        ctx.trendSegment->values[i].value / 10.0, co2Param.getBaro());
            }
            else if (paramInfo.getParamID(subId) == PARAM_TEMP)
            {
                dataStr = Unit::convert(type, paramInfo.getUnitOfSubParam(subId), ctx.trendSegment->values[i].value / 10.0);
            }
            else if (paramInfo.getParamID(subId) == PARAM_NIBP)
            {
                dataStr = Unit::convert(type, UNIT_MMHG, ctx.trendSegment->values[i].value);
            }
            else
            {
                dataStr = QString::number(ctx.trendSegment->values[i].value);
            }
        }
        switch (subId)
        {
        case SUB_PARAM_NIBP_SYS:
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP2_SYS:
            sys = dataStr;
            multiSubParamAlarm = ctx.trendSegment->values[i].alarmFlag;
            continue;
        case SUB_PARAM_NIBP_DIA:
        case SUB_PARAM_ART_DIA:
        case SUB_PARAM_PA_DIA:
        case SUB_PARAM_AUXP1_DIA:
        case SUB_PARAM_AUXP2_DIA:
            dia = dataStr;
            multiSubParamAlarm |= ctx.trendSegment->values[i].alarmFlag;
            continue;
        case SUB_PARAM_NIBP_MAP:
        case SUB_PARAM_ART_MAP:
        case SUB_PARAM_PA_MAP:
        case SUB_PARAM_AUXP1_MAP:
        case SUB_PARAM_AUXP2_MAP:
            map = dataStr;
            subParamAlarm = ctx.trendSegment->values[i].alarmFlag || multiSubParamAlarm;
            valueStr = sys + "/" + dia + "(" + map + ")";
            titleStr = paramInfo.getSubParamName(subId);
            titleStr = titleStr.left(titleStr.length() - 4);
            valueFont = fontManager.numFont(23);
            break;
        case SUB_PARAM_NIBP_PR:
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
            multiSubParamAlarm = ctx.trendSegment->values[i].alarmFlag;
            et = dataStr;
            continue;
        case SUB_PARAM_FICO2:
        case SUB_PARAM_FIN2O:
        case SUB_PARAM_FIAA1:
        case SUB_PARAM_FIAA2:
        case SUB_PARAM_FIO2:
            subParamAlarm = ctx.trendSegment->values[i].alarmFlag || multiSubParamAlarm;
            fi = dataStr;
            valueStr = et + "/" + fi;
            titleStr = trs(paramInfo.getSubParamName(subId));
            titleStr = titleStr.right(titleStr.length() - 2);
            titleStr += "(Et/Fi)";
            valueFont = fontManager.numFont(31);
            break;
        case SUB_PARAM_T1:
            t1 = dataStr;
            valueStr = dataStr;
            subParamAlarm = ctx.trendSegment->values[i].alarmFlag;
            titleStr = trs(paramInfo.getSubParamName(subId));
            valueFont = fontManager.numFont(37);
            break;
        case SUB_PARAM_T2:
            t2 = dataStr;
            valueStr = dataStr;
            subParamAlarm = ctx.trendSegment->values[i].alarmFlag;
            titleStr = trs(paramInfo.getSubParamName(subId));
            valueFont = fontManager.numFont(37);
            break;
        case SUB_PARAM_TD:
            if (t1 == InvStr() || t2 == InvStr())
            {
                td = InvStr();
            }
            else
            {
                td = QString::number(qAbs(t1.toFloat() - t2.toFloat()), 'f', 1);
            }
            valueStr = td;
            subParamAlarm = ctx.trendSegment->values[i].alarmFlag;
            titleStr = trs(paramInfo.getSubParamName(subId));
            valueFont = fontManager.numFont(37);
            break;
        default:
            valueStr = dataStr;
            subParamAlarm = ctx.trendSegment->values[i].alarmFlag;
            titleStr = trs(paramInfo.getSubParamName(subId));
            valueFont = fontManager.numFont(37);
            break;
        }

        item = new QListWidgetItem(trendListWidget);
        item->setData(EventTrendItemDelegate::ValueFontRole, qVariantFromValue(valueFont));
        item->setData(EventTrendItemDelegate::TitleFontRole, qVariantFromValue(titleFont));
        item->setData(EventTrendItemDelegate::UnitFontRole, qVariantFromValue(unitFont));

        item->setData(EventTrendItemDelegate::ValueTextRole, valueStr);
        item->setData(EventTrendItemDelegate::TitleTextRole, titleStr);
        item->setData(EventTrendItemDelegate::UnitTextRole,
                      trs(Unit::getSymbol(paramManager.getSubParamUnit(paramInfo.getParamID(subId), subId))));


        item->setData(EventTrendItemDelegate::TrendAlarmRole, subParamAlarm);

        subParamAlarm = false;
        multiSubParamAlarm = false;   // 每次设置报警之后清零。

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
    waveWidget->setInfoSegments(ctx.infoSegment);
    waveWidget->setWaveMedSecond(0);
    waveWidget->setWaveSegments(ctx.waveSegments);
}

void EventWindowPrivate::refreshPageInfo()
{
    QString title = QString("%1( %2/%3 )").arg(trs("EventReview"))
                                          .arg(QString::number(curPage))
                                          .arg(QString::number(totalPage));
    q_ptr->setWindowTitle(title);
}

void EventWindowPrivate::eventTypeOrLevelChange()
{
    dataIndex.clear();
    for (int i = blockList.count() - 1; i >= 0; i--)
    {
        BlockEntry blockInfo = blockList.at(i);
        EventType type = static_cast<EventType>(blockInfo.type & 0xff);
        AlarmPriority prio = static_cast<AlarmPriority>((blockInfo.type >> 8) & 0xff);
        if ((type == curEventType || curEventType == EventAll) &&
                (prio == levelToPriority(curEventLevel) || curEventLevel == EVENT_LEVEL_ALL))
        {
            dataIndex.append(i);
        }
    }
    curDisplayEventNum = dataIndex.count();
}

void EventWindowPrivate::calculationPage()
{
    curPage = 1;
    if (dataIndex.count() == 0)
    {
        totalPage = 1;
    }
    else if (dataIndex.count() % PAGE_ROW_COUNT)
    {
        totalPage = dataIndex.count() / PAGE_ROW_COUNT + 1;
    }
    else
    {
        totalPage = dataIndex.count() / PAGE_ROW_COUNT;
    }
    QString title = QString("%1( %2/%3 )").arg(trs("EventReview"))
                                          .arg(QString::number(curPage))
                                          .arg(QString::number(totalPage));
    q_ptr->setWindowTitle(title);
}

void EventWindowPrivate::refreshEventList()
{
    printList.clear();
    QList<QString> timeList;
    QList<QString> eventList;
    for (int i = 0; i < PAGE_ROW_COUNT; i++)
    {
        QString timeStr;
        QString dateStr;
        QString infoStr;
        SubParamID subId;
        AlarmPriority priority;
        AlarmPriority curPriority;
        int index = i + (curPage - 1) * PAGE_ROW_COUNT;
        if (index >= dataIndex.count())
        {
            break;
        }

        if (parseEventData(dataIndex.at(index)))
        {
            if (ctx.infoSegment->type == EventOxyCRG)
            {
                continue;
            }

            if (ctx.infoSegment->type != curEventType && curEventType != EventAll)
            {
                continue;
            }
            unsigned t = ctx.infoSegment->timestamp;
            // 事件时间
            timeDate.getDate(t, dateStr, true);
            timeDate.getTime(t, timeStr, true);
            QString timeItemStr = dateStr + " " + timeStr;

            switch (ctx.infoSegment->type)
            {
            case EventPhysiologicalAlarm:
            {
                AlarmLimitIFace *alarmLimit = NULL;
                AlarmOneShotIFace *alarmOneShot = NULL;
                subId = (SubParamID)(ctx.almSegment->subParamID);
                unsigned char alarmId = ctx.almSegment->alarmType;
                unsigned char alarmInfo = ctx.almSegment->alarmInfo;
                if (alarmInfo & 0x01)   // oneshot 报警事件
                {
                    alarmOneShot = alertor.getAlarmOneShotIFace(subId);
                    if (alarmOneShot)
                    {
                        priority = alarmOneShot->getAlarmPriority(alarmId);
                    }
                }
                else
                {
                    alarmLimit = alertor.getAlarmLimitIFace(subId);
                    if (alarmLimit)
                    {
                        priority = alarmLimit->getAlarmPriority(alarmId);
                    }
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
                // oneshot 报警
                if (alarmInfo & 0x01)
                {
                    // 将参数ID转换为oneshot报警对应的参数ID
                    if (paramId == PARAM_DUP_ECG)
                    {
                        paramId = PARAM_ECG;
                    }
                    else if (paramId == PARAM_DUP_RESP)
                    {
                        paramId = PARAM_RESP;
                    }
                }
                infoStr += " ";
                infoStr += trs(Alarm::getPhyAlarmMessage(paramId, alarmId, alarmInfo & 0x1));

                // 超限报警
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
                    infoStr += " ";
                    infoStr += trs(Unit::getSymbol(unit));
                }
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            case EventCodeMarker:
            {
                if (levelToPriority(curEventLevel) != ALARM_PRIO_PROMPT)
                {
                    continue;
                }
                infoStr = (QString)ctx.codeMarkerSegment->codeName;
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            case EventRealtimePrint:
            {
                if (levelToPriority(curEventLevel) != ALARM_PRIO_PROMPT)
                {
                    continue;
                }
                infoStr = trs("RealtimePrintSegment");
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            case EventNIBPMeasurement:
            {
                if (levelToPriority(curEventLevel) != ALARM_PRIO_PROMPT)
                {
                    continue;
                }
                infoStr = trs("NibpMeasurement");
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            case EventWaveFreeze:
            {
                if (levelToPriority(curEventLevel) != ALARM_PRIO_PROMPT)
                {
                    continue;
                }
                infoStr = trs("WaveFreeze");
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            }
            default:
                continue;
            }
            QString printStr = timeItemStr + " " + infoStr;
            printList.append(printStr);
        }
    }
    model->setPageRowCount(PAGE_ROW_COUNT);
    model->updateEvent(timeList, eventList);
}
