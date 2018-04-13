#include "TrendSubWaveWidget.h"
#include "IWidget.h"
#include <QPainter>
#include <QLabel>
#include "TimeDate.h"
#include "ParamInfo.h"
#include "ParamManager.h"
#include "AlarmConfig.h"

#define GRAPH_POINT_NUMBER          120
#define DATA_INTERVAL_PIXEL         5

/***************************************************************************************************
 * 构造
 **************************************************************************************************/
TrendSubWaveWidget::TrendSubWaveWidget(SubParamID id, TrendGraphType type, int xHead, int xTail,
                                       int yTop, int yBottom, int downRuler, int upRuler) :
    _yTop(yTop), _yBottom(yBottom), _xHead(xHead), _xTail(xTail), _xSize(xHead - xTail),
    _ySize(yBottom - yTop), _trendDataHead(xHead + xTail), _size(GRAPH_POINT_NUMBER + 1),
    _color(Qt::red), _upRulerValue(upRuler), _downRulerValue(downRuler),
    _rulerSize(upRuler - downRuler), _dataBuf(NULL), _dataBufSecond(NULL), _dataBufThird(NULL),
    _paramID(id), _cursorPosition(GRAPH_POINT_NUMBER), _type(type)
{
    SubParamID subID = _paramID;
    ParamID paramId = paramInfo.getParamID(subID);
    UnitType unitType = paramManager.getSubParamUnit(paramId, subID);

    if (_type == TREND_GRAPH_TYPE_NORMAL || _type == TREND_GRAPH_TYPE_AG_TEMP)
    {
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
        _paramName = paramInfo.getSubParamName(id);
    }
    else if (_type == TREND_GRAPH_TYPE_ART_IBP || _type == TREND_GRAPH_TYPE_NIBP)
    {
        LimitAlarmConfig configUp = alarmConfig.getLimitAlarmConfig(SubParamID(subID - 2), unitType);
        LimitAlarmConfig configDown = alarmConfig.getLimitAlarmConfig(SubParamID(subID - 1), unitType);
        if (configUp.scale == 1)
        {
            _downRulerValue = configDown.lowLimit;
            _upRulerValue = configUp.highLimit;
        }
        else
        {
            _downRulerValue = (double)configDown.lowLimit / configDown.scale;
            _upRulerValue = (double)configUp.highLimit / configUp.scale;
        }
        QString str = paramInfo.getSubParamName(id);
        _paramName = str.left(str.length() - 4);
    }

    _rulerSize = _upRulerValue - _downRulerValue;

    _paramUnit = Unit::getSymbol(paramInfo.getUnitOfSubParam(id));

    setFocusPolicy(Qt::NoFocus);

    _trendWaveBuf = new QPoint[_size];
    _dataBuf = new int[_size];
    for (int i = 0; i < _size; i ++)
    {
        _dataBuf[i] = InvData();
    }

    if (_type != TREND_GRAPH_TYPE_NORMAL)
    {
        _trendWaveBufSecond = new QPoint[_size];
        _dataBufSecond = new int[_size];
        for (int i = 0; i < _size; i ++)
        {
            _dataBufSecond[i] = InvData();
        }
    }

    if (_type == TREND_GRAPH_TYPE_NIBP || _type == TREND_GRAPH_TYPE_ART_IBP)
    {
        _trendWaveBufThird = new QPoint[_size];
        _dataBufThird = new int[_size];
        for (int i = 0; i < _size; i ++)
        {
            _dataBufThird[i] = InvData();
        }
    }

    loadParamData();
}

/***************************************************************************************************
 * 析构
 **************************************************************************************************/
TrendSubWaveWidget::~TrendSubWaveWidget()
{

}

/***************************************************************************************************
 * demo数据
 **************************************************************************************************/
void TrendSubWaveWidget:: demoData()
{
    for (int i =  0; i < _size; i ++)
    {
        _dataBuf[i] = qrand() % 10 + 50;
    }
}

/***************************************************************************************************
 * 存储一组趋势数据
 **************************************************************************************************/
void TrendSubWaveWidget::trendData(int *buf, int size)
{
    for (int i =  0; i < size; i ++)
    {
        _dataBuf[_size - i - 1] = buf[i];
    }
}

/***************************************************************************************************
 * 存储两组趋势数据
 **************************************************************************************************/
