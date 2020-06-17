/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/25
 **/

#include "TrendSubWaveWidget.h"
#include "IWidget.h"
#include <QPainter>
#include <QLabel>
#include "ParamInfo.h"
#include "ParamManager.h"
#include "FontManager.h"
#include "CO2Param.h"
#include "IConfig.h"
#include "TrendDataStorageManager.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/Utility/Utility.h"
#include "TrendGraphConfig.h"

#define GRAPH_POINT_NUMBER          120
#define DATA_INTERVAL_PIXEL         5
#define isEqual(a, b) (qAbs((a)-(b)) < 0.000001)
#define SCALE_NUM                   6   // 刻度线
#define SCALE_VALUE_AREA_HEIGHT    30  // 标尺值区域高度

TrendSubWaveWidget::TrendSubWaveWidget(SubParamID id, TrendGraphType type) : _id(id), _type(type),
    _trendInfo(TrendGraphInfo()), _timeX(TrendParamDesc()), _valueY(TrendParamDesc()), _xSize(0), _ySize(0),
    _trendDataHead(0), _cursorPosIndex(0),
    _maxValue(0), _minValue(0), _fristValue(true)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

TrendSubWaveWidget::~TrendSubWaveWidget()
{
    _trendInfo.reset();
}

void TrendSubWaveWidget::setWidgetParam(SubParamID id, TrendGraphType type)
{
    _id = id;
    _type = type;

    // 初始化当前单位标尺和默认单位标尺
    ParamID paramId = paramInfo.getParamID(_id);
    UnitType unitType = paramManager.getSubParamUnit(paramId, _id);
    UnitType defUnitType = paramInfo.getUnitOfSubParam(_id);
    ParamRulerConfig config = TrendGraphConfig::getParamRulerConfig(_id, unitType);
    _rulerY.max = config.upRuler;
    _rulerY.min = config.downRuler;
    _rulerY.scale = config.scale;
    if (unitType != defUnitType)
    {
        ParamRulerConfig defConfig = TrendGraphConfig::getParamRulerConfig(_id, defUnitType);
        _valueY.scale = defConfig.scale;
        _valueY.max = Unit::convert(defUnitType, unitType, static_cast<double>(config.upRuler) / config.scale,
                                    co2Param.getBaro()).toDouble() * _valueY.scale;
        _valueY.min = Unit::convert(defUnitType, unitType, static_cast<double>(config.downRuler) / config.scale,
                                    co2Param.getBaro()).toDouble() * _valueY.scale;
    }
    else
    {
        _valueY.min = config.downRuler;
        _valueY.max = config.upRuler;
        _valueY.scale = config.scale;
    }

    // 初始化参数名称和单位
    if (_type == TREND_GRAPH_TYPE_NORMAL || _type == TREND_GRAPH_TYPE_AG_TEMP
            || _type == TREND_GRAPH_TYPE_CO)
    {
        _paramName = trs(paramInfo.getSubParamName(_id));
        if (_type == TREND_GRAPH_TYPE_AG_TEMP)
        {
            _paramName = _paramName.right(_paramName.length() - 2) + "(Et/Fi)";
        }
        if (_id == SUB_PARAM_T1)
        {
            _paramName = "T1/T2";
        }
        if (_id == SUB_PARAM_SPO2)
        {
            _paramName = QString("%1/%2").arg(trs("SPO2")).arg(trs("SPO2_2"));
        }
    }
    else if (_type == TREND_GRAPH_TYPE_NIBP)
    {
        QString str = paramInfo.getSubParamName(_id);
        _paramName = str.left(str.length() - 4);
    }
    else if (_type == TREND_GRAPH_TYPE_ART_IBP)
    {
        _paramName = paramInfo.getIBPPressName(_id);
    }
    _paramUnit = trs(Unit::getSymbol(paramManager.getSubParamUnit(paramInfo.getParamID(_id), _id)));
}

