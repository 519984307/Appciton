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

    int xLeft = x() + 50;
    int xRight = x() + width() - 3;
    int yUp = y() + 2;
    int yLow = y() + height() - 3;
    int yMid = (_mid - _low) * (yUp - yLow) / (_up - _low) + yLow;

    painter.setFont(font());
    painter.setPen(QPen(palette().windowText(), 1, Qt::DotLine));

    // 上标尺
    painter.drawLine(xLeft, yUp, xRight, yUp);

    // 中标尺
    painter.drawLine(xLeft, yMid, xRight, yMid);

    // 下标尺
    painter.drawLine(xLeft, yLow, xRight, yLow);
}

/**************************************************************************************************
 * 设置标尺的刻度。
 *************************************************************************************************/
void IBPWaveRuler::setRuler(double up, double mid, double low)
{
    _up = up;
    _mid = mid;
    _low = low;

    update();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPWaveRuler::IBPWaveRuler(IBPWaveWidget *wave) : WaveWidgetItem(wave, true)
{
    _up = 15.0;
    _mid = 7.5;
    _low = 0;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPWaveRuler::~IBPWaveRuler()
{
}

