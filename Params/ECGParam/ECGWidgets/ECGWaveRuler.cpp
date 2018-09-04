/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/3
 **/

#include "ECGWaveRuler.h"
#include "ECGWaveWidget.h"
#include "FontManager.h"
#include <QPainter>
#include "SystemManager.h"
#include <QRect>
#include "Debug.h"
#include "ThemeManager.h"

/***************************************************************************************************
 * 绘图函数
 *
 * 参数：
 *      painter: 绘图对象
 **************************************************************************************************/
void ECGWaveRuler::paintItem(QPainter &painter)
{
    painter.setPen(QPen(palette().color(QPalette::Foreground), 3, Qt::SolidLine));
    QFont font;
    font.setPointSize(15);
    font.setBold(true);
    painter.setFont(font);
    float rulerPos = _waveWidget->width() / 8;
    float middlePos = _waveWidget->height() / 2;
    QRect rulerRect(rulerPos + 5, middlePos / 2 * 3, 100, middlePos / 2);
    switch (_gain)
    {
    case ECG_GAIN_X0125:
        painter.drawLine(rulerPos, middlePos - 10 / _pixelHPitch / 16, rulerPos,
                         middlePos + 10 / _pixelHPitch / 16);
        painter.drawText(rulerRect, "1mV");
        break;
    case ECG_GAIN_X025:
        painter.drawLine(rulerPos, middlePos - 10 / _pixelHPitch / 8, rulerPos,
                         middlePos + 10 / _pixelHPitch / 8);
        painter.drawText(rulerRect, "1mV");
        break;
    case ECG_GAIN_X05:
        painter.drawLine(rulerPos, middlePos - 10 / _pixelHPitch / 4, rulerPos,
                         middlePos + 10 / _pixelHPitch / 4);
        painter.drawText(rulerRect, "1mV");
        break;
    case ECG_GAIN_X10:
        painter.drawLine(rulerPos, middlePos - 10 / _pixelHPitch / 2, rulerPos,
                         middlePos + 10 / _pixelHPitch / 2);
        painter.drawText(rulerRect, "1mV");
        break;
    case ECG_GAIN_X20:
        painter.drawLine(rulerPos, middlePos - 10 / _pixelHPitch / 2, rulerPos,
                         middlePos + 10 / _pixelHPitch / 2);
        painter.drawText(rulerRect, "0.5mV");
        break;
    case ECG_GAIN_X40:
        painter.drawLine(rulerPos, middlePos - 10 / _pixelHPitch / 2, rulerPos,
                         middlePos + 10 / _pixelHPitch / 2);
        painter.drawText(rulerRect, "0.25mV");
        break;
    default:
        break;
    }
}

void ECGWaveRuler::setGain(ECGGain g)
{
    _gain = g;
    setBackground(true);
}

/***************************************************************************************************
 * 构造函数
 *
 * 参数：
 *      wave: 标尺所属的波形控件
 **************************************************************************************************/
ECGWaveRuler::ECGWaveRuler(ECGWaveWidget *wave) :
    WaveWidgetItem(wave, true),
    _gain(ECG_GAIN_X10), _pixelHPitch(0),
    _waveWidget(wave)
{
    _pixelHPitch = systemManager.getScreenPixelHPitch();
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
ECGWaveRuler::~ECGWaveRuler()
{
}
