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
#define TREND_DISPLAY_OFFSET        60

/***************************************************************************************************
 * 构造
 **************************************************************************************************/
TrendSubWaveWidget::TrendSubWaveWidget(SubParamID id, TrendGraphType type) : _id(id), _type(type)
{
    SubParamID subID = id;
    ParamID paramId = paramInfo.getParamID(subID);
    UnitType unitType = paramManager.getSubParamUnit(paramId, subID);

    if (_type == TREND_GRAPH_TYPE_NORMAL || _type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subID, unitType);
        if (config.scale == 1)
        {
            _valueY.min = config.lowLimit;
            _valueY.max = config.highLimit;
        }
        else
        {
            _valueY.min = (double)config.lowLimit / config.scale;
            _valueY.max = (double)config.highLimit / config.scale;
        }
        _paramName = paramInfo.getSubParamName(id);
    }
    else if (_type == TREND_GRAPH_TYPE_ART_IBP || _type == TREND_GRAPH_TYPE_NIBP)
    {
        LimitAlarmConfig configUp = alarmConfig.getLimitAlarmConfig(SubParamID(subID - 2), unitType);
        LimitAlarmConfig configDown = alarmConfig.getLimitAlarmConfig(SubParamID(subID - 1), unitType);
        if (configUp.scale == 1)
        {
            _valueY.min = configDown.lowLimit;
            _valueY.max = configUp.highLimit;
        }
        else
        {
            _valueY.min = (double)configDown.lowLimit / configDown.scale;
            _valueY.max = (double)configUp.highLimit / configUp.scale;
        }
        QString str = paramInfo.getSubParamName(id);
        _paramName = str.left(str.length() - 4);
    }
    _paramUnit = Unit::getSymbol(paramInfo.getUnitOfSubParam(id));

}

/***************************************************************************************************
 * 析构
 **************************************************************************************************/
TrendSubWaveWidget::~TrendSubWaveWidget()
{
}

void TrendSubWaveWidget::trendDataInfo(TrendGraphInfo info)
{
    _trendInfo = info;
    _cursorPosIndex = _trendInfo.trendData.count() - 1;
}

/***************************************************************************************************
 * 更新趋势图
 **************************************************************************************************/
void TrendSubWaveWidget::updateTrendGraph()
{
//    _cursorPosition = GRAPH_POINT_NUMBER;
//    loadParamData();
    update();
}

void TrendSubWaveWidget::loadTrendSubWidgetInfo(TrendSubWidgetInfo info)
{
    _info = info;
    _valueY.start = info.yTop;
    _valueY.end = info.yBottom;
    _timeX.start = info.xHead;
    _timeX.end = info.xTail;


    _xSize = info.xHead - info.xTail;
    _ySize = info.yBottom - info.yTop;
    _trendDataHead = info.xHead + info.xTail;
//    ParamID paramId = paramInfo.getParamID(_id);
//    UnitType unitType = paramManager.getSubParamUnit(paramId, _id);

//    if (_type == TREND_GRAPH_TYPE_NORMAL || _type == TREND_GRAPH_TYPE_AG_TEMP)
//    {
//        LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(_id, unitType);
//        if (config.scale == 1)
//        {
//            _downRulerValue = config.lowLimit;
//            _upRulerValue = config.highLimit;
//        }
//        else
//        {
//            _downRulerValue = (double)config.lowLimit / config.scale;
//            _upRulerValue = (double)config.highLimit / config.scale;
//        }
//        _paramName = paramInfo.getSubParamName(_id);
//    }
//    else if (_type == TREND_GRAPH_TYPE_ART_IBP || _type == TREND_GRAPH_TYPE_NIBP)
//    {
//        LimitAlarmConfig configUp = alarmConfig.getLimitAlarmConfig(SubParamID(_id - 2), unitType);
//        LimitAlarmConfig configDown = alarmConfig.getLimitAlarmConfig(SubParamID(_id - 1), unitType);
//        if (configUp.scale == 1)
//        {
//            _downRulerValue = configDown.lowLimit;
//            _upRulerValue = configUp.highLimit;
//        }
//        else
//        {
//            _downRulerValue = (double)configDown.lowLimit / configDown.scale;
//            _upRulerValue = (double)configUp.highLimit / configUp.scale;
//        }
//        QString str = paramInfo.getSubParamName(_id);
//        _paramName = str.left(str.length() - 4);
//    }

//    _rulerSize = _upRulerValue - _downRulerValue;

//    _paramUnit = Unit::getSymbol(paramInfo.getUnitOfSubParam(_id));

//    setFocusPolicy(Qt::NoFocus);

//    _trendWaveBuf = new QPoint[_size];
//    _dataBuf = new int[_size];
//    for (int i = 0; i < _size; i ++)
//    {
//        _dataBuf[i] = InvData();
//    }

//    if (_type != TREND_GRAPH_TYPE_NORMAL)
//    {
//        _trendWaveBufSecond = new QPoint[_size];
//        _dataBufSecond = new int[_size];
//        for (int i = 0; i < _size; i ++)
//        {
//            _dataBufSecond[i] = InvData();
//        }
//    }

//    if (_type == TREND_GRAPH_TYPE_NIBP || _type == TREND_GRAPH_TYPE_ART_IBP)
//    {
//        _trendWaveBufThird = new QPoint[_size];
//        _dataBufThird = new int[_size];
//        for (int i = 0; i < _size; i ++)
//        {
//            _dataBufThird[i] = InvData();
//        }
//    }

    //    loadParamData();
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
//    _upRulerValue = up;
//    _downRulerValue = down;
//    _rulerSize = up - down;
//    loadParamData();
//    update();
    _valueY.max = up;
    _valueY.min = down;
}

