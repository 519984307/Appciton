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

TrendSubWaveWidget::~TrendSubWaveWidget()
{
    _trendInfo.reset();
}

void TrendSubWaveWidget::trendDataInfo(TrendGraphInfo info)
{
    _trendInfo = info;
    _cursorPosIndex = _trendInfo.alarmInfo.count() - 1;
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
}

void TrendSubWaveWidget::setThemeColor(QColor color)
{
    if (color != QColor(0, 0, 0))
    {
        _color = color;
    }
}

void TrendSubWaveWidget::setRulerRange(int down, int up)
{
    _valueY.max = up;
    _valueY.min = down;
}

void TrendSubWaveWidget::setTimeRange(unsigned leftTime, unsigned rightTime)
{
    _timeX.max = leftTime;
    _timeX.min = rightTime;
}

void TrendSubWaveWidget::cursorMove(int position)
{
    _cursorPosIndex = position;
    update();
}

SubParamID TrendSubWaveWidget::getSubParamID()
{
    return _id;
}

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

    // 边框线
    barPainter.setPen(QPen(Qt::gray, 1, Qt::DotLine));
    barPainter.drawLine(rect().bottomLeft(), rect().bottomRight());

    // 趋势波形图
    barPainter.setPen(QPen(_color, 2, Qt::SolidLine));
    if (!_trendInfo.alarmInfo.count())
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
            double fristData1 = _trendInfo.trendDataV2.at(i).data[0] / 10;
            double fristData2 = _trendInfo.trendDataV2.at(i + 1).data[0] / 10;
            double secondData1 = _trendInfo.trendDataV2.at(i).data[1] / 10;
            double secondData2 = _trendInfo.trendDataV2.at(i + 1).data[1] / 10;
            double fristY1 = _mapValue(_valueY, fristData1);
            double fristY2 = _mapValue(_valueY, fristData2);
            double secondY1 = _mapValue(_valueY, secondData1);
            double secondY2 = _mapValue(_valueY, secondData2);
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
    else if (_type == TREND_GRAPH_TYPE_ART_IBP)
    {
        for (int i = 0; i < _trendInfo.trendDataV3.count() - 1; i ++)
        {
            double fristData1 = _trendInfo.trendDataV3.at(i).data[0];
            double fristData2 = _trendInfo.trendDataV3.at(i + 1).data[0];
            double secondData1 = _trendInfo.trendDataV3.at(i).data[1];
            double secondData2 = _trendInfo.trendDataV3.at(i + 1).data[1];
            double thirdData1 = _trendInfo.trendDataV3.at(i).data[2];
            double thirdData2 = _trendInfo.trendDataV3.at(i + 1).data[2];

            double fristY1 = _mapValue(_valueY, fristData1);
            double fristY2 = _mapValue(_valueY, fristData2);
            double secondY1 = _mapValue(_valueY, secondData1);
            double secondY2 = _mapValue(_valueY, secondData2);
            double thirdY1 = _mapValue(_valueY, thirdData1);
            double thirdY2 = _mapValue(_valueY, thirdData2);
            double x1 = _mapValue(_timeX, _trendInfo.trendDataV3.at(i).timestamp);
            double x2 = _mapValue(_timeX, _trendInfo.trendDataV3.at(i + 1).timestamp);
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

            if (thirdY1 != InvData() && thirdY2 != InvData())
            {
                barPainter.drawLine(x1, thirdY1, x2, thirdY2);
            }
            else if (thirdY1 != InvData())
            {
                barPainter.drawPoint(x1, thirdY1);
            }
            else if (thirdY2 != InvData())
            {
                barPainter.drawPoint(x2, thirdY2);
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
    QRect dataRect(_trendDataHead, height()/4, width() - _trendDataHead, height() / 4 * 3);
    if (_type == TREND_GRAPH_TYPE_NORMAL)
    {
        if (_trendInfo.trendData.at(_cursorPosIndex).isAlarm)
        {
            barPainter.fillRect(dataRect, Qt::white);
        }
        else
        {
            barPainter.fillRect(dataRect, Qt::black);
        }
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
    else if (_type == TREND_GRAPH_TYPE_NIBP || _type == TREND_GRAPH_TYPE_ART_IBP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);

        if (_trendInfo.trendDataV3.at(_cursorPosIndex).isAlarm)
        {
            barPainter.fillRect(dataRect, Qt::white);
        }
        else
        {
            barPainter.fillRect(dataRect, Qt::black);
        }
        TrendDataType map =  _trendInfo.trendDataV3.at(_cursorPosIndex).data[0];
        TrendDataType dia = _trendInfo.trendDataV3.at(_cursorPosIndex).data[1];
        TrendDataType sys = _trendInfo.trendDataV3.at(_cursorPosIndex).data[2];

        if (map != InvData() && dia != InvData() && sys != InvData())
        {
            QString trendStr = QString::number(sys) + "/" + QString::number(dia);
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/2, trendStr);
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET + 10, height()/3*2, QString::number(map));
        }
        else
        {
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/2, "---/---");
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET + 10, height()/3*2, "(---)");
        }
    }
    else if (_type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);

        if (_trendInfo.trendDataV2.at(_cursorPosIndex).isAlarm)
        {
            barPainter.fillRect(dataRect, Qt::white);
        }
        else
        {
            barPainter.fillRect(dataRect, Qt::black);
        }
        double value1 = _trendInfo.trendDataV2.at(_cursorPosIndex).data[0] / 10;
        double value2 = _trendInfo.trendDataV2.at(_cursorPosIndex).data[1] / 10;

        if (value1 != InvData() && value2 != InvData())
        {
            QString trendStr = QString::number(value1, 'g', 2) + "/" + QString::number(value2, 'g', 2);
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/2, trendStr);
        }
        else
        {
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height()/2, "---/---");
        }
    }

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
