/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/25
 **/


#include "OxyCRGRESPWaveWidget.h"
#include <QPainter>
#include "ColorManager.h"
#include "ParamInfo.h"
#include "IConfig.h"
#include "OxyCRGSymbol.h"

class OxyCRGRESPWaveWidgetPrivate : public OxyCRGTrendWaveWidgetPrivate
{
public:
    OxyCRGRESPWaveWidgetPrivate()
                : lastDataRate(0)
    {
    }
    void init();
    int lastDataRate;
};

void OxyCRGRESPWaveWidgetPrivate::init()
{
    QPalette palette = colorManager.getPalette(
                  paramInfo.getParamName(PARAM_RESP));
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    waveColor = color;

    rulerHigh = 255;
    rulerLow = 0;

    drawRuler = false;

    int dataLen = lastDataRate * MAX_WAVE_DURATION * 60;  // 最大8分钟数据
    dataBuf = new RingBuff<short>(dataLen);
    name = "RESP";
}

OxyCRGRESPWaveWidget::OxyCRGRESPWaveWidget(const QString &waveName)
                     : OxyCRGTrendWaveWidget(waveName,
                                             new OxyCRGRESPWaveWidgetPrivate)
{
    Q_D(OxyCRGRESPWaveWidget);
    d->init();
}

OxyCRGRESPWaveWidget::~OxyCRGRESPWaveWidget()
{
    Q_D(OxyCRGRESPWaveWidget);
    delete d->dataBuf;
    d->dataBuf = NULL;
}

void OxyCRGRESPWaveWidget::paintEvent(QPaintEvent *e)
{
    Q_D(OxyCRGRESPWaveWidget);
    OxyCRGTrendWaveWidget::paintEvent(e);

    QPainter painter(this);
    d->drawWave(&painter, d->waveRegion, d->waveBuffer, d->waveColor);
}

void OxyCRGRESPWaveWidget::showEvent(QShowEvent *e)
{
    Q_D(OxyCRGRESPWaveWidget);
    OxyCRGTrendWaveWidget::showEvent(e);

    if (d->isClearWaveData == false)
    {
        d->isClearWaveData = true;
        return;
    }

    if (d->dataBuf)
    {
        d->dataBuf->clear();
    }
}

void OxyCRGRESPWaveWidget::hideEvent(QHideEvent *e)
{
    Q_D(OxyCRGRESPWaveWidget);
    OxyCRGTrendWaveWidget::hideEvent(e);

    if (d->isClearWaveData == false)
    {
        d->isClearWaveData = true;
        return;
    }

    if (d->dataBuf)
    {
        d->dataBuf->clear();
    }
}

void OxyCRGRESPWaveWidget::setDataRate(int rate)
{
    Q_D(OxyCRGRESPWaveWidget);
    OxyCRGTrendWaveWidget::setDataRate(rate);

    if (d->lastDataRate == rate)
    {
        return;
    }
    d->lastDataRate = rate;

    delete d->dataBuf;
    d->dataBuf = NULL;

    int dataLen = d->lastDataRate * MAX_WAVE_DURATION * 60;  // 最大8分钟数据
    d->dataBuf = new RingBuff<short>(dataLen);
}
