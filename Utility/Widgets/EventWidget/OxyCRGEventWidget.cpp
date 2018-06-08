#include "OxyCRGEventWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "ITableWidget.h"
#include "IButton.h"
#include "IDropList.h"
#include "IMoveButton.h"
#include "EventDataDefine.h"
#include "EventStorageManager.h"
#include "TimeDate.h"
#include "ParamInfo.h"
#include "OxyCRGEventWaveWidget.h"
#include "EventInfoWidget.h"
#include "AlarmParamIFace.h"
#include "ParamManager.h"
#include "AlarmConfig.h"
#include "OxyCRGEventSetWidget.h"
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include "OxyCRGPageGenerator.h"
#include "TrendCache.h"
#include "WaveformCache.h"
#include "TimeDate.h"
#include "CO2Param.h"
#include "RecorderManager.h"

#define ITEM_HEIGHT     30
#define ITEM_WIDTH      100

OxyCRGEventWidget *OxyCRGEventWidget::_selfObj = NULL;

struct OxyCRGEventDataContex
{
    OxyCRGEventDataContex()
        :eventDataBuf(NULL),
          infoSegment(NULL),
          alamSegment(NULL),
          oxyCRGSegment(NULL)
    {}

    void reset()
    {
        if (eventDataBuf)
        {
            qFree(eventDataBuf);
            eventDataBuf = NULL;
        }
        infoSegment = NULL;
        trendSegments.clear();
        waveSegments.clear();
        alamSegment = NULL;
        oxyCRGSegment = NULL;
    }

    ~OxyCRGEventDataContex()
    {
        if (eventDataBuf)
        {
            qFree(eventDataBuf);
        }
    }

    char *eventDataBuf;
    EventInfoSegment *infoSegment;
    QVector<TrendDataSegment *> trendSegments;
    QVector<WaveformDataSegment *> waveSegments;
    AlarmInfoSegment *alamSegment;
    OxyCRGSegment *oxyCRGSegment;
};

class OxyCRGEventWidgetPrivate
{
public:
    OxyCRGEventWidgetPrivate()
        :eventTable(NULL)
    {
        waveInfo.id = WAVE_RESP;
        backend = eventStorageManager.backend();
    }

    bool parseEventData(int dataIndex)
    {
        if (!backend || dataIndex >= (int)backend->getBlockNR() || dataIndex < 0)
        {
            return false;
        }

        quint32 length = backend->getBlockDataLen(dataIndex);

        char *buf = (char *)qMalloc(length);
        if (!buf)
        {
            return false;
        }

        if (backend->readBlockData(dataIndex, buf, length) != length)
        {
            qFree(buf);
            return false;
        }

        ctx.reset();

        ctx.eventDataBuf = buf;

        char *parseBuffer = buf;
        bool parseEnd = false;
        while (!parseEnd)
        {
            EventSegmentType *eventType = (EventSegmentType *)parseBuffer;
            switch (*eventType)
            {
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
                ctx.trendSegments.append((TrendDataSegment *)parseBuffer);
                //find the location of the next event type
                parseBuffer += sizeof(TrendDataSegment) + ctx.trendSegments.last()->trendValueNum * sizeof(TrendValueSegment);
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
                ctx.alamSegment = (AlarmInfoSegment *) parseBuffer;
                parseBuffer += sizeof(AlarmInfoSegment);
                break;
            case EVENT_OXYCRG_SEGMENT:
                parseBuffer += sizeof(EventSegmentType);
                ctx.oxyCRGSegment = (OxyCRGSegment *) parseBuffer;
                parseBuffer += sizeof(OxyCRGSegment);
                break;
            default:
                qdebug("unknown segment type %d, stop parsing.",  *eventType);
                parseEnd = true;
                break;
            }
        }

        if (parseBuffer >= buf + length)
        {
            parseEnd = true;
        }
        return true;

    }

    ITableWidget *eventTable;
    IButton *detail;
    IDropList *type;
    IButton *upTable;
    IButton *downTable;

    EventInfoWidget *infoWidget;
    OxyCRGEventWaveWidget *waveWidget;
    IButton *eventList;
    IMoveButton *moveCoordinate;
    IMoveButton *moveCursor;
    IMoveButton *moveEvent;
    IButton *print;
    IButton *set;