void TrendSubWaveWidget::trendData(int *oneBuf, int *secondBuf, int size)
{
    for (int i =  0; i < size; i ++)
    {
        _dataBuf[_size - i - 1] = oneBuf[i];
        _dataBufSecond[_size - i - 1] = secondBuf[i];
    }
}

/***************************************************************************************************
 * 存储三组趋势数据
 **************************************************************************************************/
void TrendSubWaveWidget::trendData(int *oneBuf, int *secondBuf, int *thirdBuf, int size)
{
    for (int i =  0; i < size; i ++)
    {
        _dataBuf[_size - i - 1] = oneBuf[i];
        _dataBufSecond[_size - i - 1] = secondBuf[i];
        _dataBufThird[_size - i - 1] = thirdBuf[i];
    }
}

/***************************************************************************************************
 * 更新趋势图
 **************************************************************************************************/
void TrendSubWaveWidget::updateTrendGraph()
{
    _cursorPosition = GRAPH_POINT_NUMBER;
    loadParamData();
    update();
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
    return (index * 5 + _xHead);
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
//    demoData();
    int yValue = 0;
    for (int i =  0; i < _size; i ++)
    {
        if (_dataBuf[i] != InvData())
        {
            if (_type == TREND_GRAPH_TYPE_AG_TEMP)
            {
                yValue = valueToY(_dataBuf[i]/10);
            }
            else
            {
                yValue = valueToY(_dataBuf[i]);
            }
        }
        else
        {
            yValue = InvData();
        }
        if (yValue < _yTop && yValue != InvData())
        {
            _trendWaveBuf[i].setY(_yTop);
        }
        else if (yValue > _yBottom && yValue != InvData())
        {
            _trendWaveBuf[i].setY(_yBottom);
        }
        else
        {
            _trendWaveBuf[i].setY(yValue);
        }
        _trendWaveBuf[i].setX(indexToX(i));

        if (_type != TREND_GRAPH_TYPE_NORMAL)
        {
            if (_dataBufSecond[i] != InvData())
            {
                if (_type == TREND_GRAPH_TYPE_AG_TEMP)
                {
                    yValue = valueToY(_dataBufSecond[i]/10);
                }
                else
                {
                    yValue = valueToY(_dataBufSecond[i]);
                }
            }
            else
            {
                yValue = InvData();
            }
            if (yValue < _yTop && yValue != InvData())
            {
                _trendWaveBufSecond[i].setY(_yTop);
            }
            else if (yValue > _yBottom && yValue != InvData())
            {
                _trendWaveBufSecond[i].setY(_yBottom);
            }
            else
            {
                _trendWaveBufSecond[i].setY(yValue);
            }
            _trendWaveBufSecond[i].setX(indexToX(i));
        }

        if (_type == TREND_GRAPH_TYPE_NIBP || _type == TREND_GRAPH_TYPE_ART_IBP)
        {
            if (_dataBufThird[i] != InvData())
            {
                yValue = valueToY(_dataBufThird[i]);
            }
            else
            {
                yValue = InvData();
            }
            if (yValue < _yTop && yValue != InvData())
            {
                _trendWaveBufThird[i].setY(_yTop);
            }
            else if (yValue > _yBottom && yValue != InvData())
            {
                _trendWaveBufThird[i].setY(_yBottom);
            }
            else
            {
                _trendWaveBufThird[i].setY(yValue);
            }
            _trendWaveBufThird[i].setX(indexToX(i));
        }
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

    // 趋势波形图
    barPainter.setPen(QPen(_color, 2, Qt::SolidLine));
    if (_type == TREND_GRAPH_TYPE_NORMAL)
    {
        for (int i = 0; i < _size - 1; i ++)
        {
            if (_trendWaveBuf[i].y() != InvData() && _trendWaveBuf[i + 1].y() != InvData())
            {
                barPainter.drawLine(_trendWaveBuf[i].x(), _trendWaveBuf[i].y(),
                                    _trendWaveBuf[i + 1].x(), _trendWaveBuf[i + 1].y());
            }
            else if (_trendWaveBuf[i + 1].y() != InvData())
            {
                barPainter.drawPoint(_trendWaveBuf[i + 1].x(), _trendWaveBuf[i + 1].y());
            }
        }
    }
    else if (_type == TREND_GRAPH_TYPE_NIBP)
    {
        for (int i = 0; i < _size; i ++)
        {
            if (_trendWaveBuf[i].y() != InvData() && _trendWaveBufSecond[i].y() != InvData() &&
                    _trendWaveBufThird[i].y() != InvData())
            {
                barPainter.drawLine(_trendWaveBuf[i].x() - 3, _trendWaveBuf[i].y() - 3,
                                    _trendWaveBuf[i].x(), _trendWaveBuf[i].y());
                barPainter.drawLine(_trendWaveBuf[i].x(), _trendWaveBuf[i].y(),
                                    _trendWaveBuf[i].x() + 3, _trendWaveBuf[i].y() - 3);
                barPainter.drawLine(_trendWaveBufSecond[i].x() - 3, _trendWaveBufSecond[i].y() + 3,
                                    _trendWaveBufSecond[i].x(), _trendWaveBufSecond[i].y());
                barPainter.drawLine(_trendWaveBufSecond[i].x(), _trendWaveBufSecond[i].y(),
                                    _trendWaveBufSecond[i].x() + 3, _trendWaveBufSecond[i].y() + 3);
                barPainter.drawLine(_trendWaveBuf[i].x(), _trendWaveBuf[i].y(),
                                    _trendWaveBufSecond[i].x(), _trendWaveBufSecond[i].y());
                barPainter.drawLine(_trendWaveBufThird[i].x() - 3, _trendWaveBufThird[i].y(),
                                    _trendWaveBufThird[i].x() + 3, _trendWaveBufThird[i].y());
            }
        }
    }
    else if (_type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        for (int i = 0; i < _size - 1; i ++)
        {
            if (_trendWaveBuf[i].y() != InvData() && _trendWaveBuf[i + 1].y() != InvData())
            {
                barPainter.drawLine(_trendWaveBuf[i].x(), _trendWaveBuf[i].y(),
                                    _trendWaveBuf[i + 1].x(), _trendWaveBuf[i + 1].y());
            }
            if (_trendWaveBufSecond[i].y() != InvData() && _trendWaveBufSecond[i + 1].y() != InvData())
            {
                barPainter.drawLine(_trendWaveBufSecond[i].x(), _trendWaveBufSecond[i].y(),
                                    _trendWaveBufSecond[i + 1].x(), _trendWaveBufSecond[i + 1].y());
            }
        }
    }
    barPainter.setPen(QPen(_color, 1, Qt::SolidLine));

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
        if (_dataBuf[_cursorPosition] != InvData())
        {
            barPainter.drawText(_trendDataHead + 60, height()/3*2, QString::number(_dataBuf[_cursorPosition]));
        }
        else
        {
            barPainter.drawText(_trendDataHead + 60, height()/3*2, "---");
        }
    }
    else if (_type == TREND_GRAPH_TYPE_NIBP || _type == TREND_GRAPH_TYPE_ART_IBP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);

        if (_dataBuf[_cursorPosition] != InvData() && _dataBufSecond[_cursorPosition] != InvData() && _dataBufThird[_cursorPosition] != InvData())
        {
            QString trendStr = QString::number(_dataBuf[_cursorPosition]) + "/" + QString::number(_dataBufSecond[_cursorPosition]);
            barPainter.drawText(_trendDataHead + 60, height()/2, trendStr);
            barPainter.drawText(_trendDataHead + 70, height()/3*2, QString::number(_dataBufThird[_cursorPosition]));
        }
        else
        {
            barPainter.drawText(_trendDataHead + 60, height()/2, "---/---");
            barPainter.drawText(_trendDataHead + 70, height()/3*2, "(---)");
        }
    }
    else if (_type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);
        if (_dataBuf[_cursorPosition] != InvData() && _dataBufSecond[_cursorPosition] != InvData())
        {
            double value1 = (double)_dataBuf[_cursorPosition] / 10;
            double value2 = (double)_dataBufSecond[_cursorPosition] / 10;
            QString trendStr = QString::number(value1, 'g', 2) + "/" + QString::number(value2, 'g', 2);
            barPainter.drawText(_trendDataHead + 60, height()/2, trendStr);
        }
        else
        {
            barPainter.drawText(_trendDataHead + 60, height()/2, "---/---");
        }
    }

    barPainter.setPen(QPen(Qt::gray, 1, Qt::DotLine));
    barPainter.drawLine(rect().bottomLeft(), rect().bottomRight());
}
