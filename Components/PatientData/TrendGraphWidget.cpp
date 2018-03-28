#include "TrendGraphWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "IButton.h"
#include "LanguageManager.h"
#include "TrendWaveWidget.h"
#include "TrendGraphSetWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#define ITEM_HEIGHT             30
#define ITEM_WIDTH              100

TrendGraphWidget *TrendGraphWidget::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TrendGraphWidget::~TrendGraphWidget()
{
    delete _waveWidget;
}

void TrendGraphWidget::_leftMoveCoordinate()
{

}

void TrendGraphWidget::_rightMoveCoordinate()
{

}

void TrendGraphWidget::_leftMoveCursor()
{
    _waveWidget->leftMoveCursor();
}

void TrendGraphWidget::_rightMoveCursor()
{
    _waveWidget->rightMoveCursor();
}

void TrendGraphWidget::_leftMoveEvent()
{

}

void TrendGraphWidget::_rightMoveEvent()
{

}

/**************************************************************************************************
 * 趋势图设置槽函数
 *************************************************************************************************/
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

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TrendGraphWidget::TrendGraphWidget()
{
    _maxWidth = windowManager.getPopMenuWidth();
    _maxHeight = windowManager.getPopMenuHeight();
    setFixedSize(_maxWidth, _maxHeight);

    int fontSize = fontManager.getFontSize(1);
    setFont(fontManager.textFont(fontSize));

    _waveWidget = new TrendWaveWidget();
    _waveWidget->setWidgetSize(_maxWidth - 10, _maxHeight - ITEM_HEIGHT * 3);
    _waveWidget->changeTrendDisplay();

    _moveCoordinate = new IMoveButton(trs("MoveCoordinate"));
    _moveCoordinate->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _moveCoordinate->setFont(fontManager.textFont(fontSize));

    _moveCursor = new IMoveButton(trs("MoveCursor"));
    _moveCursor->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _moveCursor->setFont(fontManager.textFont(fontSize));
    connect(_moveCursor, SIGNAL(leftMove()), this, SLOT(_leftMoveCursor()));
    connect(_moveCursor, SIGNAL(rightMove()), this, SLOT(_rightMoveCursor()));

    _moveEvent = new IMoveButton(trs("MoveEvent"));
    _moveEvent->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _moveEvent->setFont(fontManager.textFont(fontSize));

    _print = new IButton(trs("Print"));
    _print->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _print->setFont(fontManager.textFont(fontSize));

    _set = new IButton(trs("Set"));
    _set->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _set->setFont(fontManager.textFont(fontSize));
    connect(_set, SIGNAL(realReleased()), this, SLOT(_trendGraphSetReleased()));

    _up = new IButton();
    _up->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *lineLayout = new QHBoxLayout();
    lineLayout->setMargin(0);
    lineLayout->setSpacing(2);
    lineLayout->addWidget(_moveCoordinate);
    lineLayout->addWidget(_moveCursor);
    lineLayout->addWidget(_moveEvent);
    lineLayout->addWidget(_print);
    lineLayout->addWidget(_set);
    lineLayout->addWidget(_up);
    lineLayout->addWidget(_down);

    contentLayout->addWidget(_waveWidget);
    contentLayout->addStretch();
    contentLayout->addLayout(lineLayout);
    contentLayout->addStretch();
}