void TrendSubWaveWidget::trendDataInfo(const TrendGraphInfo &info)
{
    _trendInfo = info;
    // 数据更新时判断是否为自动标尺,是则刷新标尺
    if (getAutoRuler())
    {
        _autoRulerCal();
    }
}

void TrendSubWaveWidget::loadTrendSubWidgetInfo(const TrendSubWidgetInfo &info)
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

int TrendSubWaveWidget::getLimitMax()
{
    return _valueY.max;
}

int TrendSubWaveWidget::getLimitMin()
{
    return _valueY.min;
}

int TrendSubWaveWidget::getLimitScale()
{
    return _valueY.scale;
}

void TrendSubWaveWidget::setThemeColor(QColor color)
{
    if (color != QColor(0, 0, 0))
    {
        _color = color;
    }
    else
    {
        _color = Qt::white;
    }
}

void TrendSubWaveWidget::setRulerRange(int down, int up, int scale)
{
    // 更新当前单位标尺和默认单位标尺
    _rulerY.max = up;
    _rulerY.min = down;
    _rulerY.scale = scale;
    ParamID paramId = paramInfo.getParamID(_id);
    UnitType unit = paramManager.getSubParamUnit(paramId, _id);
    UnitType defUnit = paramInfo.getUnitOfSubParam(_id);
    if (unit != defUnit)
    {
        _valueY.max = Unit::convert(defUnit, unit, static_cast<double>(up) / scale,
                                    co2Param.getBaro()).toDouble() * _valueY.scale;
        _valueY.min = Unit::convert(defUnit, unit, static_cast<double>(down) / scale,
                                    co2Param.getBaro()).toDouble() * _valueY.scale;
    }
    else
    {
        _valueY.max = _rulerY.max;
        _valueY.min = _rulerY.min;
    }
    TrendGraphConfig::setParamRulerConfig(_id, unit, _rulerY.min, _rulerY.max);
    update();
}

UnitType TrendSubWaveWidget::getUnitType()
{
    return paramManager.getSubParamUnit(paramInfo.getParamID(_id), _id);
}

