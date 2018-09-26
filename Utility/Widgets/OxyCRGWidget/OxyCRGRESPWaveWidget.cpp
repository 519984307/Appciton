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
    int lastDataRate;
};

OxyCRGRESPWaveWidget::OxyCRGRESPWaveWidget(const QString &waveName)
                     : OxyCRGTrendWaveWidget(waveName,
                                             new OxyCRGRESPWaveWidgetPrivate)
{
    init();
}

OxyCRGRESPWaveWidget::~OxyCRGRESPWaveWidget()
{
    Q_D(OxyCRGRESPWaveWidget);
    delete d->dataBuf;
    d->dataBuf = NULL;
    delete d->flagBuf;
    d->flagBuf = NULL;
}

void OxyCRGRESPWaveWidget::paintEvent(QPaintEvent *e)
{
    Q_D(OxyCRGRESPWaveWidget);
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
    int rulerHigh = 255;
    int rulerLow = -256;
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
        dataH = h - (d->dataBuf->at(i) - rulerLow) * 1.0 /
                (rulerHigh - rulerLow) * h;
        curX += xStep;
        if (curX > (rect().width() + rect().x() - WX_SHIFT))
        {
            curX = rect().width() + rect().x() - WX_SHIFT;
            path.lineTo(curX, dataH);
            break;
        }

        // 添加虚线坐标
        if (d->flagBuf->at(i))
        {
            pointList.append(QPoint(curX, dataH));
            int dataNextH = h - (d->dataBuf->at(i + 1) - rulerLow) * 1.0 /
                    (rulerHigh - rulerLow) * h;
            int curNextX = curX + xStep;
            pointList.append(QPoint(curNextX, dataNextH));
            continue;
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

    if (d->flagBuf)
    {
        d->flagBuf->clear();
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

    if (d->flagBuf)
    {
        d->flagBuf->clear();
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
    delete d->flagBuf;
    d->flagBuf = NULL;

    int dataLen = d->lastDataRate * 8 * 60;  // 最大8分钟数据
    d->flagBuf = new RingBuff<bool>(dataLen);
    d->dataBuf = new RingBuff<short>(dataLen);
    d->dataBufIndex = 0;
    d->dataBufLen = dataLen;
}

void OxyCRGRESPWaveWidget::init()
{
    Q_D(OxyCRGRESPWaveWidget);

    QPalette palette = colorManager.getPalette(
                  paramInfo.getParamName(PARAM_RESP));
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    d->waveColor = color;

    d->rulerHigh = InvData();
    d->rulerLow = InvData();

    int dataLen = d->lastDataRate * 8 * 60;  // 最大8分钟数据
    d->flagBuf = new RingBuff<bool>(dataLen);
    d->dataBuf = new RingBuff<short>(dataLen);
    d->dataBufIndex = 0;
    d->dataBufLen = dataLen;
    d->name = "RESP";
}