    QWidget *tableWidget;
    QWidget *chartWidget;
    QStackedLayout *stackLayout;

    OxyCRGEventDataContex ctx;
    IStorageBackend *backend;

    QList<int> dataIndex;           // 当前选中事件项对应的数据所在索引
    QList<TrendGraphInfo> trendInfoList;    // 呼吸氧合数据链表
    OxyCRGWaveInfo waveInfo;                // 波形数据
};

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGEventWidget::~OxyCRGEventWidget()
{

}

void OxyCRGEventWidget::eventInfoUpdate()
{
    QString infoStr;
    QString timeInfoStr;
    QString indexStr;

    switch (d_ptr->ctx.oxyCRGSegment->type)
    {
    case OxyCRGEventECG:
    case OxyCRGEventSpO2:
    case OxyCRGEventResp:
    {
        SubParamID subId = (SubParamID)(d_ptr->ctx.alamSegment->subParamID);
        unsigned char alarmInfo = d_ptr->ctx.alamSegment->alarmInfo;
        infoStr = paramInfo.getSubParamName(subId);

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
        double limitValue = (double)d_ptr->ctx.alamSegment->alarmLimit / config.scale;
        infoStr += QString::number(limitValue);
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

void OxyCRGEventWidget::eventWaveUpdate()
{
//    d_ptr->waveWidget->setWaveTrendSegments(d_ptr->ctx.waveSegments, d_ptr->ctx.trendSegments);
    d_ptr->waveWidget->loadOxyCRGEventData(d_ptr->trendInfoList, d_ptr->waveInfo, d_ptr->ctx.infoSegment);
}

void OxyCRGEventWidget::loadTrendData()
{
    TrendGraphInfo trendInfoHR;
    TrendGraphInfo trendInfoSPO2;
    TrendGraphInfo trendInfoRR;

    trendInfoHR.startTime = d_ptr->ctx.infoSegment->timestamp - d_ptr->ctx.infoSegment->duration_before;
    trendInfoSPO2.startTime = d_ptr->ctx.infoSegment->timestamp - d_ptr->ctx.infoSegment->duration_before;
    trendInfoRR.startTime = d_ptr->ctx.infoSegment->timestamp - d_ptr->ctx.infoSegment->duration_before;

    trendInfoHR.endTime = d_ptr->ctx.infoSegment->timestamp + d_ptr->ctx.infoSegment->duration_after;
    trendInfoSPO2.endTime = d_ptr->ctx.infoSegment->timestamp + d_ptr->ctx.infoSegment->duration_after;
    trendInfoRR.endTime = d_ptr->ctx.infoSegment->timestamp + d_ptr->ctx.infoSegment->duration_after;

    trendInfoHR.subParamID = SUB_PARAM_HR_PR;
    trendInfoSPO2.subParamID = SUB_PARAM_SPO2;
    trendInfoRR.subParamID = SUB_PARAM_RR_BR;

    trendInfoHR.unit = paramManager.getSubParamUnit(paramInfo.getParamID(SUB_PARAM_HR_PR), SUB_PARAM_HR_PR);
    trendInfoSPO2.unit = paramManager.getSubParamUnit(paramInfo.getParamID(SUB_PARAM_SPO2), SUB_PARAM_SPO2);
    trendInfoRR.unit = paramManager.getSubParamUnit(paramInfo.getParamID(SUB_PARAM_RR_BR), SUB_PARAM_RR_BR);

    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_HR_PR, trendInfoHR.unit);
    trendInfoHR.scale.min = (double)config.lowLimit / config.scale;
    trendInfoHR.scale.max = (double)config.highLimit / config.scale;
    config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_SPO2, trendInfoSPO2.unit);
    trendInfoSPO2.scale.min = (double)config.lowLimit / config.scale;
    trendInfoSPO2.scale.max = (double)config.highLimit / config.scale;
    config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_RR_BR, trendInfoRR.unit);
    trendInfoRR.scale.min = (double)config.lowLimit / config.scale;
    trendInfoRR.scale.max = (double)config.highLimit / config.scale;

    TrendGraphData dataHR;
    TrendGraphData dataSPO2;
    TrendGraphData dataRR;
    TrendDataSegment *trendSegment;
    for (int i = 0; i < d_ptr->ctx.trendSegments.count(); i ++)
    {
        unsigned t = d_ptr->ctx.trendSegments.at(i)->timestamp;
        trendSegment = d_ptr->ctx.trendSegments.at(i);
        for(int j = 0; j < trendSegment->trendValueNum; j ++)
        {
            TrendValueSegment valueSegment = trendSegment->values[j];
            switch((SubParamID)valueSegment.subParamId)
            {
            case SUB_PARAM_HR_PR:
                dataHR.data = valueSegment.value;
                dataHR.timestamp = t;
                trendInfoHR.trendData.append(dataHR);
                break;
            case SUB_PARAM_SPO2:
                dataSPO2.data = valueSegment.value;
                dataSPO2.timestamp = t;
                trendInfoSPO2.trendData.append(dataSPO2);
                break;
            case SUB_PARAM_RR_BR:
                dataRR.data = valueSegment.value;
                dataRR.timestamp = t;
                trendInfoRR.trendData.append(dataRR);
                break;
            default:
                break;
            }
        }
    }
    d_ptr->trendInfoList.clear();
    d_ptr->trendInfoList.append(trendInfoHR);
    d_ptr->trendInfoList.append(trendInfoSPO2);
    d_ptr->trendInfoList.append(trendInfoRR);
}

