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
#include <QBoxLayout>
#include "Debug.h"
#include "Alarm.h"
#include <QFile>
#include <QHeaderView>

#define ITEM_HEIGHT     30
#define ITEM_WIDTH      80

struct EventDataPraseContex
{
    EventDataPraseContex()
        :eventDataBuf(NULL),
          infoSegment(NULL),
          trendSegment(NULL),
          almSegment(NULL),
          codeMarkerSegment(NULL)
    {
    }

    void reset()
    {
        if(eventDataBuf)
        {
            qFree(eventDataBuf);
            eventDataBuf = NULL;
        }
        infoSegment = NULL;
        trendSegment = NULL;
        waveSegments.clear();
        almSegment = NULL;
        codeMarkerSegment = NULL;
    }

    ~EventDataPraseContex()
    {
        if(eventDataBuf)
        {
            qFree(eventDataBuf);
        }
    }

    char *eventDataBuf; //buffer for the event data
    EventInfoSegment *infoSegment;  //pointor of the info segment
    TrendDataSegment *trendSegment; //pointer of the trend segment
    QVector<WaveformDataSegment *> waveSegments;    //pointers of the wave segments
    AlarmInfoSegment *almSegment;   //pointer to the alarm segment
    CodeMarkerSegment *codeMarkerSegment;   //pointer to the code marker segment
};

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
        if(!backend || dataIndex >= (int) backend->getBlockNR() || dataIndex < 0)
        {
            return false;
        }

        quint32 length  = backend->getBlockDataLen(dataIndex);

        char *buf = (char *)qMalloc(length);
        if(!buf)
        {
            return false;
        }

        if (backend->readBlockData(dataIndex, buf, length) != length)
        {
            qFree(buf);
            return false;
        }

        ctx.reset();

        //clear the wave with empyt wave segments
        waveWidget->setWaveSegments(ctx.waveSegments);

        ctx.eventDataBuf = buf;

        char *parseBuffer = buf;
        bool parseEnd = false;
        while(!parseEnd)
        {
            EventSegmentType *eventType = (EventSegmentType *)parseBuffer;
            switch (*eventType) {
            case EVENT_INFO_SEGMENT:
                //skip the offset of the segment type field
                parseBuffer += sizeof(EventSegmentType);
                ctx.infoSegment = (EventInfoSegment *) parseBuffer;
                //find the location of the next event type
                parseBuffer += sizeof(EventInfoSegment);
                break;
            case EVENT_TRENDDATA_SEGMENT:
                //skip the offset of the segment type field
                parseBuffer += sizeof(EventSegmentType);
                ctx.trendSegment = (TrendDataSegment *)parseBuffer;
                //find the location of the next event type
                parseBuffer += sizeof(TrendDataSegment) + ctx.trendSegment->trendValueNum * sizeof(TrendValueSegment);
                break;
            case EVENT_WAVEFORM_SEGMENT:
                //skip the offset of the segment type field
                parseBuffer += sizeof(EventSegmentType);
                ctx.waveSegments.append((WaveformDataSegment *) parseBuffer);
                //find the location of the next event type
                parseBuffer += sizeof(WaveformDataSegment) + ctx.waveSegments.last()->waveNum * sizeof(WaveDataType);
                break;
            case EVENT_ALARM_INFO_SEGMENT:
                parseBuffer += sizeof(EventSegmentType);
                ctx.almSegment = (AlarmInfoSegment *) parseBuffer;
                parseBuffer += sizeof(AlarmInfoSegment);
                break;
            case EVENT_CODEMARKER_SEGMENT:
                parseBuffer += sizeof(EventSegmentType);
                ctx.codeMarkerSegment = (CodeMarkerSegment *) parseBuffer;
                parseBuffer += sizeof(CodeMarkerSegment);
                break;
            default:
                qdebug("unknown segment type %d, stop parsing.",  *eventType);
                parseEnd = true;
                break;
            }
        }

        if(parseBuffer >= buf + length)
        {
            parseEnd = true;
        }
    }


    void loadTestWaveSegment()
    {
        QFile f("/usr/local/nPM/demodata/SPO2");
        f.open(QIODevice::ReadOnly);
        QByteArray d = f.readAll();
        f.close();
        
        WaveformDataSegment *seg = new WaveformDataSegment;
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
    IButton *waveInfo;
    IButton *upTable;
    IButton *downTable;
    IDropList *type;
    IDropList *level;

    QWidget *dataWidget;
    QWidget *chartWidget;
    QStackedLayout *stackLayout;

    EventDataPraseContex ctx;
    IStorageBackend *backend;
    int curParseIndex;
};

