#include "EventReviewWindow.h"
#include <QListWidget>
#include "EventWaveWidget.h"
#include "EventInfoWidget.h"
#include "ITableWidget.h"
#include "WindowManager.h"
#include "IMoveButton.h"
#include "FontManager.h"
#include "EventTrendItemDelegate.h"
#include "ColorManager.h"
#include "EventStorageManager.h"
#include "LanguageManager.h"
#include "IDropList.h"
#include "TimeDate.h"
#include "ParamInfo.h"
#include "ParamManager.h"
#include "AlarmConfig.h"
#include "EventDataSymbol.h"
#include "EventWaveSetWidget.h"
#include "ConfigManager.h"
#include <QBoxLayout>
#include "Debug.h"
#include "Utility.h"
#include <QFile>
#include <QHeaderView>
#include <QScrollBar>
#include "EventDataParseContext.h"
#include "RecorderManager.h"
#include "EventPageGenerator.h"

#define ITEM_HEIGHT     30
#define ITEM_WIDTH      100

EventReviewWindow *EventReviewWindow::_selfObj = NULL;


class EventReviewWindowPrivate
{
public:
    EventReviewWindowPrivate()
        :trendListWidget(NULL),
          waveWidget(NULL),
          backend(NULL),
          curParseIndex(-1)
    {
        backend = eventStorageManager.backend();
    }

    //prase the event data from the backend
    bool parseEventData(int dataIndex)
    {
        ctx.reset();
        //clear the wave with empyt wave segments
        waveWidget->setWaveSegments(ctx.waveSegments);
        return ctx.parse(backend, dataIndex);
    }


    void loadTestWaveSegment()
    {
        QFile f("/usr/local/nPM/demodata/SPO2");
        f.open(QIODevice::ReadOnly);
        QByteArray d = f.readAll();
        f.close();
        
//        WaveformDataSegment *seg = new WaveformDataSegment;
    }

    EventInfoWidget *infoWidget;
    QListWidget *trendListWidget;
    EventWaveWidget *waveWidget;
    IButton *eventList;
    IMoveButton *moveCoordinate;
    IMoveButton *moveEvent;
    IButton *print;
    IButton *set;
    IButton *upParam;
    IButton *downParam;

    ITableWidget *eventTable;
    IButton *upTable;
    IButton *downTable;
    IDropList *type;
    IDropList *level;

    QWidget *dataWidget;
    QWidget *chartWidget;
    QStackedLayout *stackLayout;

    EventDataPraseContext ctx;
    IStorageBackend *backend;
    int curParseIndex;
    int eventNum;                           // 总事件数
    int curListScroller;
    EventType curEventType;
    EventLevel curEventLevel;
    QList<int> dataIndex;                   // 当前选中事件项对应的数据所在索引
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
EventReviewWindow::EventReviewWindow()
    :PopupWidget(), d_ptr(new EventReviewWindowPrivate())
{
    d_ptr->curEventType = EventAll;
    d_ptr->curEventLevel = EVENT_LEVEL_ALL;
    setAttribute(Qt::WA_DeleteOnClose, true);
    setTitleBarText(trs("EventReview"));

    int windowWidth = windowManager.getPopMenuWidth();
    int windowHeight = windowManager.getPopMenuHeight();
    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);

//    int itemW = windowManager.getPopMenuWidth() - 20;
//    int btnWidth = itemW / 4;

