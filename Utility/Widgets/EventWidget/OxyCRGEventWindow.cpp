/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/6
 **/

#include "OxyCRGEventWindow.h"
#include "TableView.h"
#include "Button.h"
#include "LanguageManager.h"
#include "ComboBox.h"
#include "EventInfoWidget.h"
#include "OxyCRGEventWaveWidget.h"
#include <QStackedLayout>
#include "IStorageBackend.h"
#include "EventStorageManager.h"
#include "Debug.h"
#include "TableHeaderView.h"
#include "TimeDate.h"
#include "ParamInfo.h"
#include "EventReviewModel.h"
#include "AlarmConfig.h"
#include "OxyCRGPageGenerator.h"
#include "TrendCache.h"
#include "WaveformCache.h"
#include "ParamManager.h"
#include "RecorderManager.h"
#include "OxyCRGEventSetWindow.h"
#include "WindowManager.h"
#include "DataStorageDefine.h"
#include "MoveButton.h"
#include <QTimerEvent>
#include <QLabel>

#define STOP_PRINT_TIMEOUT          (100)

struct OxyCRGEventContex
{
    OxyCRGEventContex()
        : eventDataBuf(NULL),
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

    ~OxyCRGEventContex()
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

class OxyCRGEventWindowPrivate
{
public:
    OxyCRGEventWindowPrivate()
        : table(NULL), model(NULL), typeCbo(NULL),
          upPageBtn(NULL), downPageBtn(NULL), infoWidget(NULL),
          waveWidget(NULL), eventListBtn(NULL), coordinateMoveBtn(NULL),
          cursorMoveBtn(NULL), eventMoveBtn(NULL), printBtn(NULL),
          setBtn(NULL), tableWidget(NULL), chartWidget(NULL),
          stackLayout(NULL), backend(NULL), eventNum(0),
          curDisplayEventNum(0), isHistory(false),
          printTimerId(-1),
          waitTimerId(-1),
          isWait(false),
          timeoutNum(0),
          generator(NULL)
    {
        waveInfo.id = WAVE_RESP;
        backend = eventStorageManager.backend();
    }


    bool parseEventData(int dataIndex)
    {
        int blockNR = static_cast<int>(backend->getBlockNR());
        if (!backend || dataIndex >= blockNR || dataIndex < 0)
        {
            return false;
        }

        quint32 length = backend->getBlockDataLen(dataIndex);

        char *buf = reinterpret_cast<char *>(qMalloc(length));
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
            EventSegmentType *eventType = reinterpret_cast<EventSegmentType *>(parseBuffer);
            switch (*eventType)
            {
            case EVENT_INFO_SEGMENT:
                // skip the offset of the segment type field
                parseBuffer += sizeof(EventSegmentType);
                ctx.infoSegment = reinterpret_cast<EventInfoSegment *>(parseBuffer);
                // find the location of the next event type
                parseBuffer += sizeof(EventInfoSegment);
                break;
            case EVENT_TRENDDATA_SEGMENT:
                // skip the offset of the segment type field
                parseBuffer += sizeof(EventSegmentType);
                ctx.trendSegments.append(reinterpret_cast<TrendDataSegment *>(parseBuffer));
                // find the location of the next event type
                parseBuffer += sizeof(TrendDataSegment) +
                        ctx.trendSegments.last()->trendValueNum * sizeof(TrendValueSegment);
                break;
            case EVENT_WAVEFORM_SEGMENT:
                // skip the offset of the segment type field
                parseBuffer += sizeof(EventSegmentType);
                ctx.waveSegments.append(reinterpret_cast<WaveformDataSegment *>(parseBuffer));
                // find the location of the next event type
                parseBuffer += sizeof(WaveformDataSegment) + ctx.waveSegments.last()->waveNum * sizeof(WaveDataType);
                break;
            case EVENT_ALARM_INFO_SEGMENT:
                parseBuffer += sizeof(EventSegmentType);
                ctx.alamSegment = reinterpret_cast<AlarmInfoSegment *>(parseBuffer);
                parseBuffer += sizeof(AlarmInfoSegment);
                break;
            case EVENT_OXYCRG_SEGMENT:
                parseBuffer += sizeof(EventSegmentType);
                ctx.oxyCRGSegment = reinterpret_cast<OxyCRGSegment *>(parseBuffer);
                parseBuffer += sizeof(OxyCRGSegment);
                break;
            default:
                qdebug("unknown segment type %d, stop parsing.",  *eventType);
                parseEnd = true;
                break;
            }
        }

