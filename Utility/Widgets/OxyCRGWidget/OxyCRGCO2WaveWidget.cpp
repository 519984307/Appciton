/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/25
 **/


#include "OxyCRGCO2WaveWidget.h"
#include "OxyCRGTrendWaveWidget_p.h"
#include <QPainter>
#include "ColorManager.h"
#include "ParamInfo.h"
#include "ConfigManager.h"
#include "OxyCRGSymbol.h"

class OxyCRGCO2WaveWidgetPrivate : public OxyCRGTrendWaveWidgetPrivate
{
public:
    explicit OxyCRGCO2WaveWidgetPrivate(OxyCRGCO2WaveWidget * const q_ptr)
        :OxyCRGTrendWaveWidgetPrivate(q_ptr)
    {
    }

    void init();
};


void OxyCRGCO2WaveWidgetPrivate::init()
{
    QPalette palette = colorManager.getPalette(
                  paramInfo.getParamName(PARAM_CO2));
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    waveColor = color;

    int valueLow = 0;
    int valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|CO2Low", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|CO2High", valueHigh);
    QString strValueLow =  OxyCRGSymbol::convert(CO2LowTypes(valueLow));
    valueLow = strValueLow.toInt();
    QString strValueHigh =  OxyCRGSymbol::convert(CO2HighTypes(valueHigh));
    valueHigh = strValueHigh.toInt();
    rulerHigh = valueHigh;
    rulerLow = valueLow;
    name = "CO2";
}

OxyCRGCO2WaveWidget::OxyCRGCO2WaveWidget(const QString &waveName)
                     : OxyCRGTrendWaveWidget(waveName,
                                             new OxyCRGCO2WaveWidgetPrivate(this))
{
    Q_D(OxyCRGCO2WaveWidget);
    d->init();
}

OxyCRGCO2WaveWidget::~OxyCRGCO2WaveWidget()
{
}

void OxyCRGCO2WaveWidget::paintEvent(QPaintEvent *e)
{
    Q_D(OxyCRGCO2WaveWidget);

    OxyCRGTrendWaveWidget::paintEvent(e);

    QPainter painter(this);
    d->drawWave(&painter, d->waveRegion, d->waveBuffer, d->waveColor);
}

void OxyCRGCO2WaveWidget::setDataRate(int rate)
{
    Q_D(OxyCRGCO2WaveWidget);

    if (d->waveDataRate == rate)
    {
        return;
    }

    d->waveDataRate = rate;
    delete d->dataBuf;
    d->dataBuf = NULL;

    int dataLen = d->waveDataRate * MAX_WAVE_DURATION * 60;  // 最大8分钟数据
    d->dataBuf = new RingBuff<short>(dataLen);

    d->updateWaveDrawingContext();
}