    // 事件表格窗口
    d_ptr->eventTable = new ITableWidget();
    d_ptr->eventTable->setColumnCount(2);
    d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);
    d_ptr->eventTable->horizontalHeader()->setEnabled(false);
    d_ptr->eventTable->verticalHeader()->setVisible(false);
    d_ptr->eventTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d_ptr->eventTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->eventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ptr->eventTable->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->eventTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QStringList tableTitle;
    tableTitle << trs("Time") << trs("Incident");
    d_ptr->eventTable->setHorizontalHeaderLabels(tableTitle);
    connect(d_ptr->eventTable, SIGNAL(clicked(QModelIndex)), this, SLOT(_waveInfoReleased(QModelIndex)));

    d_ptr->type = new IDropList(trs("Type"));
    d_ptr->type->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->type->setFont(font);
    for (int i = 0; i < EventTypeMax; i ++)
    {
        d_ptr->type->addItem(trs(EventDataSymbol::convert((EventType)i)));
    }
    connect(d_ptr->type, SIGNAL(currentIndexChange(int)), this, SLOT(_eventTypeSelect(int)));

    d_ptr->level = new IDropList(trs("Level"));
    d_ptr->level->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->level->setFont(font);
    for (int i = 0; i < EVENT_LEVEL_NR; i ++)
    {
        d_ptr->level->addItem(trs(EventDataSymbol::convert((EventLevel)i)));
    }
    connect(d_ptr->level, SIGNAL(currentIndexChange(int)), this, SLOT(_eventLevelSelect(int)));

    d_ptr->upTable = new IButton();
    d_ptr->upTable->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->upTable->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(d_ptr->upTable, SIGNAL(realReleased()), this, SLOT(_upMoveEventReleased()));

    d_ptr->downTable = new IButton();
    d_ptr->downTable->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->downTable->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(d_ptr->downTable, SIGNAL(realReleased()), this,  SLOT(_downMoveEventReleased()));

    QHBoxLayout *hTableLayout = new QHBoxLayout();
    hTableLayout->setMargin(0);
    hTableLayout->setSpacing(2);
    hTableLayout->addWidget(d_ptr->type);
    hTableLayout->addWidget(d_ptr->level);
    hTableLayout->addWidget(d_ptr->upTable);
    hTableLayout->addWidget(d_ptr->downTable);

    QVBoxLayout *vTableLayout = new QVBoxLayout();
    vTableLayout->addWidget(d_ptr->eventTable);
    vTableLayout->addLayout(hTableLayout);

    d_ptr->dataWidget = new QWidget();
    d_ptr->dataWidget->setLayout(vTableLayout);

    // 事件波形窗口
    d_ptr->infoWidget = new EventInfoWidget;
    d_ptr->infoWidget->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *hlayout =new QHBoxLayout();

    d_ptr->waveWidget = new EventWaveWidget;
    d_ptr->waveWidget->setWaveSegments(d_ptr->ctx.waveSegments);
    d_ptr->waveWidget->setFocusPolicy(Qt::NoFocus);
    hlayout->addWidget(d_ptr->waveWidget, 3);
    hlayout->addSpacing(1);

    d_ptr->trendListWidget = new QListWidget;
    QPalette pal = d_ptr->trendListWidget->palette();
    pal.setColor(QPalette::Base, Qt::black);
    d_ptr->trendListWidget->setPalette(pal);
    d_ptr->trendListWidget->setFrameShape(QFrame::NoFrame);
    d_ptr->trendListWidget->setItemDelegate(new EventTrendItemDelegate(d_ptr->trendListWidget));
    d_ptr->trendListWidget->setResizeMode(QListWidget::Adjust);
    d_ptr->trendListWidget->setFocusPolicy(Qt::NoFocus);
    hlayout->addWidget(d_ptr->trendListWidget, 1);

    d_ptr->eventList = new IButton(trs("EventList"));
    d_ptr->eventList->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->eventList->setFont(font);
    connect(d_ptr->eventList, SIGNAL(realReleased()), this, SLOT(_eventListReleased()));

    d_ptr->moveCoordinate = new IMoveButton(trs("MoveCoordinate"));
    d_ptr->moveCoordinate->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->moveCoordinate->setFont(font);
    connect(d_ptr->moveCoordinate, SIGNAL(leftMove()), this, SLOT(_leftMoveCoordinate()));
    connect(d_ptr->moveCoordinate, SIGNAL(rightMove()), this, SLOT(_rightMoveCoordinate()));

    d_ptr->moveEvent = new IMoveButton(trs("MoveEvent"));
    d_ptr->moveEvent->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->moveEvent->setFont(font);
    connect(d_ptr->moveEvent, SIGNAL(leftMove()), this, SLOT(_leftMoveEvent()));
    connect(d_ptr->moveEvent, SIGNAL(rightMove()), this, SLOT(_rightMoveEvent()));

    d_ptr->print = new IButton(trs("Print"));
    d_ptr->print->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->print->setFont(font);
    connect(d_ptr->print, SIGNAL(realReleased()), this, SLOT(_printRelease()));

    d_ptr->set = new IButton(trs("Set"));
    d_ptr->set->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->set->setFont(font);
    connect(d_ptr->set, SIGNAL(realReleased()), this, SLOT(_setReleased()));

    d_ptr->upParam = new IButton();
    d_ptr->upParam->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->upParam->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(d_ptr->upParam, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    d_ptr->downParam = new IButton();
    d_ptr->downParam->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->downParam->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(d_ptr->downParam, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *lWaveLayout = new QHBoxLayout();
    lWaveLayout->setMargin(0);
    lWaveLayout->setSpacing(2);
    lWaveLayout->addWidget(d_ptr->eventList);
    lWaveLayout->addWidget(d_ptr->moveCoordinate);
    lWaveLayout->addWidget(d_ptr->moveEvent);
    lWaveLayout->addWidget(d_ptr->print);
    lWaveLayout->addWidget(d_ptr->set);
    lWaveLayout->addWidget(d_ptr->upParam);
    lWaveLayout->addWidget(d_ptr->downParam);

    QVBoxLayout *vWaveLayout = new QVBoxLayout();
    vWaveLayout->addWidget(d_ptr->infoWidget, 1);
    vWaveLayout->addSpacing(1);
    vWaveLayout->addLayout(hlayout, 15);
    vWaveLayout->addLayout(lWaveLayout);

    d_ptr->chartWidget = new QWidget();
    d_ptr->chartWidget->setLayout(vWaveLayout);

    d_ptr->stackLayout = new QStackedLayout();
    d_ptr->stackLayout->addWidget(d_ptr->dataWidget);
    d_ptr->stackLayout->addWidget(d_ptr->chartWidget);

    contentLayout->addLayout(d_ptr->stackLayout);


    setFixedSize(windowWidth, windowHeight);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
EventReviewWindow::~EventReviewWindow()
{

}

/**************************************************************************************************
 * 事件信息布局刷新。
 *************************************************************************************************/
void EventReviewWindow::eventInfoUpdate()
{
    QString infoStr;
    QString timeInfoStr;
    QString indexStr;

    switch (d_ptr->ctx.infoSegment->type)
    {
    case EventPhysiologicalAlarm:
    {
        SubParamID subId = (SubParamID)(d_ptr->ctx.almSegment->subParamID);
        AlarmLimitIFace *alarmLimit = alertor.getAlarmLimitIFace(subId);
        unsigned char alarmId = d_ptr->ctx.almSegment->alarmType;
        unsigned char alarmInfo = d_ptr->ctx.almSegment->alarmInfo;
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
        infoStr += paramInfo.getSubParamName(subId);

        if ((alarmInfo >> 1) & 0x1)
        {
            infoStr += trs("Upper");
            infoStr += " > ";
        }
        else
        {
            infoStr += trs("Lower");
            infoStr += " < ";
        }
        ParamID id = paramInfo.getParamID(subId);
        UnitType type = paramManager.getSubParamUnit(id, subId);
        LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subId, type);
        double limitValue = (double)d_ptr->ctx.almSegment->alarmLimit / config.scale;
        infoStr += QString::number(limitValue);
        break;
    }
    case EventCodeMarker:
    {
        infoStr = (QString)d_ptr->ctx.codeMarkerSegment->codeName;
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
    t = d_ptr->ctx.infoSegment->timestamp;
    timeDate.getDate(t, dateStr, true);
    timeDate.getTime(t, timeStr, true);
    timeInfoStr = dateStr + " " + timeStr;

    indexStr = QString::number(d_ptr->eventTable->currentRow() + 1) + "/" + QString::number(d_ptr->eventTable->rowCount());

    d_ptr->infoWidget->loadDataInfo(infoStr, timeInfoStr, indexStr);
}

/**************************************************************************************************
 * 事件趋势布局刷新。
 *************************************************************************************************/
void EventReviewWindow::eventTrendUpdate()
{
    d_ptr->trendListWidget->clear();
    QListWidgetItem *item;
    SubParamID subId;
    QColor color;
    QFont valueFont;
    QFont titleFont = fontManager.textFont(16);
    QFont unitFont = fontManager.textFont(12);

    QString sys;
    QString dia;
    QString map;
    QString et;
    QString fi;
    QString valueStr;
    QString titleStr;
    int paramNum = d_ptr->ctx.trendSegment->trendValueNum;
    for (int i = 0; i < paramNum; i ++)
    {
        QString dataStr;
        if (d_ptr->ctx.trendSegment->values[i].value == InvData())
        {
            dataStr = "-.-";
        }
        else
        {
            dataStr = QString::number(d_ptr->ctx.trendSegment->values[i].value);
        }
        subId = (SubParamID)d_ptr->ctx.trendSegment->values[i].subParamId;
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
            valueFont = fontManager.numFont(20);
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
            valueFont = fontManager.numFont(26);
            break;
        default:
            valueStr = dataStr;
            titleStr = paramInfo.getSubParamName(subId);
            valueFont = fontManager.numFont(32);
            break;
        }

        item = new QListWidgetItem(d_ptr->trendListWidget);
        item->setData(EventTrendItemDelegate::ValueFontRole, qVariantFromValue(valueFont));
        item->setData(EventTrendItemDelegate::TitleFontRole, qVariantFromValue(titleFont));
        item->setData(EventTrendItemDelegate::UnitFontRole, qVariantFromValue(unitFont));

        item->setData(EventTrendItemDelegate::ValueTextRole, valueStr);
        item->setData(EventTrendItemDelegate::TitleTextRole, titleStr);
        item->setData(EventTrendItemDelegate::UnitTextRole, Unit::getSymbol(paramInfo.getUnitOfSubParam(subId)));


        item->setData(EventTrendItemDelegate::TrendAlarmRole, d_ptr->ctx.trendSegment->values[i].alarmFlag);
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

/**************************************************************************************************
 * 事件波形布局刷新。
 *************************************************************************************************/
void EventReviewWindow::eventWaveUpdate()
{
    d_ptr->waveWidget->setWaveSegments(d_ptr->ctx.waveSegments);
}

void EventReviewWindow::setWaveSpeed(int speed)
{
    d_ptr->waveWidget->setSweepSpeed((EventWaveWidget::SweepSpeed)speed);
}

void EventReviewWindow::setWaveGain(int gain)
{
    d_ptr->waveWidget->setGain((ECGEventGain)gain);
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void EventReviewWindow::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

    d_ptr->stackLayout->setCurrentIndex(0);
    _loadEventData();
}

/**************************************************************************************************
 * 切换至波形事件布局
 *************************************************************************************************/
void EventReviewWindow::_waveInfoReleased(QModelIndex index)
{
    d_ptr->stackLayout->setCurrentIndex(1);
    if (!d_ptr->backend->getBlockNR())
    {
        return;
    }

    d_ptr->parseEventData(d_ptr->dataIndex.at(index.row()));
    eventInfoUpdate();
    eventTrendUpdate();
    eventWaveUpdate();
    d_ptr->eventList->setFocus();
}

/**************************************************************************************************
 * 切换至表格事件布局
 *************************************************************************************************/
void EventReviewWindow::_eventListReleased()
{
    d_ptr->stackLayout->setCurrentIndex(0);
}

/**************************************************************************************************
 * 向上事件翻页
 *************************************************************************************************/
void EventReviewWindow::_upMoveEventReleased()
{
    int maxValue = d_ptr->eventTable->verticalScrollBar()->maximum();
    d_ptr->curListScroller = d_ptr->eventTable->verticalScrollBar()->value();
    if (d_ptr->curListScroller > 0)
    {
        QScrollBar *scrollBar = d_ptr->eventTable->verticalScrollBar();
        int positon = d_ptr->curListScroller - (maxValue * 15) / (d_ptr->eventTable->rowCount() - 15);
        scrollBar->setSliderPosition(positon);
    }
}

/**************************************************************************************************
 * 向下事件翻页
 *************************************************************************************************/
void EventReviewWindow::_downMoveEventReleased()
{
    int maxValue = d_ptr->eventTable->verticalScrollBar()->maximum();
    d_ptr->curListScroller = d_ptr->eventTable->verticalScrollBar()->value();
    if (d_ptr->curListScroller < maxValue && d_ptr->eventTable->rowCount() != 15)
    {
        QScrollBar *scrollBar = d_ptr->eventTable->verticalScrollBar();
        int positon = d_ptr->curListScroller + (maxValue * 15) / (d_ptr->eventTable->rowCount() - 15);
        scrollBar->setSliderPosition(positon);
    }
}

/**************************************************************************************************
 * 事件类型选择。
 *************************************************************************************************/
void EventReviewWindow::_eventTypeSelect(int index)
{
    d_ptr->curEventType = (EventType)index;
    _loadEventData();
}

/**************************************************************************************************
 * 事件级别选择。
 *************************************************************************************************/
void EventReviewWindow::_eventLevelSelect(int index)
{
    d_ptr->curEventLevel = (EventLevel)index;
    _loadEventData();
}

/**************************************************************************************************
 * 左移坐标。
 *************************************************************************************************/
void EventReviewWindow::_leftMoveCoordinate()
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
        return;
    case EventWaveWidget::SWEEP_SPEED_125:
        if (medSecond == -durationBefore + 4)
        {
            return;
        }
        medSecond --;
        break;
    case EventWaveWidget::SWEEP_SPEED_250:
        if (medSecond == -durationBefore + 2)
        {
            return;
        }
        medSecond --;
        break;
    case EventWaveWidget::SWEEP_SPEED_500:
        if (medSecond == -durationBefore + 1)
        {
            return;
        }
        medSecond --;
        break;
    default:
        break;
    }
    d_ptr->waveWidget->setWaveMedSecond(medSecond);
}

/**************************************************************************************************
 * 右移坐标。
 *************************************************************************************************/
void EventReviewWindow::_rightMoveCoordinate()
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
        return;
    case EventWaveWidget::SWEEP_SPEED_125:
        if (medSecond == durationAfter - 4)
        {
            return;
        }
        medSecond ++;
        break;
    case EventWaveWidget::SWEEP_SPEED_250:
        if (medSecond == durationAfter - 2)
        {
            return;
        }
        medSecond ++;
        break;
    case EventWaveWidget::SWEEP_SPEED_500:
        if (medSecond == durationAfter - 1)
        {
            return;
        }
        medSecond ++;
        break;
    default:
        break;
    }
    d_ptr->waveWidget->setWaveMedSecond(medSecond);
}

