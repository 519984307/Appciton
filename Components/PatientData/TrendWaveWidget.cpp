#include "TrendWaveWidget.h"
#include "TrendSubWaveWidget.h"
#include "TimeDate.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>

#define GRAPH_DISPLAY_DATA_NUMBER           4
#define GRAPH_POINT_NUMBER                  600

TrendWaveWidget::TrendWaveWidget() :
    _timeInterval(RESOLUTION_RATIO_30_SECOND),
    _initTime(0),
    _cursorPosition(GRAPH_POINT_NUMBER), _currentCursorTime(0),
    _displayGraphNum(2), _totalGraphNum(3)
{
    _initTime = timeDate.time();

    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::NoFocus);

    _subWidget = new IWidget();
    _subWidget->setFocusPolicy(Qt::NoFocus);
    _subWidgetScrollArea = new QScrollArea();

    // 重新调整子部件的大小
    _subWidgetScrollArea->setWidgetResizable(true);

    // 滚动区域透明处理
    _subWidgetScrollArea->setStyleSheet("QScrollArea {background-color:transparent;}");
    _subWidgetScrollArea->viewport()->setStyleSheet("background-color:transparent;");

    _subWidgetScrollArea->setFocusPolicy(Qt::NoFocus);

    _subWidgetScrollArea->verticalScrollBar()->setStyleSheet(
                                                             "QScrollBar:vertical"
                                                             "{"
                                                             "width:12px;"
                                                             "}"
                                                             );

    _mainLayout = new QVBoxLayout();
    _mainLayout->setMargin(0);

    _hLayoutTrend = new QVBoxLayout();
    _hLayoutTrend->setMargin(0);
    _hLayoutTrend->setSpacing(0);
    _subWidget->setLayout(_hLayoutTrend);

    _subWidgetScrollArea->setWidget(_subWidget);

    _mainLayout->addSpacing(30);
    _mainLayout->addWidget(_subWidgetScrollArea);

    setLayout(_mainLayout);

}

TrendWaveWidget::~TrendWaveWidget()
{

}

/**************************************************************************************************
 * 切换趋势数据的显示
 *************************************************************************************************/
void TrendWaveWidget::changeTrendDisplay()
{
    _clearLayout();
    _trendLayout();
}

/**************************************************************************************************
 * 设置窗口大小
 *************************************************************************************************/
void TrendWaveWidget::setWidgetSize(int w, int h)
{
    setFixedSize(w, h);
    _waveRegionWidth = width()/5 * 4;
    _oneFrameWidth = GRAPH_POINT_NUMBER/GRAPH_DISPLAY_DATA_NUMBER;
}

void TrendWaveWidget::leftMoveCursor()
{
    if (_cursorPosition <= 0)
    {
        _cursorPosition = 0;
    }
    else{
        _cursorPosition --;
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosition);
        }
    }

    update();
}

void TrendWaveWidget::rightMoveCursor()
{
    if (_cursorPosition >= GRAPH_POINT_NUMBER)
    {
        _cursorPosition = GRAPH_POINT_NUMBER;
    }
    else{
        _cursorPosition ++;
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosition);
        }
    }

    update();
}

void TrendWaveWidget::pageUpParam()
{
    int maxValue = _subWidgetScrollArea->verticalScrollBar()->maximum();
    _curVScroller = _subWidgetScrollArea->verticalScrollBar()->value();
    if (_curVScroller > 0)
    {
        // 当前位置　－　最大位置乘以总行数除以看不见的行数
        _subWidgetScrollArea->verticalScrollBar()->setSliderPosition(
                    _curVScroller - (maxValue * _displayGraphNum) / (_totalGraphNum - _displayGraphNum));
    }

}

void TrendWaveWidget::pageDownParam()
{
    int maxValue = _subWidgetScrollArea->verticalScrollBar()->maximum();
    _curVScroller = _subWidgetScrollArea->verticalScrollBar()->value();
    if (_curVScroller < maxValue && _totalGraphNum != _displayGraphNum)
    {
        _subWidgetScrollArea->verticalScrollBar()->setSliderPosition(
                    _curVScroller + (maxValue * _displayGraphNum) / (_totalGraphNum - _displayGraphNum));
    }
}

/**************************************************************************************************
 * 主窗口绘图事件
 *************************************************************************************************/
