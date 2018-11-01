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
#include <QPainter>
#include "ColorManager.h"
#include "ParamInfo.h"
#include "IConfig.h"
#include "OxyCRGSymbol.h"

class OxyCRGCO2WaveWidgetPrivate : public OxyCRGTrendWaveWidgetPrivate
{
public:
    OxyCRGCO2WaveWidgetPrivate()
                : lastDataRate(0)
    {
    }
    int lastDataRate;
};

OxyCRGCO2WaveWidget::OxyCRGCO2WaveWidget(const QString &waveName)
                     : OxyCRGTrendWaveWidget(waveName,
                                             new OxyCRGCO2WaveWidgetPrivate)
{
    init();
}

OxyCRGCO2WaveWidget::~OxyCRGCO2WaveWidget()
{
    Q_D(OxyCRGCO2WaveWidget);
    delete d->dataBuf;
    d->dataBuf = NULL;
}

void OxyCRGCO2WaveWidget::paintEvent(QPaintEvent *e)
{
    Q_D(OxyCRGCO2WaveWidget);

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

    QList<QPoint> pointList;
    for (int i = index; i < dataSize; i++)
    {
        // TODO : check invalid wave data point
        dataH = h - (d->dataBuf->at(i) - rulerLow) * 1.0 /
                (rulerHigh - rulerLow) * h;
        curX += xStep;
        // 超出界限，不再添加波形
        if (curX > (rect().width() + rect().x() - WX_SHIFT))
        {
            curX = rect().width() + rect().x() - WX_SHIFT;
            path.lineTo(curX, dataH);
            break;
        }

        path.lineTo(curX, dataH);
    }
    painter.drawPath(path);
    // 画虚线
    if (pointList.isEmpty())
    {
        return;
    }

    painter.setPen(QPen(d->waveColor, 2, Qt::DotLine));
    int count = pointList.count() / 2;
    for (int i = 0; i < count; i++)
    {
        painter.drawLine(pointList.at(2 * i), pointList.at(2 * i + 1));
    }
}

void OxyCRGCO2WaveWidget::showEvent(QShowEvent *e)
{
    Q_D(OxyCRGCO2WaveWidget);
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

void OxyCRGCO2WaveWidget::hideEvent(QHideEvent *e)
{
    Q_D(OxyCRGCO2WaveWidget);
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

void OxyCRGCO2WaveWidget::setDataRate(int rate)
{
    Q_D(OxyCRGCO2WaveWidget);
    OxyCRGTrendWaveWidget::setDataRate(rate);

    if (d->lastDataRate == rate)
    {
        return;
    }
    d->lastDataRate = rate;

    delete d->dataBuf;
    d->dataBuf = NULL;

    int dataLen = d->lastDataRate * 8 * 60;  // 最大8分钟数据
    d->dataBuf = new RingBuff<short>(dataLen);
    d->dataBufIndex = 0;
    d->dataBufLen = dataLen;
}

void OxyCRGCO2WaveWidget::init()
{
    Q_D(OxyCRGCO2WaveWidget);

    QPalette palette = colorManager.getPalette(
                  paramInfo.getParamName(PARAM_CO2));
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    d->waveColor = color;

    int valueLow = 0;
    int valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|CO2Low", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|CO2High", valueHigh);
    QString strValueLow =  OxyCRGSymbol::convert(CO2LowTypes(valueLow));
    valueLow = strValueLow.toInt();
    QString strValueHigh =  OxyCRGSymbol::convert(CO2HighTypes(valueHigh));
    valueHigh = strValueHigh.toInt();
    d->rulerHigh = valueHigh;
    d->rulerLow = valueLow;

    int dataLen = d->lastDataRate * 8 * 60;  // 最大8分钟数据
    d->dataBuf = new RingBuff<short>(dataLen);
    d->dataBufIndex = 0;
    d->dataBufLen = dataLen;
    d->name = "CO2";
}
