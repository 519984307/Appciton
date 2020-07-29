/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/3
 **/

#include "IBPParam.h"
#include "IBPWaveRuler.h"
#include "IBPWaveWidget.h"
#include <QPainter>
#include "FontManager.h"

// Ruler text size
#define RULER_TEXT_MAX_WIDTH     (40)
#define RULER_TEXT_MAX_HEITHT    (20)

/**************************************************************************************************
 * 绘图函数。
 *************************************************************************************************/
void IBPWaveRuler::paintItem(QPainter &painter)
{
    if (_up == _low)
    {
        return;
    }
    // get cur unit type
    UnitType curUnit = ibpParam.getUnit();
    QString lowRulerText = QString::number(_low);
    QString highRulerText = QString::number(_up);
    QString midRulerText = QString::number(_mid);

    // Unit change, get the ruler data corresponding to the unit
    if (curUnit != UNIT_MMHG)
    {
        lowRulerText = Unit::convert(curUnit, UNIT_MMHG, _low);
        highRulerText = Unit::convert(curUnit, UNIT_MMHG, _up);
        midRulerText = Unit::convert(curUnit, UNIT_MMHG, _mid);
    }

    int xLeft = x();
    int xRight = x() + width();
    int yUp = y();
    int yLow = y() + height() - 2;
    int yMid = (_mid - _low) * (yUp - yLow) / (_up - _low) + yLow;

    int fontSize = fontManager.getFontSize(1);
    painter.setFont(fontManager.textFont(fontSize));
    painter.setPen(QPen(palette().windowText(), 1, Qt::DashLine));

    // 上标尺
    painter.drawText(QRectF(xLeft, yUp, RULER_TEXT_MAX_WIDTH, RULER_TEXT_MAX_HEITHT),
                      Qt::AlignCenter | Qt::AlignTop, highRulerText);
    painter.drawLine(xLeft, yUp, xRight, yUp);

    // 中标尺
    painter.drawText(QRectF(xLeft, yMid - 10, RULER_TEXT_MAX_WIDTH, RULER_TEXT_MAX_HEITHT),
                      Qt::AlignCenter | Qt::AlignHCenter, midRulerText);
    painter.drawLine(xLeft + RULER_TEXT_MAX_WIDTH, yMid, xRight, yMid);

    // 下标尺
    painter.drawText(QRectF(xLeft, yLow - 20, RULER_TEXT_MAX_WIDTH, RULER_TEXT_MAX_HEITHT),
                      Qt::AlignCenter | Qt::AlignBottom, lowRulerText);
    painter.drawLine(xLeft, yLow, xRight, yLow);
}

/**************************************************************************************************
 * 设置标尺的刻度。
 *************************************************************************************************/
void IBPWaveRuler::setRuler(int up, int mid, int low)
{
    _up = up;
    _mid = mid;
    _low = low;

    update();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPWaveRuler::IBPWaveRuler(IBPWaveWidget *wave) : WaveWidgetItem(wave, true),
    _up(30), _mid(15), _low(0)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPWaveRuler::~IBPWaveRuler()
{
}