int TrendSubWaveWidget::getAutoRuler(void)
{
    int autoRuler = 0;
    QString prefix = "TrendGraph|Ruler|";
    prefix += paramInfo.getSubParamName(_id, true);
    systemConfig.getNumAttr(prefix, "Auto", autoRuler);
    return autoRuler;
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

QPainterPath TrendSubWaveWidget::getTrendPainterPath(const QVector<TrendGraphDataV2> &dataVertor, int index)
{
    QPainterPath path;
    bool lastPointInvalid = true;
    QPointF lastPoint;
    QVector<TrendGraphDataV2>::ConstIterator iter = dataVertor.constBegin();
    for (; iter != dataVertor.constEnd(); iter++)
    {
        TrendDataType data = iter->data[index];
        if (data == InvData())
        {
            if (!lastPointInvalid)
            {
                path.lineTo(lastPoint);
                lastPointInvalid = true;
            }
            continue;
        }

        qreal x = _mapValue(_timeX, iter->timestamp);
        ParamID paramId = paramInfo.getParamID(_id);
        UnitType type = paramManager.getSubParamUnit(paramId, _id);
        qreal v = 0;
        if (paramId == PARAM_CO2)
        {
            v = Unit::convert(type, UNIT_PERCENT, data / 10.0, co2Param.getBaro()).toDouble();
        }
        else if (paramId == PARAM_TEMP)
        {
            QString vStr = Unit::convert(type, UNIT_TC, data / 10.0);
            v = vStr.toDouble();
        }
        else if (paramId == PARAM_SPO2)
        {
            v = data;
        }
        else
        {
            v = data / 10;
        }
        qreal value = _mapValue(_valueY, v);

        if (lastPointInvalid)
        {
            path.moveTo(x, value);
            lastPointInvalid = false;
        }
        else
        {
            if (!isEqual(lastPoint.y(), value))
            {
                path.lineTo(lastPoint);
                path.lineTo(x, value);
            }
        }

        lastPoint.rx() = x;
        lastPoint.ry() = value;
    }

    if (!lastPointInvalid)
    {
        path.lineTo(lastPoint);
    }
    return path;
}

QPainterPath TrendSubWaveWidget::getTrendPainterPath(const QVector<TrendGraphDataV3> &dataVertor, int index)
{
    QPainterPath path;
    bool lastPointInvalid = true;
    QPointF point;
    QVector<TrendGraphDataV3>::ConstIterator iter = dataVertor.constBegin();
    for (; iter != dataVertor.constEnd(); iter++)
    {
        TrendDataType data = iter->data[index];
        if (data == InvData())
        {
            // 画连续无效点
            if (!lastPointInvalid)
            {
                path.lineTo(point);
                lastPointInvalid = true;
            }
            continue;
        }

        qreal x = _mapValue(_timeX, iter->timestamp);
        qreal value = _mapValue(_valueY, data);

        // 判断是否为最后一个点
        if (lastPointInvalid)
        {
            path.moveTo(x, value);
            lastPointInvalid = false;
        }
        else
        {
            if (!isEqual(point.y(), value))
            {
                path.lineTo(point);
                path.lineTo(x, value);
            }
        }

        point.rx() = x;
        point.ry() = value;
    }

    if (!lastPointInvalid)
    {
        path.lineTo(point);
    }
    return path;
}

QList<QPainterPath> TrendSubWaveWidget::generatorPainterPath(const TrendGraphInfo &graphInfo)
{
    QList<QPainterPath> paths;
    switch (_type)
    {
    case TREND_GRAPH_TYPE_NIBP:
    {
        QPainterPath path;

        QVector<TrendGraphDataV3>::ConstIterator iter = graphInfo.trendDataV3.constBegin();
        for (; iter != graphInfo.trendDataV3.constEnd(); iter ++)
        {
            if (iter->data[0] == InvData() || !(iter->status & TrendDataStorageManager::CollectStatusNIBP))
            {
                continue;
            }

            qreal x = _mapValue(_timeX, iter->timestamp);
            int sysData = iter->data[0];
            int diaData = iter->data[1];
            int mapData = iter->data[2];
            qreal sys = _mapValue(_valueY, sysData);
            qreal dia = _mapValue(_valueY, diaData);
            qreal map = _mapValue(_valueY, mapData);

            if (sys == _valueY.start)
            {
                path.moveTo(x - 3, sys + 3);
                path.lineTo(x, sys);
                path.lineTo(x + 3, sys + 3);
            }
            else
            {
                path.moveTo(x - 3, sys - 3);
                path.lineTo(x, sys);
                path.lineTo(x + 3, sys - 3);
            }

            if (dia == _valueY.end)
            {
                path.moveTo(x - 3, dia - 3);
                path.lineTo(x, dia);
                path.lineTo(x + 3, dia - 3);
            }
            else
            {
                path.moveTo(x - 3, dia + 3);
                path.lineTo(x, dia);
                path.lineTo(x + 3, dia + 3);
            }

            path.moveTo(x, sys);
            path.lineTo(x, dia);

            path.moveTo(x - 3, map);
            path.lineTo(x + 3, map);
        }
        paths.append(path);
    }
    break;
    case TREND_GRAPH_TYPE_CO:
    {
        QPainterPath path;

        QVector<TrendGraphData>::ConstIterator iter = graphInfo.trendData.constBegin();
        for (; iter != graphInfo.trendData.constEnd(); iter ++)
        {
            if (iter->data == InvData())
            {
                continue;
            }
            qreal x = _mapValue(_timeX, iter->timestamp);
            qreal y = _mapValue(_valueY, iter->data);
            /* add a circle */
            QRectF boundRect(0.0, 0.0, 5.0, 5.0);
            boundRect.moveCenter(QPointF(x, y));
            path.addEllipse(boundRect);
        }
        paths.append(path);
    }
        break;
    case TREND_GRAPH_TYPE_AG_TEMP:
    {
        int trendNum = 2;       // 体温和co2有2个趋势参数
        for (int i = 0; i < trendNum; i++)
        {
            QPainterPath path = getTrendPainterPath(graphInfo.trendDataV2, i);
            paths.append(path);
        }
    }
    break;
    case TREND_GRAPH_TYPE_ART_IBP:
    {
        int trendNum = 3;       // IBP 动脉压有3个趋势参数
        for (int i = 0; i < trendNum; i++)
        {
            QPainterPath path = getTrendPainterPath(graphInfo.trendDataV3, i);
            paths.append(path);
        }
    }
    break;
    case TREND_GRAPH_TYPE_NORMAL:
    {
        QPainterPath path;

        QPointF lastPoint;
        bool lastPointInvalid = true;
        QVector<TrendGraphData>::ConstIterator iter = graphInfo.trendData.constBegin();
        for (; iter != graphInfo.trendData.constEnd(); iter ++)
        {
            if (iter->data == InvData())
            {
                if (!lastPointInvalid)
                {
                    path.lineTo(lastPoint);
                    lastPointInvalid = true;
                }
                continue;
            }

            qreal x = _mapValue(_timeX, iter->timestamp);
            qreal data = iter->data;
            if (_id == SUB_PARAM_PI)
            {
                data = data / 100;
            }
            else if (_id == SUB_PARAM_SPHB || _id == SUB_PARAM_SPMET)
            {
                data = data / 10;
            }
            qreal y = _mapValue(_valueY, data);

            if (lastPointInvalid)
            {
                path.moveTo(x, y);
                lastPointInvalid = false;
            }
            else
            {
                if (!isEqual(lastPoint.y(), y))
                {
                    path.lineTo(lastPoint);
                    path.lineTo(x, y);
                }
            }

            lastPoint.rx() = x;
            lastPoint.ry() = y;
        }

        if (!lastPointInvalid)
        {
            path.lineTo(lastPoint);
        }

        paths.append(path);
    }
    break;
    default:
        break;
    }
    return paths;
}

void TrendSubWaveWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    QPainter barPainter(this);
    barPainter.setPen(QPen(_color, 1, Qt::SolidLine));

    // 刻度线
    for (int i = 0; i <= SCALE_NUM; i++)
    {
        int offset = (_info.yBottom - _info.yTop) / SCALE_NUM * i + _info.yTop;
        barPainter.setPen(QPen(QColor(100, 100, 100, 200), 1, Qt::SolidLine));
        barPainter.drawLine(_info.xHead, offset, _info.xTail, offset);
        barPainter.setPen(QPen(_color, 1, Qt::SolidLine));
        if (i % 3 == 0)
        {
            barPainter.drawLine(_info.xHead - 10, offset, _info.xHead, offset);
        }
        else
        {
            barPainter.drawLine(_info.xHead - 5, offset, _info.xHead, offset);
        }
    }
    barPainter.drawLine(_info.xHead, _info.yTop, _info.xHead, _info.yBottom);

    // 边框线
    barPainter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    barPainter.drawLine(rect().bottomLeft(), rect().bottomRight());

    // 趋势波形图
    barPainter.setPen(QPen(_color, 2, Qt::SolidLine));
    if (!_trendInfo.alarmInfo.count())
    {
        // 趋势标尺上下限
        QRect upRulerRect(_info.xHead / 4, _info.yTop - 10, _info.xHead / 3 * 2, SCALE_VALUE_AREA_HEIGHT);
        QRect downRulerRect(_info.xHead / 4, _info.yBottom - 10, _info.xHead / 3 * 2, SCALE_VALUE_AREA_HEIGHT);
        QFont textfont = fontManager.textFont(fontManager.getFontSize(3));
        barPainter.setFont(textfont);
        barPainter.drawText(upRulerRect, Qt::AlignRight | Qt::AlignTop,
                            Util::convertToString(_rulerY.max, _rulerY.scale));
        barPainter.drawText(downRulerRect, Qt::AlignRight | Qt::AlignTop,
                            Util::convertToString(_rulerY.min, _rulerY.scale));

        QFont font;
        font.setPixelSize(15);
        barPainter.setFont(font);
        // 趋势参数名称
        QRect nameRect(_trendDataHead + 5, 5, width() - _trendDataHead - 10, 30);
        barPainter.drawText(nameRect, Qt::AlignLeft | Qt::AlignTop, _paramName);

        // 趋势参数单位
        QRect unitRect(_trendDataHead + 5, 5, width() - _trendDataHead - 10, 30);
        barPainter.drawText(unitRect, Qt::AlignRight | Qt::AlignTop, _paramUnit);
        font.setPixelSize(60);
        barPainter.setFont(font);

        // 趋势参数数据
        QRect dataRect(_trendDataHead + 5, height() / 5, width() - _trendDataHead, height() / 5 * 4);
        QTextOption option;
        option.setAlignment(Qt::AlignCenter);
        barPainter.drawText(dataRect, "---", option);
        return;
    }
    barPainter.save();
    QList<QPainterPath> paths = generatorPainterPath(_trendInfo);
    QList<QPainterPath>::ConstIterator iter;
    for (iter = paths.constBegin(); iter != paths.constEnd(); iter++)
    {
        barPainter.save();
        if (_type == TREND_GRAPH_TYPE_CO)
        {
            barPainter.fillPath(*iter, _color);
        }
        else
        {
            barPainter.drawPath(*iter);
        }
        barPainter.restore();
    }
    barPainter.restore();
    barPainter.setPen(QPen(_color, 1, Qt::SolidLine));

    // 趋势标尺上下限
    QRect upRulerRect(_info.xHead / 4, _info.yTop - 10, _info.xHead / 3 * 2, SCALE_VALUE_AREA_HEIGHT);
    QRect downRulerRect(_info.xHead / 4, _info.yBottom - 10, _info.xHead / 3 * 2, SCALE_VALUE_AREA_HEIGHT);
    QFont textfont = fontManager.textFont(fontManager.getFontSize(3));
    barPainter.setFont(textfont);
    barPainter.drawText(upRulerRect, Qt::AlignRight | Qt::AlignTop,
                        Util::convertToString(_rulerY.max, _rulerY.scale));
    barPainter.drawText(downRulerRect, Qt::AlignRight | Qt::AlignTop,
                        Util::convertToString(_rulerY.min, _rulerY.scale));

    QFont font;
    font.setPixelSize(15);
    barPainter.setFont(font);
    // 趋势参数名称
    QRect nameRect(_trendDataHead + 5, 5, width() - _trendDataHead - 10, 30);
    barPainter.drawText(nameRect, Qt::AlignLeft | Qt::AlignTop, _paramName);

    // 趋势参数单位
    QRect unitRect(_trendDataHead + 5, 5, width() - _trendDataHead - 10, 30);
    barPainter.drawText(unitRect, Qt::AlignRight | Qt::AlignTop, _paramUnit);
    font.setPixelSize(60);
    barPainter.setFont(font);

    // 趋势参数数据
    QRect dataRect(_trendDataHead + 5, height() / 5, width() - _trendDataHead, height() / 5 * 4);
    QTextOption option;
    option.setAlignment(Qt::AlignCenter);
    if (_type == TREND_GRAPH_TYPE_NORMAL)
    {
        if (_trendInfo.trendData.isEmpty())
        {
            return;
        }
        if (_trendInfo.trendData.at(_cursorPosIndex).isAlarm)
        {
            barPainter.fillRect(dataRect, Qt::white);
            if (_color == Qt::white)
            {
                QPen p = barPainter.pen();
                p.setColor(Qt::red);
                barPainter.setPen(p);
            }
        }
        else
        {
            barPainter.fillRect(dataRect, Qt::black);
        }
        TrendDataType value = _trendInfo.trendData.at(_cursorPosIndex).data;
        QString dataStr = QString::number(value);
        // CAP,LAP,RAP,ICP parameter data unit conversion
        switch (_id)
        {
        case SUB_PARAM_PI:
            dataStr = QString::number(((value * 1.0) / 100), 'f', 2);
            break;
        case SUB_PARAM_SPHB:
        case SUB_PARAM_SPMET:
            dataStr = QString::number(((value * 1.0) / 10), 'f', 1);
            break;
        case SUB_PARAM_CO_TB:
            dataStr = QString::number(value / 10.0f, 'f', 1);
            break;
        case SUB_PARAM_CVP_MAP:
        case SUB_PARAM_LAP_MAP:
        case SUB_PARAM_RAP_MAP:
        case SUB_PARAM_ICP_MAP:
        {
            ParamID paramId = paramInfo.getParamID(_id);
            UnitType type = paramManager.getSubParamUnit(paramId, _id);
            if (type != UNIT_MMHG && value != InvData())
            {
                dataStr = Unit::convert(type, UNIT_MMHG, value, co2Param.getBaro());
            }
        }
            break;
        default:
            break;
        }
        dataStr = value == InvData() ? InvStr() : dataStr;
        barPainter.drawText(dataRect, dataStr, option);
    }
    else if (_type == TREND_GRAPH_TYPE_NIBP || _type == TREND_GRAPH_TYPE_ART_IBP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);
        if (_trendInfo.trendDataV3.isEmpty())
        {
            return;
        }

        unsigned status = _trendInfo.trendDataV3.at(_cursorPosIndex).status;
        if (_trendInfo.trendDataV3.at(_cursorPosIndex).isAlarm)
        {
            if (_type == TREND_GRAPH_TYPE_NIBP)
            {
                if (status & TrendDataStorageManager::CollectStatusNIBP)
                {
                    barPainter.fillRect(dataRect, Qt::white);
                }
                else
                {
                    barPainter.fillRect(dataRect, Qt::black);
                }
            }
            else
            {
                barPainter.fillRect(dataRect, Qt::white);
            }
        }
        else
        {
            barPainter.fillRect(dataRect, Qt::black);
        }
        TrendDataType sys =  _trendInfo.trendDataV3.at(_cursorPosIndex).data[0];
        TrendDataType dia = _trendInfo.trendDataV3.at(_cursorPosIndex).data[1];
        TrendDataType map = _trendInfo.trendDataV3.at(_cursorPosIndex).data[2];
        QString sysStr;
        QString diaStr;
        QString mapStr;
        ParamID paramId = paramInfo.getParamID(_id);
        UnitType type = paramManager.getSubParamUnit(paramId, _id);
        if (type != UNIT_MMHG)
        {
            sysStr = Unit::convert(type, UNIT_MMHG, sys, co2Param.getBaro());
            diaStr = Unit::convert(type, UNIT_MMHG, dia, co2Param.getBaro());
            mapStr = Unit::convert(type, UNIT_MMHG, map, co2Param.getBaro());
        }
        else
        {
            sysStr = QString::number(sys);
            diaStr = QString::number(dia);
            mapStr = QString::number(map);
        }

        QRect upDataRect = dataRect.adjusted(0, 0, 0, - dataRect.height() / 2);
        QRect downDataRect = dataRect.adjusted(0, dataRect.height() / 2, 0, 0);
        QTextOption nibpOption;
        if (map != InvData() && dia != InvData() && sys != InvData())
        {
            if ((status & TrendDataStorageManager::CollectStatusNIBP) || _type == TREND_GRAPH_TYPE_ART_IBP)
            {
                QString trendStr = sysStr + "/" + diaStr;
                nibpOption.setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
                barPainter.drawText(upDataRect, trendStr, nibpOption);
                nibpOption.setAlignment(Qt::AlignTop | Qt::AlignHCenter);
                mapStr = "(" + mapStr + ")";
                barPainter.drawText(downDataRect, mapStr, nibpOption);
            }
            else
            {
                nibpOption.setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
                barPainter.drawText(upDataRect, "---/---", nibpOption);
                nibpOption.setAlignment(Qt::AlignTop | Qt::AlignHCenter);
                barPainter.drawText(downDataRect, "(---)", nibpOption);
            }
        }
        else
        {
            nibpOption.setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
            barPainter.drawText(upDataRect, "---/---", nibpOption);
            nibpOption.setAlignment(Qt::AlignTop | Qt::AlignHCenter);
            barPainter.drawText(downDataRect, "(---)", nibpOption);
        }
    }
    else if (_type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);

        if (_trendInfo.trendDataV2.isEmpty())
        {
            return;
        }

        if (_trendInfo.trendDataV2.at(_cursorPosIndex).isAlarm)
        {
            barPainter.fillRect(dataRect, Qt::white);
        }
        else
        {
            barPainter.fillRect(dataRect, Qt::black);
        }
        double value1 = _trendInfo.trendDataV2.at(_cursorPosIndex).data[0];
        double value2 = _trendInfo.trendDataV2.at(_cursorPosIndex).data[1];

        QString trendStr = QString("%1/%2");
        QString str1 = InvStr();
        QString str2 = InvStr();
        ParamID paramId = paramInfo.getParamID(_id);
        UnitType type = paramManager.getSubParamUnit(paramId, _id);

        if (paramId == PARAM_TEMP)
        {
            if (value1 != InvData())
            {
                str1 = Unit::convert(type, UNIT_TC, value1 / 10.0);
            }
            if (value2 != InvData())
            {
                str2 = Unit::convert(type, UNIT_TC, value2 / 10.0);
            }
        }
        else if (paramId == PARAM_CO2)
        {
            if (value1 != InvData())
            {
                str1 = Unit::convert(type, UNIT_PERCENT, value1 / 10.0, co2Param.getBaro());
            }
            if (value2 != InvData())
            {
                str2 = Unit::convert(type, UNIT_PERCENT, value2 / 10.0, co2Param.getBaro());
            }
        }
        else if (paramId == PARAM_SPO2)
        {
            if (value1 != InvData())
            {
                str1 = Unit::convert(type, UNIT_PERCENT, static_cast<int>(value1));
            }
            if (value2 != InvData())
            {
                str2 = Unit::convert(type, UNIT_PERCENT, static_cast<int>(value2));
            }
        }
        else
        {
            if (value1 != InvData())
            {
                str1 = QString::number((value1 / 10), 'f', 1);
            }
            if (value2 != InvData())
            {
                str2 = QString::number((value2 / 10), 'f', 1);
            }
        }
        trendStr = trendStr.arg(str1).arg(str2);
        barPainter.drawText(dataRect, trendStr, option);
    }
    else if (_type == TREND_GRAPH_TYPE_CO)
    {
        if (_trendInfo.trendData.isEmpty())
        {
            return;
        }

        barPainter.fillRect(dataRect, Qt::black);

        TrendDataType value = _trendInfo.trendData.at(_cursorPosIndex).data;
        QString dataStr = QString::number(value / 10.0, 'f', 1);
        dataStr = value == InvData() ? InvStr() : dataStr;
        barPainter.drawText(dataRect, dataStr, option);
    }
}

void TrendSubWaveWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);
    _cursorPosIndex = 0;
}

double TrendSubWaveWidget::_mapValue(TrendParamDesc desc, int data)
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

void TrendSubWaveWidget::_autoRulerCal()
{
    _fristValue = true;  // 以第一个数据为基准开始遍历
    switch (_type)
    {
    case TREND_GRAPH_TYPE_NIBP:
    case TREND_GRAPH_TYPE_ART_IBP:
    {
        QVector<TrendGraphDataV3>::ConstIterator iter = _trendInfo.trendDataV3.constBegin();
        for (; iter != _trendInfo.trendDataV3.constEnd(); iter++)
        {
            int num = 3;        // 参数数据为3个
            // 遍历每一个数据保证数据在标尺范围内
            for (int i = 0; i < num; i++)
            {
                TrendDataType data = iter->data[i];
                if (data == InvData())
                {
                    continue;
                }
                _updateAutoRuler(data);
            }
        }
        break;
    }
    case TREND_GRAPH_TYPE_AG_TEMP:
    {
        QVector<TrendGraphDataV2>::ConstIterator iter = _trendInfo.trendDataV2.constBegin();
        for (; iter != _trendInfo.trendDataV2.constEnd(); iter++)
        {
            int num = 2;        // 参数数据为2个
            // 遍历每一个数据保证数据在标尺范围内
            for (int i = 0; i < num; i++)
            {
                TrendDataType data = iter->data[i];
                if (data == InvData())
                {
                    continue;
                }
                _updateAutoRuler(data);
            }
        }
        break;
    }
    case TREND_GRAPH_TYPE_NORMAL:
    {
        QVector<TrendGraphData>::ConstIterator iter = _trendInfo.trendData.constBegin();
        for (; iter != _trendInfo.trendData.constEnd(); iter++)
        {
            TrendDataType data = iter->data;
            if (data == InvData())
            {
                continue;
            }
            _updateAutoRuler(data);
        }
        break;
    }
    case TREND_GRAPH_TYPE_CO:
        break;
    default:
        break;
    }

    // 自动标尺如果超出手动标尺设定范围则按照设置范围最大最小设定.
    ParamID paramId = paramInfo.getParamID(_id);
    UnitType unit = paramManager.getSubParamUnit(paramId, _id);
    UnitType defUnit = paramInfo.getUnitOfSubParam(_id);
    ParamRulerConfig config = TrendGraphConfig::getParamRulerConfig(_id, defUnit);
    int range = _valueY.max - _valueY.min;
    if (_valueY.min < config.minDownRuler)
    {
        _valueY.min = config.minDownRuler;
        _valueY.max = _valueY.min + range;
    }
    if (_valueY.max > config.maxUpRuler)
    {
        _valueY.max = config.maxUpRuler;
        _valueY.min = _valueY.max - range;
    }
    if (unit != defUnit)
    {
        _rulerY.max = Unit::convert(unit, defUnit, static_cast<double>(_valueY.max) / _valueY.scale,
                                    co2Param.getBaro()).toDouble() * _rulerY.scale;
        _rulerY.min = Unit::convert(unit, defUnit, static_cast<double>(_valueY.min) / _valueY.scale,
                                    co2Param.getBaro()).toDouble() * _rulerY.scale;
    }
    else
    {
        _rulerY.max = _valueY.max;
        _rulerY.min = _valueY.min;
    }
    TrendGraphConfig::setParamRulerConfig(_id, unit,
                                          _rulerY.min,
                                          _rulerY.max);
}