        return true;
    }

    void loadOxyCRGEventData(void);

    void loadTrendData(void);

    void loadWaveformData(void);

    void eventInfoUpdate(int curRow);

    void eventWaveUpdate(void);

public:
    TableView *table;
    EventReviewModel *model;
    ComboBox *typeCbo;
    Button *upPageBtn;
    Button *downPageBtn;

    EventInfoWidget *infoWidget;
    OxyCRGEventWaveWidget *waveWidget;
    Button *eventListBtn;
    MoveButton *coordinateMoveBtn;
    MoveButton *cursorMoveBtn;
    MoveButton *eventMoveBtn;
    Button *printBtn;
    Button *setBtn;

    QWidget *tableWidget;
    QWidget *chartWidget;
    QStackedLayout *stackLayout;

    OxyCRGEventContex ctx;
    IStorageBackend *backend;
    int eventNum;                       // 存储总事件数
    int curDisplayEventNum;             // 当前显示事件数

    QList<int> dataIndex;                       // 当前选中事件项对应的数据所在索引
    QList<TrendGraphInfo> trendInfoList;        // 呼吸氧合数据链表
    OxyCRGWaveInfo waveInfo;                    // 波形数据
    QString eventTitle;                         // 事件标题

    bool isHistory;                             // 历史回顾标志
    QString historyDataPath;                    // 历史数据路径

    int printTimerId;
    int waitTimerId;
    bool isWait;
    int timeoutNum;
    RecordPageGenerator *generator;
};

OxyCRGEventWindow *OxyCRGEventWindow::getInstance()
{
    static OxyCRGEventWindow *instance = NULL;
    if (!instance)
    {
        instance = new OxyCRGEventWindow;
    }
    return instance;
}

OxyCRGEventWindow::~OxyCRGEventWindow()
{
}

void OxyCRGEventWindow::waveWidgetTrend1(bool isRR)
{
    d_ptr->waveWidget->setWaveWidgetTrend1(isRR);
}

void OxyCRGEventWindow::waveWidgetCompressed(WaveformID id)
{
    d_ptr->waveInfo.id = id;
    d_ptr->loadWaveformData();
    d_ptr->eventWaveUpdate();
}

void OxyCRGEventWindow::setHistoryDataPath(QString path)
{
    d_ptr->historyDataPath = path;
}

void OxyCRGEventWindow::setHistoryData(bool flag)
{
    // 动态内存释放
    if (d_ptr->isHistory)
    {
        delete d_ptr->backend;
    }

    d_ptr->isHistory = flag;
    if ((d_ptr->historyDataPath != "") && d_ptr->isHistory)
    {
        d_ptr->backend =  StorageManager::open(d_ptr->historyDataPath + EVENT_DATA_FILE_NAME, QIODevice::ReadWrite);
    }
    else
    {
        d_ptr->backend = eventStorageManager.backend();
    }
}

void OxyCRGEventWindow::showEvent(QShowEvent *ev)
{
    Dialog::showEvent(ev);

    d_ptr->stackLayout->setCurrentIndex(0);
    d_ptr->loadOxyCRGEventData();

    if (d_ptr->table->model()->rowCount() == 0)
    {
        d_ptr->table->setFocusPolicy(Qt::NoFocus);
    }
    else
    {
        d_ptr->table->setFocusPolicy(Qt::StrongFocus);
    }
}

void OxyCRGEventWindow::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->printTimerId == ev->timerId())
    {
        if (!recorderManager.isPrinting() || d_ptr->timeoutNum == 10)
        {
            if (!recorderManager.isPrinting())
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
        else if (d_ptr->waitTimerId == ev->timerId())
        {
            d_ptr->printTimerId = startTimer(STOP_PRINT_TIMEOUT);
            killTimer(d_ptr->waitTimerId);
            d_ptr->waitTimerId = -1;
            d_ptr->isWait = false;
        }
    }
}

void OxyCRGEventWindow::waveInfoReleased(QModelIndex index)
{
    waveInfoReleased(index.row());
}

void OxyCRGEventWindow::waveInfoReleased(int index)
{
    d_ptr->stackLayout->setCurrentIndex(1);
    if (d_ptr->dataIndex.count() == 0)
    {
        return;
    }
    d_ptr->table->selectRow(index);
    d_ptr->parseEventData(d_ptr->dataIndex.at(index));
    d_ptr->loadTrendData();
    d_ptr->loadWaveformData();
    d_ptr->eventInfoUpdate(index);
    d_ptr->eventWaveUpdate();
    d_ptr->eventListBtn->setFocus();
}