void TrendWaveWidget::paintEvent(QPaintEvent *event)
{
    IWidget::paintEvent(event);

    QPainter barPainter(this);

    barPainter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    QRect rectAdjust =rect().adjusted(0,30,0,0);
    barPainter.drawLine(rectAdjust.topLeft(), rectAdjust.topRight());

    unsigned t = _initTime;
    QString tStr;
    unsigned onePixelTime;
    // 坐标刻度
    for (int i = GRAPH_DISPLAY_DATA_NUMBER; i >= 0; i --)
    {
        timeDate.getTime(t, tStr, true);
        barPainter.drawText(rectAdjust.topLeft().x() + (_waveRegionWidth -GRAPH_POINT_NUMBER)/2 + i * _oneFrameWidth - 30,
                            rectAdjust.topLeft().y() - 5, tStr);
        switch (_timeInterval)
        {
        case RESOLUTION_RATIO_5_SECOND:
            onePixelTime = 5;
            break;
        case RESOLUTION_RATIO_30_SECOND:
            onePixelTime = 30;
            break;
        case RESOLUTION_RATIO_1_MINUTE:
            onePixelTime = 60;
            break;
        case RESOLUTION_RATIO_5_MINUTE:
            onePixelTime = 300;
            break;
        case RESOLUTION_RATIO_10_MINUTE:
            onePixelTime = 600;
            break;
        case RESOLUTION_RATIO_15_MINUTE:
            onePixelTime = 900;
            break;
        case RESOLUTION_RATIO_30_MINUTE:
            onePixelTime = 1800;
            break;
        case RESOLUTION_RATIO_1_HOUR:
            onePixelTime = 3600;
            break;
        case RESOLUTION_RATIO_2_HOUR:
            onePixelTime = 7200;
            break;
        case RESOLUTION_RATIO_3_HOUR:
            onePixelTime = 10800;
            break;
        default:
            break;
        }
        t = t - onePixelTime * GRAPH_POINT_NUMBER/GRAPH_DISPLAY_DATA_NUMBER;
        barPainter.drawLine(rectAdjust.topLeft().x() + (_waveRegionWidth -GRAPH_POINT_NUMBER)/2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y(), rectAdjust.topLeft().x() + (_waveRegionWidth -GRAPH_POINT_NUMBER)/2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y() + 5);
    }

    // 游标线
    barPainter.drawLine((_waveRegionWidth - GRAPH_POINT_NUMBER)/2 + _cursorPosition, rectAdjust.topLeft().y(),
                        (_waveRegionWidth - GRAPH_POINT_NUMBER)/2 + _cursorPosition, rectAdjust.bottomLeft().y());

    // 当前趋势记录的时间
    timeDate.getDate(_initTime - onePixelTime * (GRAPH_POINT_NUMBER - _cursorPosition), tStr);
    barPainter.drawText(_waveRegionWidth + 5, rectAdjust.topLeft().y() - 5, tStr);
    timeDate.getTime(_initTime - onePixelTime * (GRAPH_POINT_NUMBER - _cursorPosition), tStr, true);
    barPainter.drawText(_waveRegionWidth + 120, rectAdjust.topLeft().y() - 5, tStr);


    barPainter.setPen(QPen(Qt::white, 1, Qt::DotLine));
    barPainter.drawLine(_waveRegionWidth, 0, _waveRegionWidth, height());
}

/**************************************************************************************************
 * 添加显示控件
 *************************************************************************************************/
void TrendWaveWidget::_trendLayout()
{
    int subWidgetHeight = (height() - 30)/_displayGraphNum;

    TrendSubWaveWidget *subWidget1 = new TrendSubWaveWidget(SUB_PARAM_ART_SYS,
                                                            (_waveRegionWidth -GRAPH_POINT_NUMBER)/2, (_waveRegionWidth + GRAPH_POINT_NUMBER)/2,
                                                            subWidgetHeight/5, subWidgetHeight/5*4);
    subWidget1->setFixedHeight(subWidgetHeight);
    subWidget1->setVisible(true);
    subWidget1->setParent(this);
    subWidget1->setThemeColor(Qt::red);
    _hLayoutTrend->addWidget(subWidget1);

    TrendSubWaveWidget *subWidget2 = new TrendSubWaveWidget(SUB_PARAM_HR_PR,
                                                            (_waveRegionWidth -GRAPH_POINT_NUMBER)/2, (_waveRegionWidth + GRAPH_POINT_NUMBER)/2,
                                                            subWidgetHeight/5, subWidgetHeight/5*4);
    subWidget2->setFixedHeight(subWidgetHeight);
    subWidget2->setVisible(true);
    subWidget2->setParent(this);
    subWidget2->setThemeColor(Qt::blue);
    _hLayoutTrend->addWidget(subWidget2);

    TrendSubWaveWidget *subWidget3 = new TrendSubWaveWidget(SUB_PARAM_RR_BR,
                                                            (_waveRegionWidth -GRAPH_POINT_NUMBER)/2, (_waveRegionWidth + GRAPH_POINT_NUMBER)/2,
                                                            subWidgetHeight/5, subWidgetHeight/5*4);
    subWidget3->setFixedHeight(subWidgetHeight);
    subWidget3->setVisible(true);
    subWidget3->setParent(this);
    subWidget3->setThemeColor(Qt::yellow);
    _hLayoutTrend->addWidget(subWidget3);
}

/**************************************************************************************************
 * 清除显示控件
 *************************************************************************************************/
void TrendWaveWidget::_clearLayout()
{
    int trendcount = _hLayoutTrend->count();
    for (int i = 0; i < trendcount; i ++)
    {
        QLayoutItem *item = _hLayoutTrend->takeAt(0);
        IWidget *widget = (IWidget *)item->widget();
        if (widget != NULL)
        {
            widget->setVisible(false);
            widget->setParent(NULL);
        }
    }
}
