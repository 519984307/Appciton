#include "TrendSubWaveWidget.h"
#include "IWidget.h"
#include <QPainter>
#include <QLabel>
#include "TimeDate.h"
#include "ParamInfo.h"

/***************************************************************************************************
 * 构造
 **************************************************************************************************/
TrendSubWaveWidget::TrendSubWaveWidget(SubParamID id, int xHead, int xTail,
                                       int yTop, int yBottom, int downRuler, int upRuler) :
    _yTop(yTop), _yBottom(yBottom), _xHead(xHead), _xTail(xTail), _xSize(xHead - xTail),
    _ySize(yBottom - yTop), _trendDataHead(xHead + xTail), _size(600),
    _color(Qt::green), _upRulerValue(upRuler), _downRulerValue(downRuler),
    _rulerSize(upRuler - downRuler), _cursorPosition(600)
{
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
    return ((_yBottom - y) * _rulerSize / _ySize);
}

/***************************************************************************************************
 * 趋势值换算成y坐标
 **************************************************************************************************/
int TrendSubWaveWidget::valueToY(int value)
{
    return (_yBottom - value * _ySize / _rulerSize);
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
    _color = color;
}

/***************************************************************************************************
 * 设置窗口标尺范围
 **************************************************************************************************/
void TrendSubWaveWidget::setRulerRange(int down, int up)
{
    _upRulerValue = up;
    _downRulerValue = down;
    _rulerSize = up - down;
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

    for (int i =  0; i < _size; i ++)
    {
        _trendWaveBuf[i].setX(indexToX(i));
        _trendWaveBuf[i].setY(valueToY(_dataBuf[i]));
    }
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
    barPainter.drawText(_trendDataHead + 60, height()/2, QString::number(_dataBuf[(600 - _cursorPosition)]));

}