void OxyCRGEventWindow::eventListReleased()
{
    d_ptr->stackLayout->setCurrentIndex(0);
}

void OxyCRGEventWindow::leftMoveEvent()
{
    int curIndex = d_ptr->table->currentIndex().row();
    if (curIndex != 0)
    {
        curIndex--;
        d_ptr->table->selectRow(curIndex);

        d_ptr->parseEventData(d_ptr->dataIndex.at(curIndex));
        d_ptr->eventInfoUpdate(curIndex);
        d_ptr->eventWaveUpdate();
    }
}

void OxyCRGEventWindow::rightMoveEvent()
{
    int curIndex = d_ptr->table->currentIndex().row();
    if (curIndex != d_ptr->curDisplayEventNum - 1)
    {
        curIndex++;
        d_ptr->table->selectRow(curIndex);

        d_ptr->parseEventData(d_ptr->dataIndex.at(curIndex));
        d_ptr->eventInfoUpdate(curIndex);
        d_ptr->eventWaveUpdate();
    }
}

void OxyCRGEventWindow::printReleased()
{
    QList<TrendGraphInfo> trendInfos;
    trendInfos.append(d_ptr->trendInfoList.at(0));
    trendInfos.append(d_ptr->trendInfoList.at(1));
    RecordPageGenerator *generator = new OxyCRGPageGenerator(trendInfos, d_ptr->waveInfo, d_ptr->eventTitle);
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

void OxyCRGEventWindow::setReleased()
{
    windowManager.showWindow(&oxyCRGEventSetWindow, WindowManager::ShowBehaviorModal);
}

OxyCRGEventWindow::OxyCRGEventWindow()
    : Dialog(), d_ptr(new OxyCRGEventWindowPrivate())
{
    setWindowTitle(trs("OxyCRGEventReview"));

    // 事件表格窗口
    d_ptr->table = new TableView();
    TableHeaderView *horizontalHeader = new TableHeaderView(Qt::Horizontal);
    d_ptr->table->setHorizontalHeader(horizontalHeader);
    horizontalHeader->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->table->verticalHeader()->setVisible(false);
    d_ptr->table->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->table->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ptr->table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->table->setShowGrid(false);
    d_ptr->model = new EventReviewModel();
    d_ptr->table->setModel(d_ptr->model);
    connect(d_ptr->table, SIGNAL(clicked(QModelIndex)), this, SLOT(waveInfoReleased(QModelIndex)));
    connect(d_ptr->table, SIGNAL(rowClicked(int)), this, SLOT(waveInfoReleased(int)));

    QLabel *typeLabel = new QLabel(trs("Type"));
    d_ptr->typeCbo = new ComboBox();
    d_ptr->typeCbo->addItem(trs("All"));

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
    hTableLayout->addStretch(5);
    hTableLayout->addWidget(d_ptr->upPageBtn, 1);
    hTableLayout->addWidget(d_ptr->downPageBtn, 1);

    QVBoxLayout *vTableLayout = new QVBoxLayout();
    vTableLayout->addWidget(d_ptr->table);
    vTableLayout->addLayout(hTableLayout);

    d_ptr->tableWidget = new QWidget();
    d_ptr->tableWidget->setLayout(vTableLayout);

    // 事件波形窗口;
    d_ptr->infoWidget = new EventInfoWidget();
    d_ptr->infoWidget->setFocusPolicy(Qt::NoFocus);

    d_ptr->waveWidget = new OxyCRGEventWaveWidget();
    d_ptr->waveWidget->setFocusPolicy(Qt::NoFocus);

    d_ptr->eventListBtn = new Button(trs("EventList"));
    d_ptr->eventListBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->eventListBtn, SIGNAL(released()), this, SLOT(eventListReleased()));

    d_ptr->coordinateMoveBtn = new MoveButton(trs("Time"));
    d_ptr->coordinateMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->coordinateMoveBtn, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveCoordinate()));
    connect(d_ptr->coordinateMoveBtn, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveCoordinate()));

    d_ptr->cursorMoveBtn = new MoveButton(trs("Scroll"));
    d_ptr->cursorMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->cursorMoveBtn, SIGNAL(leftMove()), d_ptr->waveWidget, SLOT(leftMoveCursor()));
    connect(d_ptr->cursorMoveBtn, SIGNAL(rightMove()), d_ptr->waveWidget, SLOT(rightMoveCursor()));

    d_ptr->eventMoveBtn = new MoveButton(trs("Event"));
    d_ptr->eventMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->eventMoveBtn, SIGNAL(leftMove()), this, SLOT(leftMoveEvent()));
    connect(d_ptr->eventMoveBtn, SIGNAL(rightMove()), this, SLOT(rightMoveEvent()));

    d_ptr->printBtn = new Button(trs("Print"));
    d_ptr->printBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->printBtn, SIGNAL(released()), this, SLOT(printReleased()));

    d_ptr->setBtn = new Button(trs("Set"));
    d_ptr->setBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->setBtn, SIGNAL(released()), this, SLOT(setReleased()));

    QHBoxLayout *hWaveLayout = new QHBoxLayout();
    hWaveLayout->addWidget(d_ptr->eventListBtn, 2);
    hWaveLayout->addWidget(d_ptr->coordinateMoveBtn, 2);
    hWaveLayout->addWidget(d_ptr->cursorMoveBtn, 2);
    hWaveLayout->addWidget(d_ptr->eventMoveBtn, 2);
    hWaveLayout->addWidget(d_ptr->printBtn, 2);
    hWaveLayout->addWidget(d_ptr->setBtn, 2);

    QVBoxLayout *vWaveLayout = new QVBoxLayout();
    vWaveLayout->addWidget(d_ptr->infoWidget, 1);
    vWaveLayout->addWidget(d_ptr->waveWidget, 15);
    vWaveLayout->addLayout(hWaveLayout);
    vWaveLayout->setSpacing(0);

    d_ptr->chartWidget = new QWidget();
    d_ptr->chartWidget->setLayout(vWaveLayout);

    d_ptr->stackLayout = new QStackedLayout();
    d_ptr->stackLayout->addWidget(d_ptr->tableWidget);
    d_ptr->stackLayout->addWidget(d_ptr->chartWidget);

    setWindowLayout(d_ptr->stackLayout);

    int width = windowManager.getPopWindowWidth();
    int height = windowManager.getPopWindowHeight();
    setFixedSize(width, height);
}

