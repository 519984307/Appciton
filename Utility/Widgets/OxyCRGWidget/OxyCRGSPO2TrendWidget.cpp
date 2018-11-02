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
#include <QPainter>
#include "ColorManager.h"
#include "ParamInfo.h"
#include "IConfig.h"
#include "OxyCRGSymbol.h"

class OxyCRGSPO2TrendWidgetPrivate : public OxyCRGTrendWaveWidgetPrivate
{
public:
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

    int dataLen = waveDataRate * MAX_WAVE_DURATION * 60;  // 最大8分钟数据
    dataBuf = new RingBuff<short>(dataLen);
    name = "SPO2";
}

OxyCRGSPO2TrendWidget::OxyCRGSPO2TrendWidget(const QString &waveName)
                     : OxyCRGTrendWaveWidget(waveName,
                                             new OxyCRGSPO2TrendWidgetPrivate)
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

    int w = rect().width() - WX_SHIFT * 2;
    int h = rect().height();
    OxyCRGInterval interval = getIntervalTime();
    int intervalTime = 1 * 60;
    // 数据速率
    int dataRate = d->waveDataRate;

    switch (interval)
    {
    case OxyCRG_Interval_1:
        intervalTime = 1 * 60;
        break;
    case OxyCRG_Interval_2:
        intervalTime = 2 * 60;
        break;
    case OxyCRG_Interval_4:
        intervalTime = 4 * 60;
        break;
    case OxyCRG_Interval_8:
        intervalTime = 8 * 60;
        break;
    case OxyCRG_Interval_NR:
        intervalTime = 8 * 60;
        break;
    }
    // 每次需要显示的数据点数
    int dataCount = dataRate * intervalTime;

    QPainter painter(this);
    painter.setPen(QPen(d->waveColor, 2, Qt::SolidLine));
    QPainterPath path;

    double xAllShift = w * 1.0 / intervalTime;
    double xStep = xAllShift / dataRate;
    int rulerHigh = d->rulerHigh;
    int rulerLow = d->rulerLow;
    int dataSize = d->dataBuf->dataSize();
    int index = (dataSize - dataCount > 0) ? (dataSize - dataCount) : (0);
    double curX = rect().width()
                + rect().x()
                - WX_SHIFT
                - dataSize * xStep;
    if (curX < rect().x() + WX_SHIFT)
    {
        curX = rect().x() + WX_SHIFT;
    }
    double dataH = h - (d->dataBuf->at(index) - rulerLow) * 1.0 /
                   (rulerHigh - rulerLow) * h;
    path.moveTo(curX, dataH);
    for (int i = index; i < dataSize; i++)
    {
        dataH = h - (d->dataBuf->at(i) - rulerLow) * 1.0 /
                (rulerHigh - rulerLow) * h;
        curX += xStep;
        if (curX > (rect().width() + rect().x() - WX_SHIFT))
        {
            curX = rect().width() + rect().x() - WX_SHIFT;
            path.lineTo(curX, dataH);
            break;
        }

       path.lineTo(curX, dataH);
    }
    painter.drawPath(path);
}

void OxyCRGSPO2TrendWidget::showEvent(QShowEvent *e)
{
    Q_D(OxyCRGSPO2TrendWidget);
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

void OxyCRGSPO2TrendWidget::hideEvent(QHideEvent *e)
{
    Q_D(OxyCRGSPO2TrendWidget);
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

void OxyCRGSPO2TrendWidget::addTrendData(int data)
{
    addWaveData(data);
}