/**************************************************************************************************
 * 左移事件。
 *************************************************************************************************/
void EventReviewWindow::_leftMoveEvent()
{
    if (d_ptr->eventTable->currentRow() != 0)
    {
        d_ptr->eventTable->selectRow(d_ptr->eventTable->currentRow() - 1);
    }
    if (!d_ptr->backend->getBlockNR())
    {
        return;
    }

    d_ptr->parseEventData(d_ptr->dataIndex.at(d_ptr->eventTable->currentRow()));
    eventInfoUpdate();
    eventTrendUpdate();
    eventWaveUpdate();
}

/**************************************************************************************************
 * 右移事件。
 *************************************************************************************************/
void EventReviewWindow::_rightMoveEvent()
{
    if (d_ptr->eventTable->currentRow() != d_ptr->eventNum - 1)
    {
        d_ptr->eventTable->selectRow(d_ptr->eventTable->currentRow() + 1);
    }
    if (!d_ptr->backend->getBlockNR())
    {
        return;
    }

    d_ptr->parseEventData(d_ptr->dataIndex.at(d_ptr->eventTable->currentRow()));
    eventInfoUpdate();
    eventTrendUpdate();
    eventWaveUpdate();
}

/**************************************************************************************************
 * 设置槽函数。
 *************************************************************************************************/