void OxyCRGEventWidget::loadWaveformData()
{
    d_ptr->waveInfo.reset();
    waveformCache.getRange(d_ptr->waveInfo.id, d_ptr->waveInfo.minWaveValue, d_ptr->waveInfo.maxWaveValue);
    d_ptr->waveInfo.sampleRate = waveformCache.getSampleRate(d_ptr->waveInfo.id);
    waveformCache.getBaseline(d_ptr->waveInfo.id, d_ptr->waveInfo.waveBaseLine);
    if (d_ptr->waveInfo.id == WAVE_RESP)
    {
        d_ptr->waveInfo.waveInfo.resp.zoom = RESP_ZOOM_X100;
    }
    else if (d_ptr->waveInfo.id == WAVE_CO2)
    {
        d_ptr->waveInfo.waveInfo.co2.zoom = CO2_DISPLAY_ZOOM_20;
    }
    WaveformDataSegment *waveData;
    for (int i = 0; d_ptr->ctx.waveSegments.count(); i ++)
    {
        waveData = d_ptr->ctx.waveSegments.at(i);
        if (waveData->waveID == d_ptr->waveInfo.id)
        {
            break;
        }
    }
    for (int j = 0; j < waveData->waveNum; j ++)
    {
        WaveDataType waveValue = waveData->waveData[j];
        d_ptr->waveInfo.waveData.append(waveValue);
    }
}

void OxyCRGEventWidget::waveWidgetTrend1(bool isRR)
{
    d_ptr->waveWidget->setWaveWidgetTrend1(isRR);
}

void OxyCRGEventWidget::waveWidgetCompressed(WaveformID id)
{
    d_ptr->waveInfo.id = id;
    loadWaveformData();
    eventWaveUpdate();
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void OxyCRGEventWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

    d_ptr->stackLayout->setCurrentIndex(0);
    _loadOxyCRGEventData();    
    if (d_ptr->eventTable->rowCount() == 0)
    {
        d_ptr->detail->setEnabled(false);
    }
    else
    {
        d_ptr->detail->setEnabled(true);
    }
}

void OxyCRGEventWidget::_upMoveEventReleased()
{
    if (d_ptr->eventTable->currentRow() != 0)
    {
        d_ptr->eventTable->selectRow(d_ptr->eventTable->currentRow() - 1);
    }
}

void OxyCRGEventWidget::_downMoveEventReleased()
{
    if (d_ptr->eventTable->currentRow() != d_ptr->eventTable->rowCount() - 1)
    {
        d_ptr->eventTable->selectRow(d_ptr->eventTable->currentRow() + 1);
    }
}

void OxyCRGEventWidget::_detailReleased()
{
    d_ptr->stackLayout->setCurrentIndex(1);
    if (d_ptr->dataIndex.count() == 0)
    {
        return;
    }
    d_ptr->parseEventData(d_ptr->dataIndex.at(d_ptr->eventTable->currentRow()));
    loadTrendData();
    loadWaveformData();
    eventInfoUpdate();
    eventWaveUpdate();
    d_ptr->eventList->setFocus();
}

