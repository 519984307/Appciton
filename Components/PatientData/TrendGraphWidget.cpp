#include "TrendGraphWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "IButton.h"
#include "LanguageManager.h"
#include "TrendWaveWidget.h"
#include "TrendGraphSetWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "EventReviewWindow.h"
#include "OxyCRGEventWidget.h"
#include "TrendGraphPageGenerator.h"
#include "RecorderManager.h"

#define ITEM_HEIGHT             30
#define ITEM_WIDTH              100

TrendGraphWidget *TrendGraphWidget::_selfObj = NULL;

TrendGraphWidget::~TrendGraphWidget()
{
}

void TrendGraphWidget::waveNumberChange(int num)
{
    if (num > 0 && num <= 3)
    {
        _waveWidget->setWaveNumber(num);
    }
}

void TrendGraphWidget::timeIntervalChange(int timeInterval)
{
    _waveWidget->setTimeInterval((ResolutionRatio)timeInterval);
}

void TrendGraphWidget::updateTrendGraph()
{
    _waveWidget->updateTimeRange();
}

void TrendGraphWidget::setSubWidgetRulerLimit(SubParamID id, int down, int up)
{
    _waveWidget->setRulerLimit(id, down, up);
}

void TrendGraphWidget::_trendGraphSetReleased()
{
    trendGraphSetWidget.autoShow();
}

void TrendGraphWidget::_upReleased()
{
    _waveWidget->pageUpParam();
}

void TrendGraphWidget::_downReleased()
{
    _waveWidget->pageDownParam();
}

void TrendGraphWidget::_printReleased()
{
    //test code
    const QList<TrendDataPackage *> trendDataPackList = _waveWidget->getTrendDatapacketList();
    if(trendDataPackList.isEmpty())
    {
        return;
    }
    QList<TrendGraphInfo> infos;
    TrendGraphInfo hrInfo;
    TrendGraphInfo spo2Info;
    TrendGraphInfo artInfo;
    TrendGraphInfo nibpInfo;
    hrInfo.subParamID = SUB_PARAM_HR_PR;
    hrInfo.scale.max = 100;
    hrInfo.scale.min = 50;
    hrInfo.unit = UNIT_BPM;
    spo2Info.subParamID = SUB_PARAM_SPO2;
    spo2Info.scale.max = 100;
    spo2Info.scale.min = 85;
    spo2Info.unit = UNIT_PERCENT;
    artInfo.subParamID = SUB_PARAM_ART_SYS;
    artInfo.scale.max = 160;
    artInfo.scale.min = 70;
    artInfo.unit = UNIT_MMHG;
    nibpInfo.subParamID = SUB_PARAM_NIBP_SYS;
    nibpInfo.scale.max = 160;
    nibpInfo.scale.min = 70;
    nibpInfo.unit = UNIT_MMHG;



    unsigned startTime = trendDataPackList.first()->time;
    unsigned endTime = startTime + 4 * 60; //4 minutes ago

    int index = 0;
    while(index < trendDataPackList.size())
    {
        TrendDataPackage * p = trendDataPackList.at(index);

        if(p->time > endTime)
        {
            break;
        }

        TrendGraphData hrData;
        hrData.timestamp = p->time;
        hrData.data = p->subparamValue.value(SUB_PARAM_HR_PR, InvData());
        hrInfo.trendData.append(hrData);

        TrendGraphData spo2Data;
        spo2Data.timestamp = p->time;
        spo2Data.data = p->subparamValue.value(SUB_PARAM_SPO2, InvData());
        spo2Info.trendData.append(spo2Data);

        TrendGraphDataV3 artData;
        artData.timestamp = p->time;
        artData.data[0] = p->subparamValue.value(SUB_PARAM_ART_SYS, InvData());
        artData.data[1] = p->subparamValue.value(SUB_PARAM_ART_DIA, InvData());
        artData.data[2] = p->subparamValue.value(SUB_PARAM_ART_MAP, InvData());
        artInfo.trendDataV3.append(artData);

        index ++;
    };

    //add some demo nibp value
    TrendGraphDataV3 nibpData;
    nibpData.timestamp = startTime + 60;
    nibpData.data[0] = 120;
    nibpData.data[1] = 90;
    nibpData.data[2] = 100;
    nibpInfo.trendDataV3.append(nibpData);
    nibpData.timestamp = startTime + 120;
    nibpData.data[0] = 130;
    nibpData.data[1] = 100;
    nibpData.data[2] = 110;
    nibpInfo.trendDataV3.append(nibpData);
    nibpData.timestamp = startTime + 180;
    nibpData.data[0] = 150;
    nibpData.data[1] = 110;
    nibpData.data[2] = 130;
    nibpInfo.trendDataV3.append(nibpData);

    hrInfo.endTime = endTime;
    hrInfo.startTime = startTime;
    spo2Info.endTime = endTime;
    spo2Info.startTime = startTime;
    artInfo.endTime = endTime;
    artInfo.startTime = startTime;
    nibpInfo.endTime = endTime;
    nibpInfo.startTime = startTime;


    infos.append(hrInfo);
    infos.append(spo2Info);
    infos.append(artInfo);
    infos.append(nibpInfo);

    RecordPageGenerator *pageGenerator = new TrendGraphPageGenerator(infos);

    recorderManager.addPageGenerator(pageGenerator);
}

