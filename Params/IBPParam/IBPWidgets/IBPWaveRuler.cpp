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

/**************************************************************************************************
 * 绘图函数。
 *************************************************************************************************/
void IBPWaveRuler::paintItem(QPainter &painter)
{
    if (_up == _low)
    {
        return;
    }

    int xLeft = x();
    int xRight = x() + width();
    int yUp = y();
    int yLow = y() + height() - 2;
    int yMid = (_mid - _low) * (yUp - yLow) / (_up - _low) + yLow;

    painter.setFont(font());
    painter.setPen(QPen(palette().windowText(), 1, Qt::DashLine));

    // 上标尺
    painter.drawText(QRectF(xLeft, yUp, xRight, yLow), Qt::AlignLeft | Qt::AlignTop, QString::number(_up) + "mmHg");
    painter.drawLine(xLeft, yUp, xRight, yUp);

    // 中标尺
    painter.drawLine(xLeft, yMid, xRight, yMid);

    // 下标尺
    painter.drawText(QRectF(xLeft, yUp, xRight, yLow - yMid/ 3 - 5), Qt::AlignLeft | Qt::AlignBottom, QString::number(_low) + "mmHg");
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