void OxyCRGEventWidget::_eventListReleased()
{
    d_ptr->stackLayout->setCurrentIndex(0);
}

void OxyCRGEventWidget::_leftMoveEvent()
{
    if (d_ptr->eventTable->currentRow() != 0)
    {
        d_ptr->eventTable->selectRow(d_ptr->eventTable->currentRow() - 1);
    }

    d_ptr->parseEventData(d_ptr->dataIndex.at(d_ptr->eventTable->currentRow()));
    eventInfoUpdate();
    eventWaveUpdate();
}

void OxyCRGEventWidget::_rightMoveEvent()
{
    if (d_ptr->eventTable->currentRow() != d_ptr->eventTable->rowCount() - 1)
    {
        d_ptr->eventTable->selectRow(d_ptr->eventTable->currentRow() + 1);
    }

    d_ptr->parseEventData(d_ptr->dataIndex.at(d_ptr->eventTable->currentRow()));
    eventInfoUpdate();
    eventWaveUpdate();
}

void OxyCRGEventWidget::_setReleased()
{
    oxyCRGEventSetWidget.autoShow();
}

void OxyCRGEventWidget::_printReleased()
{
    QList<TrendGraphInfo> trendInfos;
    trendInfos.append(d_ptr->trendInfoList.at(0));
    trendInfos.append(d_ptr->trendInfoList.at(1));
    RecordPageGenerator *generator = new OxyCRGPageGenerator(trendInfos, d_ptr->waveInfo);
    recorderManager.addPageGenerator(generator);
}

