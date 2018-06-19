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
    _waveWidget->setTrendWaveReset();
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
    QList<TrendGraphInfo>  trendGraphList = _waveWidget->getTrendGraphPrint();
    RecordPageGenerator *pageGenerator = new TrendGraphPageGenerator(trendGraphList);

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