EventReviewWindow::EventReviewWindow()
    :PopupWidget(), d_ptr(new EventReviewWindowPrivate())
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Event Review");

    int windowWidth = windowManager.getPopMenuWidth();
    int windowHeight = windowManager.getPopMenuHeight();
    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);

    int itemW = windowManager.getPopMenuWidth() - 20;
    int btnWidth = itemW / 4;
    int labelWidth = btnWidth/2;

    // 事件表格窗口
    d_ptr->eventTable = new ITableWidget();
    d_ptr->eventTable->setColumnCount(2);
    d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);
    d_ptr->eventTable->verticalHeader()->setVisible(false);
    d_ptr->eventTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d_ptr->eventTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->eventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ptr->eventTable->setSelectionMode(QAbstractItemView::NoSelection);
    d_ptr->eventTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QStringList tableTitle;
    tableTitle << trs("Time") << trs("Event");
    d_ptr->eventTable->setHorizontalHeaderLabels(tableTitle);

    d_ptr->waveInfo = new IButton(trs("WaveInfo"));
    d_ptr->waveInfo->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->waveInfo->setFont(font);
    connect(d_ptr->waveInfo, SIGNAL(realReleased()), this, SLOT(_waveInfoReleased()));

    d_ptr->type = new IDropList(trs("Type"));
    d_ptr->type->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->type->setFont(font);

    d_ptr->level = new IDropList(trs("Level"));
    d_ptr->level->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->level->setFont(font);

    d_ptr->upTable = new IButton();
    d_ptr->upTable->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->upTable->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));

    d_ptr->downTable = new IButton();
    d_ptr->downTable->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->downTable->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));

    QHBoxLayout *hTableLayout = new QHBoxLayout();
    hTableLayout->setMargin(0);
    hTableLayout->setSpacing(2);
    hTableLayout->addWidget(d_ptr->waveInfo);
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

    QHBoxLayout *hlayout =new QHBoxLayout();

    d_ptr->waveWidget = new EventWaveWidget;
    d_ptr->waveWidget->setWaveSegments(d_ptr->ctx.waveSegments);
    hlayout->addWidget(d_ptr->waveWidget, 3);
    hlayout->addSpacing(1);

    d_ptr->trendListWidget = new QListWidget;
    QPalette pal = d_ptr->trendListWidget->palette();
    pal.setColor(QPalette::Base, Qt::black);
    d_ptr->trendListWidget->setPalette(pal);
    d_ptr->trendListWidget->setFrameShape(QFrame::NoFrame);
    d_ptr->trendListWidget->setItemDelegate(new EventTrendItemDelegate(d_ptr->trendListWidget));
    d_ptr->trendListWidget->setResizeMode(QListWidget::Adjust);

    QListWidgetItem *item;
    QStringList valueList;
    QStringList titleList;
    QStringList unitList;
    valueList << "60" <<"98" << "120/90/(100)" << "4.6" << "14" << "120/90(100)" << "20" <<"36.5" << "36.4" <<"0.1";
    titleList <<"HR" <<"SPO2" <<"NIBP" <<"CO2" <<"RR" <<"ART" <<"CVP" <<"T1" <<"T2" <<QString::fromUtf8("ΔT");
    unitList <<"bpm" <<"%" <<"mmHg" <<"%" <<"rpm"<<"mmHg"<<"mmHg" << QString::fromUtf8("°C")
               << QString::fromUtf8("°C")<< QString::fromUtf8("°C");

    QStringList paramNames;
    paramNames << "ECG" <<"SPO2" <<"NIBP" << "CO2" <<"RESP" <<"IBP" <<"IBP"<<"TEMP"<<"TEMP"<<"TEMP";

    QFont valueFont = fontManager.numFont(32);
    QFont titleFont = fontManager.textFont(16);
    QFont unitFont = fontManager.textFont(12);

    for(int i = 0; i< valueList.length(); i++)
    {
        item = new QListWidgetItem(d_ptr->trendListWidget);

        if(valueList.at(i).length() >= 10)
        {
            item->setData(EventTrendItemDelegate::ValueFontRole, qVariantFromValue(fontManager.numFont(28)));
        }
        else
        {
            item->setData(EventTrendItemDelegate::ValueFontRole, qVariantFromValue(valueFont));
        }
        item->setData(EventTrendItemDelegate::TitleFontRole, qVariantFromValue(titleFont));
        item->setData(EventTrendItemDelegate::UnitFontRole, qVariantFromValue(unitFont));

        item->setData(EventTrendItemDelegate::ValueTextRole, valueList.at(i));
        item->setData(EventTrendItemDelegate::TitleTextRole, titleList.at(i));
        item->setData(EventTrendItemDelegate::UnitTextRole, unitList.at(i));

        item->setData(EventTrendItemDelegate::TrendAlarmRole, false);

        item->setTextColor(colorManager.getColor(paramNames.at(i)));
        item->setFlags(Qt::NoItemFlags);
    }
    hlayout->addWidget(d_ptr->trendListWidget, 1);

    d_ptr->eventList = new IButton(trs("EventList"));
    d_ptr->eventList->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->eventList->setFont(font);
    connect(d_ptr->eventList, SIGNAL(realReleased()), this, SLOT(_eventListReleased()));

    d_ptr->moveCoordinate = new IMoveButton(trs("MoveCoordinate"));
    d_ptr->moveCoordinate->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->moveCoordinate->setFont(font);

    d_ptr->moveEvent = new IMoveButton(trs("MoveEvent"));
    d_ptr->moveEvent->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->moveEvent->setFont(font);

    d_ptr->print = new IButton(trs("Print"));
    d_ptr->print->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->print->setFont(font);

    d_ptr->set = new IButton(trs("Set"));
    d_ptr->set->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->set->setFont(font);

    d_ptr->upParam = new IButton();
    d_ptr->upParam->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->upParam->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));

    d_ptr->downParam = new IButton();
    d_ptr->downParam->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->downParam->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));

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