void OxyCRGEventWindowPrivate::loadOxyCRGEventData()
{
    dataIndex.clear();
    eventNum = backend->getBlockNR();

    unsigned t = 0;
    QString timeStr;
    QString dateStr;
    QString infoStr;
    SubParamID subId;
    unsigned char alarmInfo;
    QList<QString> timeList;
    QList<QString> eventList;
    for (int i = eventNum - 1; i >= 0; i --)
    {
        if (parseEventData(i))
        {
            if (ctx.infoSegment->type != EventOxyCRG)
            {
                continue;
            }

            t = ctx.infoSegment->timestamp;

            timeDate.getDate(t, dateStr, true);
            timeDate.getTime(t, timeStr, true);
            QString timeItemStr = dateStr + " " + timeStr;

            switch (ctx.oxyCRGSegment->type)
            {
            case OxyCRGEventECG:
            case OxyCRGEventSpO2:
            case OxyCRGEventResp:
                subId = (SubParamID)(ctx.alamSegment->subParamID);
                alarmInfo = ctx.alamSegment->alarmInfo;
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
                timeList.append(timeItemStr);
                eventList.append(infoStr);
                break;
            default:
                break;
            }
            dataIndex.append(i);
        }
    }
    curDisplayEventNum = timeList.count();
    model->updateEvent(timeList, eventList);
}