void TrendSubWaveWidget::setTimeRange(unsigned leftTime, unsigned rightTime)
{
    _timeX.max = leftTime;
    _timeX.min = rightTime;
}

/***************************************************************************************************
 * 游标移动
 **************************************************************************************************/
void TrendSubWaveWidget::cursorMove(int position)
{
    _cursorPosIndex = position;
    update();
}

/***************************************************************************************************
 * 获取param id
 **************************************************************************************************/
SubParamID TrendSubWaveWidget::getSubParamID()
{
    return _id;
}

/***************************************************************************************************
 * 绘图事件
 **************************************************************************************************/
void TrendSubWaveWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    QPainter barPainter(this);
    barPainter.setPen(QPen(_color, 1, Qt::SolidLine));

    barPainter.drawLine(_info.xHead/3, _info.yTop, _info.xHead/3 + 5, _info.yTop);
    barPainter.drawLine(_info.xHead/3, _info.yTop, _info.xHead/3, _info.yBottom);
    barPainter.drawLine(_info.xHead/3, _info.yBottom, _info.xHead/3 + 5, _info.yBottom);

    // 趋势标尺上限
    QRect upRulerRect(_info.xHead/3 + 7, _info.yTop - 10, _info.xHead, 30);
    barPainter.drawText(upRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(_valueY.max));

    // 趋势标尺下限
    QRect downRulerRect(_info.xHead/3 + 7, _info.yBottom - 10, _info.xHead, 30);
    barPainter.drawText(downRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(_valueY.min));

    // 趋势波形图
    barPainter.setPen(QPen(_color, 2, Qt::SolidLine));
    if (!_trendInfo.trendData.count())
    {
        return;
    }
    if (_type == TREND_GRAPH_TYPE_NORMAL)
    {
        for (int i = 0; i < _trendInfo.trendData.count() - 1; i ++)
        {
            double y1 = _mapValue(_valueY, _trendInfo.trendData.at(i).data);
            double y2 = _mapValue(_valueY, _trendInfo.trendData.at(i + 1).data);
            double x1 = _mapValue(_timeX, _trendInfo.trendData.at(i).timestamp);
            double x2 = _mapValue(_timeX, _trendInfo.trendData.at(i + 1).timestamp);
            if (y1 != InvData() && y2 != InvData())
            {
                barPainter.drawLine(x1, y1, x2, y2);
            }
            else if (y1 != InvData())
            {
                barPainter.drawPoint(x1, y1);
            }
            else if (y2 != InvData())
            {
                barPainter.drawPoint(x2, y2);
            }
        }
    }
    else if (_type == TREND_GRAPH_TYPE_NIBP)
    {
        for (int i = 0; i < _trendInfo.trendDataV3.count(); i ++)
        {
            double x1 = _mapValue(_timeX, _trendInfo.trendDataV3.at(i).timestamp);
            double map =  _mapValue(_valueY, _trendInfo.trendDataV3.at(i).data[0]);
            double dia = _mapValue(_valueY, _trendInfo.trendDataV3.at(i).data[1]);
            double sys = _mapValue(_valueY, _trendInfo.trendDataV3.at(i).data[2]);
            if (map != InvData() && dia != InvData() && sys != InvData())
            {
                barPainter.drawLine(x1 - 3, sys - 3, x1, sys);
                barPainter.drawLine(x1, sys, x1 + 3, sys - 3);
                barPainter.drawLine(x1 - 3, dia + 3, x1, dia);
                barPainter.drawLine(x1, dia, x1 + 3, dia + 3);
                barPainter.drawLine(x1, sys, x1, dia);
                barPainter.drawLine(x1 - 3, map, x1 + 3, map);
            }
        }
    }
    else if (_type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        for (int i = 0; i < _trendInfo.trendDataV2.count() - 1; i ++)
        {
            double fristY1 = _mapValue(_valueY, _trendInfo.trendDataV2.at(i).data[0]);
            double fristY2 = _mapValue(_valueY, _trendInfo.trendDataV2.at(i + 1).data[0]);
            double secondY1 = _mapValue(_valueY, _trendInfo.trendDataV2.at(i).data[1]);
            double secondY2 = _mapValue(_valueY, _trendInfo.trendDataV2.at(i + 1).data[1]);
            double x1 = _mapValue(_timeX, _trendInfo.trendDataV2.at(i).timestamp);
            double x2 = _mapValue(_timeX, _trendInfo.trendDataV2.at(i + 1).timestamp);
            if (fristY1 != InvData() && fristY2 != InvData())
            {
                barPainter.drawLine(x1, fristY1, x2, fristY2);
            }
            else if (fristY1 != InvData())
            {
                barPainter.drawPoint(x1, fristY1);
            }
            else if (fristY2 != InvData())
            {
                barPainter.drawPoint(x2, fristY2);
            }

            if (secondY1 != InvData() && secondY2 != InvData())
            {
                barPainter.drawLine(x1, secondY1, x2, secondY2);
            }
            else if (secondY1 != InvData())
            {
                barPainter.drawPoint(x1, secondY1);
            }
            else if (secondY2 != InvData())
            {
                barPainter.drawPoint(x2, secondY2);
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
        TrendDataType value = _trendInfo.trendData.at(_cursorPosIndex).data;
        if (value != InvData())
        {
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/3*2, QString::number(value));
        }
        else
        {
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/3*2, "---");
        }
    }