EventReviewWindow::~EventReviewWindow()
{

}

void EventReviewWindow::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

    _loadEventData();
}

void EventReviewWindow::_waveInfoReleased()
{
    d_ptr->stackLayout->setCurrentIndex(1);
}

void EventReviewWindow::_eventListReleased()
{
    d_ptr->stackLayout->setCurrentIndex(0);
}

/**************************************************************************************************
 * 载入事件数据。
 *************************************************************************************************/
void EventReviewWindow::_loadEventData()
{
    int eventNum = d_ptr->backend->getBlockNR();
    d_ptr->eventTable->setRowCount(eventNum);

    unsigned t = 0;
    QString timeStr;
    QString dateStr;
    QString infoStr;
    QTableWidgetItem *item;
    SubParamID subId;
    ParamID paramId;
    int alarmId;
    AlarmPriority priority;
    int row = 0;
    for (int i = eventNum - 1; i >= 0; i --)
    {
        priority = ALARM_PRIO_PROMPT;
        if (d_ptr->parseEventData(i))
        {
            // 事件时间
            t = d_ptr->ctx.infoSegment->timestamp;
            timeDate.getDate(t, dateStr, true);
            timeDate.getTime(t, timeStr, true);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(dateStr + " " + timeStr);
            d_ptr->eventTable->setItem(row, 0, item);

            // 事件内容
            subId = (SubParamID)(d_ptr->ctx.almSegment->subParamID);
            paramId = paramInfo.getParamID(subId);
            alarmId = d_ptr->ctx.almSegment->alarmType;
            AlarmLimitIFace *alarmLimit = alertor.getAlarmLimitIFace(subId);
            if (alarmLimit)
            {
                priority = alarmLimit->getAlarmPriority(alarmId);
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
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(infoStr);
            d_ptr->eventTable->setItem(row, 1, item);
            row ++;
        }
    }
}
