/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/25
 **/


#include "OxyCRGSPO2TrendWidget.h"
#include "OxyCRGTrendWaveWidget_p.h"
#include <QPainter>
#include "ColorManager.h"
#include "ParamInfo.h"
#include "ConfigManager.h"
#include "OxyCRGSymbol.h"

class OxyCRGSPO2TrendWidgetPrivate : public OxyCRGTrendWaveWidgetPrivate
{
public:
    explicit OxyCRGSPO2TrendWidgetPrivate(OxyCRGTrendWaveWidget * const q_ptr)
        : OxyCRGTrendWaveWidgetPrivate(q_ptr)
    {}
    void init();
};

void OxyCRGSPO2TrendWidgetPrivate::init()
{
    QPalette palette = colorManager.getPalette(
                  paramInfo.getParamName(PARAM_SPO2));
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    waveColor = color;

    int valueLow = 0;
    int valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|SPO2Low", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|SPO2High", valueHigh);
    QString strValueLow =  OxyCRGSymbol::convert(SPO2LowTypes(valueLow));
    valueLow = strValueLow.toInt();
    QString strValueHigh =  OxyCRGSymbol::convert(SPO2HighTypes(valueHigh));
    valueHigh = strValueHigh.toInt();
    rulerHigh = valueHigh;
    rulerLow = valueLow;

    name = "SPO2";
}

OxyCRGSPO2TrendWidget::OxyCRGSPO2TrendWidget(const QString &waveName)
                     : OxyCRGTrendWaveWidget(waveName,
                                             new OxyCRGSPO2TrendWidgetPrivate(this))
{
    Q_D(OxyCRGSPO2TrendWidget);
    d->init();
}

OxyCRGSPO2TrendWidget::~OxyCRGSPO2TrendWidget()
{
}

void OxyCRGSPO2TrendWidget::paintEvent(QPaintEvent *e)
{
    Q_D(OxyCRGSPO2TrendWidget);
    OxyCRGTrendWaveWidget::paintEvent(e);

    QPainter painter(this);
    d->drawWave(&painter, d_ptr->waveRegion, d_ptr->waveBuffer, d_ptr->waveColor);
}

void OxyCRGSPO2TrendWidget::addTrendData(int data)
{
    addWaveData(data);
}