void EventReviewWindow::_setReleased()
{
    eventWaveSetWidget.autoShow();
}

/**************************************************************************************************
 * 向上翻阅参数。
 *************************************************************************************************/
void EventReviewWindow::_upReleased()
{
    int maxValue = d_ptr->trendListWidget->verticalScrollBar()->maximum();
    int curScroller = d_ptr->trendListWidget->verticalScrollBar()->value();
    if (curScroller > 0)
    {
        QScrollBar *scrollBar = d_ptr->trendListWidget->verticalScrollBar();
        int position = curScroller - (maxValue * 5) / (d_ptr->trendListWidget->count() - 5);
        scrollBar->setSliderPosition(position);
    }
}

/**************************************************************************************************
 * 向下翻阅参数
 *************************************************************************************************/
void EventReviewWindow::_downReleased()
{
    int maxValue = d_ptr->trendListWidget->verticalScrollBar()->maximum();
    int curScroller = d_ptr->trendListWidget->verticalScrollBar()->value();
    if (curScroller < maxValue && d_ptr->trendListWidget->count() != 5)
    {
        QScrollBar *scrollBar = d_ptr->trendListWidget->verticalScrollBar();
        int position = curScroller + (maxValue * 5) / (d_ptr->trendListWidget->count() - 5);
        scrollBar->setSliderPosition(position);
    }
}