//    else if (_type == TREND_GRAPH_TYPE_NIBP || _type == TREND_GRAPH_TYPE_ART_IBP)
//    {
//        font.setPixelSize(30);
//        barPainter.setFont(font);

//        if (_dataBuf[_cursorPosition] != InvData() && _dataBufSecond[_cursorPosition] != InvData() && _dataBufThird[_cursorPosition] != InvData())
//        {
//            QString trendStr = QString::number(_dataBuf[_cursorPosition]) + "/" + QString::number(_dataBufSecond[_cursorPosition]);
//            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/2, trendStr);
//            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET + 10, height()/3*2, QString::number(_dataBufThird[_cursorPosition]));
//        }
//        else
//        {
//            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/2, "---/---");
//            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET + 10, height()/3*2, "(---)");
//        }
//    }
//    else if (_type == TREND_GRAPH_TYPE_AG_TEMP)
//    {
//        font.setPixelSize(30);
//        barPainter.setFont(font);
//        if (_dataBuf[_cursorPosition] != InvData() && _dataBufSecond[_cursorPosition] != InvData())
//        {
//            double value1 = (double)_dataBuf[_cursorPosition] / 10;
//            double value2 = (double)_dataBufSecond[_cursorPosition] / 10;
//            QString trendStr = QString::number(value1, 'g', 2) + "/" + QString::number(value2, 'g', 2);
//            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/2, trendStr);
//        }
//        else
//        {
//            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/2, "---/---");
//        }
//    }

    barPainter.setPen(QPen(Qt::gray, 1, Qt::DotLine));
    barPainter.drawLine(rect().bottomLeft(), rect().bottomRight());
}

double TrendSubWaveWidget::_mapValue(TrendConvertDesc desc, int data)
{
    if (data == InvData())
    {
        return InvData();
    }

    double dpos = 0;
    dpos = (desc.max - data) * (desc.end - desc.start) / (desc.max - desc.min) + desc.start;

    if (dpos < desc.start)
    {
        dpos = desc.start;
    }
    else if (dpos > desc.end)
    {
        dpos = desc.end;
    }

    return dpos;
}
