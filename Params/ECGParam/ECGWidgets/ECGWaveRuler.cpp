#include "ECGWaveRuler.h"
#include "ECGWaveWidget.h"
#include "FontManager.h"
#include <QPainter>

/***************************************************************************************************
 * 绘图函数
 *
 * 参数：
 *      painter: 绘图对象
 **************************************************************************************************/
void ECGWaveRuler::paintItem(QPainter &painter)
{
    if (!_rulerHeight)
    {
        return;
    }

    int top = y() + ((height() - _rulerHeight) / 2);
    painter.setFont(font());
    painter.fillRect(x(), top, 3, _rulerHeight, palette().color(QPalette::WindowText));
    painter.setPen(palette().color(QPalette::WindowText));
    painter.drawText(x() + 10, y() + height() / 2 + 20, "1mV");
}

/***************************************************************************************************
 * 设置标尺高度
 **************************************************************************************************/
void ECGWaveRuler::setRulerHeight(int rulerHeight)
{
    _rulerHeight = rulerHeight;
    update();
}

/***************************************************************************************************
 * 构造函数
 *
 * 参数：
 *      wave: 标尺所属的波形控件
 **************************************************************************************************/
ECGWaveRuler::ECGWaveRuler(ECGWaveWidget *wave) : WaveWidgetItem(wave, true), _rulerHeight(0)
{
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
ECGWaveRuler::~ECGWaveRuler()
{
}
