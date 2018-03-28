#include "CO2Param.h"
#include "OxyCRGTrendWidgetRuler.h"
#include "OxyCRGTrendWidget.h"
#include "SystemManager.h"
#include "FontManager.h"
#include <QPainter>

/**************************************************************************************************
 * 绘图函数。
 *************************************************************************************************/
void OxyCRGTrendWidgetRuler::paintItem(QPainter &painter)
{
    if (_up == _low && _up != InvData())
    {
        return;
    }

    int xLeft = x() + 50;
    int xRight = x() + width() - 3;
    int yUp = y() + 1;
    int yLow = y() + height();
//    int yMid;


    painter.setFont(font());
    painter.setPen(QPen(Qt::white, 1, Qt::DotLine));

    int ystep = qRound(height() / 3);

    painter.setPen(QPen(Qt::white, 1, Qt::DotLine));

    for (int length = yUp + ystep; length < yUp + height() - 5; length += ystep)
    {
        painter.drawLine(xLeft, length, xRight, length);
    }

    // 上标尺
//    painter.drawLine(xLeft, yUp, xRight, yUp);
    if (_up != InvData())
    {
        painter.setPen(QPen(palette().windowText(), 1, Qt::SolidLine));
        painter.drawText(x() + 50, yUp, 50, 25, Qt::AlignTop | Qt::AlignLeft,
                         QString::number(_up));
    }
    painter.setPen(QPen(Qt::white, 1, Qt::DotLine));

//    // 中标尺
//    if (_mid != InvData())
//    {
//        yMid = (_mid - _low) * (yUp - yLow) / (_up - _low) + yLow;
//        painter.drawLine(xLeft, yMid, xRight, yMid);
//    }
//    else
//    {
//        yMid = (height() / 2) + y();
//        painter.drawLine(xLeft, yMid, xRight, yMid);
//    }

    // 下标尺
//    painter.drawLine(xLeft, yLow, xRight, yLow);
    if (_low != InvData())
    {
        painter.setPen(QPen(palette().windowText(), 1, Qt::SolidLine));
        painter.drawText(x() + 50, yLow - 25, 50, 25, Qt::AlignBottom | Qt::AlignLeft,
                         QString::number(_low));
    }


    int xstep = qRound(width() / 4);

    painter.setPen(QPen(Qt::white, 1, Qt::DotLine));

    for (int length = x() + xstep; length < xRight; length += xstep)
    {
        painter.drawLine(length, y() - 2, length, y() + height());
    }
}

/**************************************************************************************************
 * 设置标尺的刻度。
 *************************************************************************************************/
void OxyCRGTrendWidgetRuler::setRuler(int up, int mid, int low)
{
    _up = up;
    _mid = mid;
    _low = low;

    update();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
OxyCRGTrendWidgetRuler::OxyCRGTrendWidgetRuler(OxyCRGTrendWidget *wave) : WaveWidgetItem(wave, true)
{
    _up = 15;
    _mid = 8;
    _low = 0;

    _pixelWPitch = systemManager.getScreenPixelWPitch();
    _pixelHPitch = systemManager.getScreenPixelHPitch();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGTrendWidgetRuler::~OxyCRGTrendWidgetRuler()
{
}