void OxyCRGEventWidget::_loadOxyCRGEventData()
{
    d_ptr->dataIndex.clear();
    int eventNum = d_ptr->backend->getBlockNR();

    unsigned t = 0;
    QString timeStr;
    QString dataStr;
    QString infoStr;
    QTableWidgetItem *item;
    SubParamID subId;
    unsigned char alarmInfo;
    int row = 0;
    for (int i = eventNum - 1; i >= 0; i --)
    {
        if (d_ptr->parseEventData(i))
        {
            if (d_ptr->ctx.infoSegment->type != EventOxyCRG)
            {
                continue;
            }

            t = d_ptr->ctx.infoSegment->timestamp;

            timeDate.getDate(t, dataStr, true);
            timeDate.getTime(t, timeStr, true);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(dataStr + " " + timeStr);
            d_ptr->eventTable->setRowCount(row + 1);
            d_ptr->eventTable->setItem(row, 0, item);

            switch (d_ptr->ctx.oxyCRGSegment->type)
            {
            case OxyCRGEventECG:
            case OxyCRGEventSpO2:
            case OxyCRGEventResp:
                subId = (SubParamID)(d_ptr->ctx.alamSegment->subParamID);
                alarmInfo = d_ptr->ctx.alamSegment->alarmInfo;
                infoStr = paramInfo.getSubParamName(subId);
                infoStr += " ";
                if ((alarmInfo >> 1) & 0x1)
                {
                    infoStr += trs("Upper");
                }
                else
                {
                    infoStr += trs("Lower");
                }
                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(infoStr);
                d_ptr->eventTable->setItem(row, 1, item);
                break;
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
 * 构造。
 *************************************************************************************************/
OxyCRGEventWidget::OxyCRGEventWidget() : d_ptr(new OxyCRGEventWidgetPrivate())
{
    setTitleBarText(trs("OxyCRGEventReview"));
    int windowWidth = windowManager.getPopMenuWidth();
    int windowHeight = windowManager.getPopMenuHeight();
    setFixedSize(windowWidth, windowHeight);

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);

    // OxyCRG event table widget
    d_ptr->eventTable = new ITableWidget();
    d_ptr->eventTable->setColumnCount(2);
    d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);
    d_ptr->eventTable->verticalHeader()->setVisible(false);
    d_ptr->eventTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d_ptr->eventTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->eventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ptr->eventTable->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->eventTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QStringList tableTitle;
    tableTitle << trs("Time") << trs("Incident");
    d_ptr->eventTable->setHorizontalHeaderLabels(tableTitle);

    d_ptr->detail = new IButton(trs("WaveInfo"));
    d_ptr->detail->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->detail->setFont(font);
    connect(d_ptr->detail, SIGNAL(realReleased()), this, SLOT(_detailReleased()));

    d_ptr->type = new IDropList(trs("Type"));
    d_ptr->type->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->type->setFont(font);

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
    hTableLayout->addWidget(d_ptr->detail);
    hTableLayout->addWidget(d_ptr->type);
    hTableLayout->addWidget(d_ptr->upTable);
    hTableLayout->addWidget(d_ptr->downTable);

    QVBoxLayout *vTableLayout = new QVBoxLayout();
    vTableLayout->addWidget(d_ptr->eventTable);
    vTableLayout->addLayout(hTableLayout);

    d_ptr->tableWidget = new QWidget();
    d_ptr->tableWidget->setLayout(vTableLayout);

    // OxyCRG event wave widget
    d_ptr->infoWidget = new EventInfoWidget;
    d_ptr->infoWidget->setFocusPolicy(Qt::NoFocus);

    d_ptr->waveWidget = new OxyCRGEventWaveWidget;
    d_ptr->waveWidget->setFocusPolicy(Qt::NoFocus);

    d_ptr->eventList = new IButton(trs("EventList"));
    d_ptr->eventList->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->eventList->setFont(font);
    connect(d_ptr->eventList, SIGNAL(realReleased()), this, SLOT(_eventListReleased()));

    d_ptr->moveCoordinate = new IMoveButton(trs("MoveCoordinate"));
    d_ptr->moveCoordinate->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->moveCoordinate->setFont(font);
    connect(d_ptr->moveCoordinate, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveCoordinate()));
    connect(d_ptr->moveCoordinate, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveCoordinate()));

    d_ptr->moveCursor = new IMoveButton(trs("MoveCursor"));
    d_ptr->moveCursor->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->moveCursor->setFont(font);
    connect(d_ptr->moveCursor, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveCursor()));
    connect(d_ptr->moveCursor, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveCursor()));

    d_ptr->moveEvent = new IMoveButton(trs("MoveEvent"));
    d_ptr->moveEvent->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->moveEvent->setFont(font);
    connect(d_ptr->moveEvent, SIGNAL(leftMove()), this, SLOT(_leftMoveEvent()));
    connect(d_ptr->moveEvent, SIGNAL(rightMove()), this, SLOT(_rightMoveEvent()));

    d_ptr->print = new IButton(trs("Print"));
    d_ptr->print->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->print->setFont(font);
    connect(d_ptr->print, SIGNAL(realReleased()), this, SLOT(_printReleased()));

    d_ptr->set = new IButton(trs("Set"));
    d_ptr->set->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->set->setFont(font);
    connect(d_ptr->set, SIGNAL(realReleased()), this, SLOT(_setReleased()));

    QHBoxLayout *hWaveLayout = new QHBoxLayout();
    hWaveLayout->setMargin(0);
    hWaveLayout->setSpacing(2);
    hWaveLayout->addWidget(d_ptr->eventList);
    hWaveLayout->addWidget(d_ptr->moveCoordinate);
    hWaveLayout->addWidget(d_ptr->moveCursor);
    hWaveLayout->addWidget(d_ptr->moveEvent);
    hWaveLayout->addWidget(d_ptr->print);
    hWaveLayout->addWidget(d_ptr->set);

    QVBoxLayout *vWaveLayout = new QVBoxLayout();
    vWaveLayout->addWidget(d_ptr->infoWidget, 1);
    vWaveLayout->addSpacing(1);
    vWaveLayout->addWidget(d_ptr->waveWidget, 15);
    vWaveLayout->addLayout(hWaveLayout);

    d_ptr->chartWidget = new QWidget();
    d_ptr->chartWidget->setLayout(vWaveLayout);

    d_ptr->stackLayout = new QStackedLayout();
    d_ptr->stackLayout->addWidget(d_ptr->tableWidget);
    d_ptr->stackLayout->addWidget(d_ptr->chartWidget);

    contentLayout->addLayout(d_ptr->stackLayout);
}