void OxyCRGEventWindowPrivate::loadTrendData()
{
    TrendGraphInfo trendInfoHR;
    TrendGraphInfo trendInfoSPO2;
    TrendGraphInfo trendInfoRR;

    trendInfoHR.startTime = ctx.infoSegment->timestamp - ctx.infoSegment->duration_before;
    trendInfoSPO2.startTime = ctx.infoSegment->timestamp - ctx.infoSegment->duration_before;
    trendInfoRR.startTime = ctx.infoSegment->timestamp - ctx.infoSegment->duration_before;

    trendInfoHR.endTime = ctx.infoSegment->timestamp + ctx.infoSegment->duration_after;
    trendInfoSPO2.endTime = ctx.infoSegment->timestamp + ctx.infoSegment->duration_after;
    trendInfoRR.endTime = ctx.infoSegment->timestamp + ctx.infoSegment->duration_after;

    trendInfoHR.subParamID = SUB_PARAM_HR_PR;
    trendInfoSPO2.subParamID = SUB_PARAM_SPO2;
    trendInfoRR.subParamID = SUB_PARAM_RR_BR;

    trendInfoHR.unit = paramManager.getSubParamUnit(paramInfo.getParamID(SUB_PARAM_HR_PR), SUB_PARAM_HR_PR);
    trendInfoSPO2.unit = paramManager.getSubParamUnit(paramInfo.getParamID(SUB_PARAM_SPO2), SUB_PARAM_SPO2);
    trendInfoRR.unit = paramManager.getSubParamUnit(paramInfo.getParamID(SUB_PARAM_RR_BR), SUB_PARAM_RR_BR);

    ParamRulerConfig config = alarmConfig.getParamRulerConfig(SUB_PARAM_HR_PR, trendInfoHR.unit);
    trendInfoHR.scale.min = config.downRuler;
    trendInfoHR.scale.max = config.upRuler;
    trendInfoHR.scale.scale = config.scale;
    config = alarmConfig.getParamRulerConfig(SUB_PARAM_SPO2, trendInfoSPO2.unit);
    trendInfoSPO2.scale.min = config.downRuler;
    trendInfoSPO2.scale.max = config.upRuler;
    trendInfoSPO2.scale.scale = config.scale;
    config = alarmConfig.getParamRulerConfig(SUB_PARAM_RR_BR, trendInfoRR.unit);
    trendInfoRR.scale.min = config.downRuler;
    trendInfoRR.scale.max = config.upRuler;
    trendInfoRR.scale.scale = config.scale;

    TrendGraphData dataHR;
    TrendGraphData dataSPO2;
    TrendGraphData dataRR;
    TrendDataSegment *trendSegment;
    for (int i = 0; i < ctx.trendSegments.count(); i ++)
    {
        unsigned t = ctx.trendSegments.at(i)->timestamp;
        trendSegment = ctx.trendSegments.at(i);
        for (int j = 0; j < trendSegment->trendValueNum; j ++)
        {
            TrendValueSegment valueSegment = trendSegment->values[j];
            switch ((SubParamID)valueSegment.subParamId)
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
    trendInfoList.clear();
    trendInfoList.append(trendInfoHR);
    trendInfoList.append(trendInfoSPO2);
    trendInfoList.append(trendInfoRR);
}

void OxyCRGEventWindowPrivate::loadWaveformData()
{
    waveInfo.reset();
    waveformCache.getRange(waveInfo.id, &waveInfo.minWaveValue, &waveInfo.maxWaveValue);
    waveInfo.sampleRate = waveformCache.getSampleRate(waveInfo.id);
    waveInfo.waveBaseLine = waveformCache.getBaseline(waveInfo.id);
    if (waveInfo.id == WAVE_RESP)
    {
        waveInfo.waveInfo.resp.zoom = RESP_ZOOM_X100;
    }
    else if (waveInfo.id == WAVE_CO2)
    {
        waveInfo.waveInfo.co2.zoom = CO2_DISPLAY_ZOOM_20;
    }
    WaveformDataSegment *waveData;
    for (int i = 0; ctx.waveSegments.count(); i ++)
    {
        waveData = ctx.waveSegments.at(i);
        if (waveData->waveID == waveInfo.id)
        {
            break;
        }
    }
    for (int j = 0; j < waveData->waveNum; j ++)
    {
        WaveDataType waveValue = waveData->waveData[j];
        waveInfo.waveData.append(waveValue);
    }
}

void OxyCRGEventWindowPrivate::eventInfoUpdate(int curRow)
{
    QString infoStr;
    QString timeInfoStr;
    QString indexStr;

    switch (ctx.oxyCRGSegment->type)
    {
    case OxyCRGEventECG:
    case OxyCRGEventSpO2:
    case OxyCRGEventResp:
    {
        SubParamID subId = (SubParamID)(ctx.alamSegment->subParamID);
        unsigned char alarmInfo = ctx.alamSegment->alarmInfo;
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
        double limitValue = static_cast<double>(ctx.alamSegment->alarmLimit / config.scale);
        infoStr += QString::number(limitValue);
        infoStr += " ";
        infoStr += trs(Unit::getSymbol(type));
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

    eventTitle = infoStr;
    infoWidget->loadDataInfo(infoStr, timeInfoStr, indexStr);
}

void OxyCRGEventWindowPrivate::eventWaveUpdate()
{
    waveWidget->loadOxyCRGEventData(trendInfoList, waveInfo, ctx.infoSegment);
}