void EventReviewWindow::_printRelease()
{
    if(d_ptr->eventTable->currentRow() < d_ptr->dataIndex.size() &&
            d_ptr->eventTable->currentRow() >= 0)
    {
        recorderManager.addPageGenerator(new EventPageGenerator(d_ptr->backend, d_ptr->dataIndex.at(d_ptr->eventTable->currentRow())));
    }
}

/**************************************************************************************************
 * 载入事件数据。
 *************************************************************************************************/
void EventReviewWindow::_loadEventData()
{
    d_ptr->dataIndex.clear();
    d_ptr->eventTable->setRowCount(0);
    d_ptr->eventNum = d_ptr->backend->getBlockNR();

    unsigned t = 0;
    QString timeStr;
    QString dateStr;
    QString infoStr;
    QTableWidgetItem *item;
    unsigned char alarmInfo;
    SubParamID subId;
    unsigned char alarmId;
    AlarmPriority priority;
    AlarmPriority curPriority;
    AlarmLimitIFace *alarmLimit;
    int row = 0;
    for (int i = d_ptr->eventNum - 1; i >= 0; i --)
    {
        priority = ALARM_PRIO_PROMPT;
        if (d_ptr->parseEventData(i))
        {
            if (d_ptr->ctx.infoSegment->type == EventOxyCRG)
            {
                continue;
            }

            t = d_ptr->ctx.infoSegment->timestamp;
            // 事件时间
            timeDate.getDate(t, dateStr, true);
            timeDate.getTime(t, timeStr, true);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(dateStr + " " + timeStr);
            d_ptr->eventTable->setRowCount(row + 1);
            d_ptr->eventTable->setItem(row, 0, item);

            switch (d_ptr->ctx.infoSegment->type)
            {
            case EventPhysiologicalAlarm:
            {
                subId = (SubParamID)(d_ptr->ctx.almSegment->subParamID);
                alarmId = d_ptr->ctx.almSegment->alarmType;
                alarmInfo = d_ptr->ctx.almSegment->alarmInfo;
                alarmLimit = alertor.getAlarmLimitIFace(subId);
                if (alarmLimit)
                {
                    priority = alarmLimit->getAlarmPriority(alarmId);
                }

                if (d_ptr->curEventType != EventAll)
                {
                    if (d_ptr->curEventType != d_ptr->ctx.infoSegment->type)
                    {
                        continue;
                    }
                }

                if (d_ptr->curEventLevel != EVENT_LEVEL_ALL)
                {
                    curPriority = _levelToPriority(d_ptr->curEventLevel);
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
                infoStr += trs(eventStorageManager.getPhyAlarmMessage(paramId,
                                                                      alarmInfo,
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

                    infoStr += Util::convertToString(d_ptr->ctx.almSegment->alarmLimit, config.scale);
                }

                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(infoStr);
                d_ptr->eventTable->setItem(row, 1, item);
                break;
            }
            case EventCodeMarker:
            {
                infoStr = (QString)d_ptr->ctx.codeMarkerSegment->codeName;
                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(infoStr);
                d_ptr->eventTable->setItem(row, 1, item);
                break;
            }
            case EventRealtimePrint:
            {
                //TODO
                break;
            }
            case EventNIBPMeasurement:
            {
                //TODO
                break;
            }
            case EventWaveFreeze:
            {
                //TODO
                break;
            }
            default:
                break;
            }
            d_ptr->dataIndex.append(i);
            row ++;
        }
    }
    if (row)
    {
        d_ptr->eventTable->selectRow(0);
    }
}

/**************************************************************************************************
 * 报警级别转换到优先级。
 *************************************************************************************************/
AlarmPriority EventReviewWindow::_levelToPriority(EventLevel level)
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