TrendGraphWidget::TrendGraphWidget() : _waveWidget(NULL), _moveCoordinateBtn(NULL),
    _moveCursorBtn(NULL), _moveEventBtn(NULL), _printBtn(NULL), _setBtn(NULL), _upBtn(NULL),
    _downBtn(NULL), _maxWidth(0), _maxHeight(0)
{
    setTitleBarText(trs("TrendGraph"));
    _maxWidth = windowManager.getPopMenuWidth();
    _maxHeight = windowManager.getPopMenuHeight();
    setFixedSize(_maxWidth, _maxHeight);

    int fontSize = fontManager.getFontSize(1);
    setFont(fontManager.textFont(fontSize));

    _waveWidget = new TrendWaveWidget();
    _waveWidget->setWidgetSize(_maxWidth - 10, _maxHeight - ITEM_HEIGHT * 3);

    _moveCoordinateBtn = new IMoveButton(trs("MoveCoordinate"));
    _moveCoordinateBtn->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _moveCoordinateBtn->setFont(fontManager.textFont(fontSize));
    connect(_moveCoordinateBtn, SIGNAL(leftMove()), _waveWidget, SLOT(leftMoveCoordinate()));
    connect(_moveCoordinateBtn, SIGNAL(rightMove()), _waveWidget, SLOT(rightMoveCoordinate()));

    _moveCursorBtn = new IMoveButton(trs("MoveCursor"));
    _moveCursorBtn->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _moveCursorBtn->setFont(fontManager.textFont(fontSize));
    connect(_moveCursorBtn, SIGNAL(leftMove()), _waveWidget, SLOT(leftMoveCursor()));
    connect(_moveCursorBtn, SIGNAL(rightMove()), _waveWidget, SLOT(rightMoveCursor()));

    _moveEventBtn = new IMoveButton(trs("MoveEvent"));
    _moveEventBtn->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _moveEventBtn->setFont(fontManager.textFont(fontSize));
    connect(_moveEventBtn, SIGNAL(leftMove()), _waveWidget, SLOT(leftMoveEvent()));
    connect(_moveEventBtn, SIGNAL(rightMove()), _waveWidget, SLOT(rightMoveEvent()));

    _printBtn = new IButton(trs("Print"));
    _printBtn->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _printBtn->setFont(fontManager.textFont(fontSize));
    connect(_printBtn, SIGNAL(realReleased()), this, SLOT(_printReleased()));

    _setBtn = new IButton(trs("Set"));
    _setBtn->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _setBtn->setFont(fontManager.textFont(fontSize));
    connect(_setBtn, SIGNAL(realReleased()), this, SLOT(_trendGraphSetReleased()));

    _upBtn = new IButton();
    _upBtn->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _upBtn->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_upBtn, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _downBtn = new IButton();
    _downBtn->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _downBtn->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_downBtn, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *lineLayout = new QHBoxLayout();
    lineLayout->setMargin(0);
    lineLayout->setSpacing(2);
    lineLayout->addWidget(_moveCoordinateBtn);
    lineLayout->addWidget(_moveCursorBtn);
    lineLayout->addWidget(_moveEventBtn);
    lineLayout->addWidget(_printBtn);
    lineLayout->addWidget(_setBtn);
    lineLayout->addWidget(_upBtn);
    lineLayout->addWidget(_downBtn);

    contentLayout->addWidget(_waveWidget);
    contentLayout->addStretch();
    contentLayout->addLayout(lineLayout);
    contentLayout->addStretch();
}
