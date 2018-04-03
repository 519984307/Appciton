#include "TrendSubWaveWidget.h"
#include "IWidget.h"
#include <QPainter>
#include <QLabel>
#include "TimeDate.h"
#include "ParamInfo.h"
#include "ParamManager.h"
#include "AlarmConfig.h"

/***************************************************************************************************
 * 构造
 **************************************************************************************************/
TrendSubWaveWidget::TrendSubWaveWidget(SubParamID id, TrendGraphType type, int xHead, int xTail,
                                       int yTop, int yBottom, int downRuler, int upRuler) :
    _yTop(yTop), _yBottom(yBottom), _xHead(xHead), _xTail(xTail), _xSize(xHead - xTail),
    _ySize(yBottom - yTop), _trendDataHead(xHead + xTail), _size(600),
    _color(Qt::red), _upRulerValue(upRuler), _downRulerValue(downRuler),
    _rulerSize(upRuler - downRuler),_paramID(id), _cursorPosition(600), _type(type)
{
    SubParamID subID = _paramID;
    ParamID paramId = paramInfo.getParamID(subID);
    UnitType unitType = paramManager.getSubParamUnit(paramId, subID);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subID, unitType);
    if (config.scale == 1)
    {
        _downRulerValue = config.lowLimit;
        _upRulerValue = config.highLimit;
    }
    else
    {
        _downRulerValue = (double)config.lowLimit / config.scale;
        _upRulerValue = (double)config.highLimit / config.scale;
    }

    _rulerSize = _upRulerValue - _downRulerValue;

    _paramName = paramInfo.getSubParamName(id);
    _paramUnit = Unit::getSymbol(paramInfo.getUnitOfSubParam(id));

    setFocusPolicy(Qt::NoFocus);
    _trendWaveBuf = new QPoint[_size];
    _dataBuf = new int[_size];

    loadParamData();
}

/***************************************************************************************************
 * 析构
 **************************************************************************************************/
TrendSubWaveWidget::~TrendSubWaveWidget()
{

}

void TrendSubWaveWidget:: demoData()
{
    for (int i =  0; i < _size; i ++)
    {
        _dataBuf[i] = qrand() % 10 + 50;
    }
}

/***************************************************************************************************
 * y坐标换算成趋势值
 **************************************************************************************************/
int TrendSubWaveWidget::yToValue(int y)
{
    return ((_yBottom - y) * _rulerSize / _ySize) + _downRulerValue;
}

/***************************************************************************************************
 * 趋势值换算成y坐标
 **************************************************************************************************/
int TrendSubWaveWidget::valueToY(int value)
{
    return (_yBottom - (value - _downRulerValue) * _ySize / _rulerSize);
}

/***************************************************************************************************
 * x坐标换算成索引值
 **************************************************************************************************/
int TrendSubWaveWidget::xToIndex(int x)
{
    return (x - _xHead);
}

/***************************************************************************************************
 * 索引值换算成x坐标
 **************************************************************************************************/
int TrendSubWaveWidget::indexToX(int index)
{
    return (index + _xHead);
}

/***************************************************************************************************
 * 趋势图坐标系头尾位置
 **************************************************************************************************/
void TrendSubWaveWidget::beginTrendData(int xHead, int xTail, int trendHead)
{
    _xHead = xHead;
    _xTail = xTail;
    _xSize = _xTail - _xHead;
    _trendDataHead = trendHead;
    update();
}

/***************************************************************************************************
 * 设置窗口主题颜色
 **************************************************************************************************/
void TrendSubWaveWidget::setThemeColor(QColor color)
{
    if (color != QColor(0, 0, 0))
    {
        _color = color;
    }
}

/***************************************************************************************************
 * 设置窗口标尺范围
 **************************************************************************************************/
void TrendSubWaveWidget::setRulerRange(int down, int up)
{
    _upRulerValue = up;
    _downRulerValue = down;
    _rulerSize = up - down;
    loadParamData();
    update();
}

/***************************************************************************************************
 * 游标移动
 **************************************************************************************************/
void TrendSubWaveWidget::cursorMove(int position)
{
    _cursorPosition = position;
    update();
}

/***************************************************************************************************
 * 载入参数数据
 **************************************************************************************************/
void TrendSubWaveWidget::loadParamData()
{
    demoData();
    int yValue;
    for (int i =  0; i < _size; i ++)
    {
        yValue = valueToY(_dataBuf[i]);
        if (yValue < _yTop || yValue > _yBottom)
        {
            continue;
        }
        else
        {
            _trendWaveBuf[i].setY(yValue);
        }
        _trendWaveBuf[i].setX(indexToX(i));
    }
}

/***************************************************************************************************
 * 获取param id
 **************************************************************************************************/
SubParamID TrendSubWaveWidget::getSubParamID()
{
    return _paramID;
}

/***************************************************************************************************
 * 绘图事件
 **************************************************************************************************/
void TrendSubWaveWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    QPainter barPainter(this);
    barPainter.setPen(QPen(_color, 1, Qt::SolidLine));

    barPainter.drawLine(_xHead/3, _yTop, _xHead/3 + 5, _yTop);
    barPainter.drawLine(_xHead/3, _yTop, _xHead/3, _yBottom);
    barPainter.drawLine(_xHead/3, _yBottom, _xHead/3 + 5, _yBottom);

    // 趋势标尺上限
    QRect upRulerRect(_xHead/3 + 7, _yTop - 10, _xHead, 30);
    barPainter.drawText(upRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(_upRulerValue));

    // 趋势标尺下限
    QRect downRulerRect(_xHead/3 + 7, _yBottom - 10, _xHead, 30);
    barPainter.drawText(downRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(_downRulerValue));

    if (_trendWaveBuf)
    {
        barPainter.drawPolyline(_trendWaveBuf, _size);
    }

    QFont font;
    font.setPixelSize(15);
    barPainter.setFont(font);
    // 趋势参数名称
    QRect nameRect(_trendDataHead + 5, 5, width() - _trendDataHead -10, 30);
    barPainter.drawText(nameRect, Qt::AlignLeft | Qt::AlignTop, _paramName);

    // 趋势参数单位
    QRect unitRect(_trendDataHead + 5, 5, width() - _trendDataHead - 10, 30);
    barPainter.drawText(unitRect, Qt::AlignRight | Qt::AlignTop, _paramUnit);
    font.setPixelSize(60);
    barPainter.setFont(font);

    // 趋势参数数据
    if (_type == TREND_GRAPH_TYPE_NORMAL)
    {
        barPainter.drawText(_trendDataHead + 60, height()/3*2, QString::number(_dataBuf[(600 - _cursorPosition)]));
    }
    else if (_type == TREND_GRAPH_TYPE_NIBP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);
        QString trendStr = QString::number(_dataBuf[(600 - _cursorPosition)]) + "/" + "78";
        barPainter.drawText(_trendDataHead + 60, height()/2, trendStr);
        barPainter.drawText(_trendDataHead + 70, height()/3*2, "(80)");
    }
    else if (_type == TREND_GRAPH_TYPE_ART_IBP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);
        QString trendStr = QString::number(_dataBuf[(600 - _cursorPosition)]) + "/" + "78";
        barPainter.drawText(_trendDataHead + 60, height()/2, trendStr);
        barPainter.drawText(_trendDataHead + 70, height()/3*2, "(80)");
    }
    else if (_type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);
        QString trendStr = QString::number(_dataBuf[(600 - _cursorPosition)]) + "/" + "78";
        barPainter.drawText(_trendDataHead + 60, height()/3*2, trendStr);
    }
}