void TrendSubWaveWidget::_updateAutoRuler(TrendDataType data)
{
    // 数据的最大值和最小值以第一个数据为准
    if (_fristValue)
    {
        _maxValue = data;
        _minValue = data;
        _fristValue = false;
        int maxDiff = _valueY.max - _maxValue;
        if (maxDiff <= 0 || maxDiff > 10 || (static_cast<int>(_valueY.max) % 10))
        {
            _valueY.max = (_maxValue + (10 - _maxValue % 10));
        }
        int minDiff = _minValue - _valueY.min;
        if (minDiff <= 0 || minDiff > 10 || (static_cast<int>(_valueY.min) % 10))
        {
            int value = (_minValue % 10) ? (_minValue % 10) : 10;
            _valueY.min = (_minValue - value);
        }
    }

    // 大于当前数据最大值.
    if (data > _maxValue)
    {
        _maxValue = data;
        int maxDiff = _valueY.max - _maxValue;
        if (maxDiff <= 0 || maxDiff > 10 || (static_cast<int>(_valueY.max) % 10))
        {
            _valueY.max = (_maxValue + (10 - _maxValue % 10));
        }
    }

    // 小于当前数据最小值.
    if (data < _minValue)
    {
        _minValue = data;
        int minDiff = _minValue - _valueY.min;
        if (minDiff <= 0 || minDiff > 10 || (static_cast<int>(_valueY.min) % 10))
        {
            int value = (_minValue % 10) ? (_minValue % 10) : 10;
            _valueY.min = (_minValue - value);
        }
    }
}
