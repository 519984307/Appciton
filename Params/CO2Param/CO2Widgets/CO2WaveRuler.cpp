/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/3
 **/

#include "CO2Param.h"
#include "CO2WaveRuler.h"
#include "CO2WaveWidget.h"
#include <QPainter>
#include "FontManager.h"

/**************************************************************************************************
 * 绘图函数。
 *************************************************************************************************/
void CO2WaveRuler::paintItem(QPainter &painter)
{
    if (_up == _low)
    {
        return;
    }

    int xLeft = x();
    int xRight = x() + width();
    int yUp = y();
    int yLow = y() + height() - 2;
    int yMid = (yLow - yUp) / 2;

    int fontSize = fontManager.getFontSize(1);
    painter.setFont(fontManager.textFont(fontSize));
    painter.setPen(QPen(palette().windowText(), 1, Qt::DashLine));

    // 上标尺
    painter.drawText(QRectF(xLeft, yUp, 30, 20), Qt::AlignCenter | Qt::AlignTop, QString::number(_up));
    painter.drawLine(xLeft, yUp, xRight, yUp);

    // 中标尺
    painter.drawText(QRectF(xLeft, yMid - 10, 30, 20), Qt::AlignCenter | Qt::AlignHCenter, QString::number((_low + _up) / 2));
    painter.drawLine(xLeft + 30, yMid, xRight, yMid);

    // 下标尺
    painter.drawText(QRectF(xLeft, yLow - 20, 30, 20), Qt::AlignCenter | Qt::AlignBottom, QString::number(_low));
    painter.drawLine(xLeft, yLow, xRight, yLow);
}

/**************************************************************************************************
 * 设置标尺的刻度。
 *************************************************************************************************/
void CO2WaveRuler::setRuler(double up, double mid, double low)
{
    UnitType unit = co2Param.getUnit();
    if (unit == UNIT_MMHG)
    {
        int upV = Unit::convert(UNIT_MMHG, UNIT_PERCENT, up).toInt();
        int midV = Unit::convert(UNIT_MMHG, UNIT_PERCENT, mid).toInt();
        int lowV = Unit::convert(UNIT_MMHG, UNIT_PERCENT, low).toInt();
        _up = (upV + 5) / 10 * 10;
        _mid = (midV + 5) / 10 * 10;
        _low = (lowV + 5) / 10 * 10;
    }
    else
    {
        _up = up;
        _mid = mid;
        _low = low;
    }

    update();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CO2WaveRuler::CO2WaveRuler(CO2WaveWidget *wave) : WaveWidgetItem(wave, true)
{
    _up = 20.0;
    _mid = 10.0;
    _low = 0;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CO2WaveRuler::~CO2WaveRuler()
{
}

