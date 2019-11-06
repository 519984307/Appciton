/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/20
 **/

#include "OxyCRGRRHRWaveWidget.h"
#include "OxyCRGTrendWaveWidget_p.h"
#include "Framework/Utility/RingBuff.h"
#include <QPainterPath>
#include <QPainter>
#include <QPointer>
#include "ColorManager.h"
#include "ParamInfo.h"
#include "ConfigManager.h"
#include "OxyCRGSymbol.h"
#include "ECGParam.h"
#include "RESPParam.h"
#include <QTimer>
#include "SystemManager.h"
#include "FontManager.h"
#include "LanguageManager.h"

class OxyCRGRRHRWaveWidgetPrivate :
    public OxyCRGTrendWaveWidgetPrivate
{
public:
    explicit OxyCRGRRHRWaveWidgetPrivate(OxyCRGRRHRWaveWidget * const q_ptr)
        : OxyCRGTrendWaveWidgetPrivate(q_ptr),
          rrDataBuf(NULL),
          rrWaveBuffer(NULL),
          rrDataBufIndex(0),
          rrDataBufLen(0),
          rrRulerHigh(InvData()),
          rrRulerLow(InvData()),
          rrWaveColor(Qt::green),
          isShowRr(false),
          rrWaveName(""),
          rrPointGap(0),
          rrPointGapSumFraction(0.0)
    {
    }

    ~OxyCRGRRHRWaveWidgetPrivate()
    {
        if (rrWaveBuffer)
        {
            delete rrWaveBuffer;
            rrWaveBuffer = NULL;
        }

        if (rrDataBuf)
        {
            delete rrDataBuf;
            rrDataBuf = NULL;
        }
    }

    void init();

    /* reimplement */
    void updateWaveDrawingContext();

    /* reimplement */
    void reloadWaveBuffer();

    /**
     * @brief reloadRRwaveBuffer reload the rr wave buffer
     */
    void reloadRRwaveBuffer();

    RingBuff<short> *rrDataBuf;          // 波形数据缓存
    OxyCRGWaveBuffer *rrWaveBuffer;      // rr wave buffer

    int rrDataBufIndex;                  // 波形数据缓存下标

    int rrDataBufLen;                    // 波形数据长度

    int rrRulerHigh;                     // 标尺高值

    int rrRulerLow;                      // 标尺低值

    QColor rrWaveColor;                  // 波形颜色

    bool isShowRr;                       // 是否显示RR趋势波形

    QString rrWaveName;                  // rr波形名称
    float rrPointGap;                    // rr wave ponit gap
    float rrPointGapSumFraction;         // the fraction part of the rr wave gap sum
};

void OxyCRGRRHRWaveWidgetPrivate::init()
{
    // rr标尺的颜色更深。
    QPalette palette = colorManager.getPalette(
                           paramInfo.getParamName(PARAM_RESP));
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    rrWaveColor = color;
    rrWaveName = "RR";
    // rr设置标尺值
    int valueLow = 0;
    int valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|RRLow", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|RRHigh", valueHigh);
    QString strValueLow =  OxyCRGSymbol::convert(RRLowTypes(valueLow));
    valueLow = strValueLow.toInt();
    QString strValueHigh =  OxyCRGSymbol::convert(RRHighTypes(valueHigh));
    valueHigh = strValueHigh.toInt();
    rrRulerHigh = valueHigh;
    rrRulerLow = valueLow;
    // 申请存储rr波形数据堆空间
    rrDataBufIndex = 0;
    rrDataBufLen = waveDataRate * 8 * 60;  // 最大8分钟数据
    rrDataBuf = new RingBuff<short>(rrDataBufLen);

    // hr标尺的颜色更深。
    palette = colorManager.getPalette(
                  paramInfo.getParamName(PARAM_ECG));
    color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    waveColor = color;
    // hr设置标尺值
    valueLow = 0;
    valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|HRLow", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|HRHigh", valueHigh);
    strValueLow =  OxyCRGSymbol::convert(HRLowTypes(valueLow));
    valueLow = strValueLow.toInt();
    strValueHigh =  OxyCRGSymbol::convert(HRHighTypes(valueHigh));
    valueHigh = strValueHigh.toInt();
    rulerHigh = valueHigh;
    rulerLow = valueLow;
    name = "HR_PR";

    int index = 0;
    currentConfig.getNumValue("OxyCRG|Trend1", index);
    if (index == 0)
    {
        isShowRr = false;
    }
    else
    {
        isShowRr = true;
    }
}

void OxyCRGRRHRWaveWidgetPrivate::updateWaveDrawingContext()
{
    int sampleCount = getIntervalSeconds(interval) * waveDataRate;

    int segmentCount = waveRegion.width();
    if (sampleCount < segmentCount)
    {
        segmentCount = sampleCount + 1;
    }

    if (!waveBuffer)
    {
        waveBuffer = new OxyCRGWaveBuffer(segmentCount);
    }
    else
    {
        waveBuffer->resize(segmentCount);
    }

    if (!rrWaveBuffer)
    {
        rrWaveBuffer = new OxyCRGWaveBuffer(segmentCount);
    }
    else
    {
        rrWaveBuffer->resize(segmentCount);
    }

    pointGap = 1.0 * waveRegion.width() / sampleCount;

    if (pointGap < 1)
    {
        waveBuffer->highDensitiy = true;
    }

    pointGapSumFraction = 0.0;

    rrPointGap = pointGap;
    rrPointGapSumFraction = pointGapSumFraction;
}

void OxyCRGRRHRWaveWidgetPrivate::reloadWaveBuffer()
{
    Q_ASSERT(rrRulerHigh != rrRulerLow);
    OxyCRGTrendWaveWidgetPrivate::reloadWaveBuffer();
    reloadRRwaveBuffer();
}

void OxyCRGRRHRWaveWidgetPrivate::reloadRRwaveBuffer()
{
    if (!rrWaveBuffer)
    {
        return;
    }

    Q_ASSERT(rrRulerHigh != rrRulerLow);

    rrWaveBuffer->clear();
    rrPointGapSumFraction = 0.0;

    int dataSize = rrDataBuf->dataSize();
    int requestDataSize = getIntervalSeconds(interval) * waveDataRate;
    int index = (dataSize > requestDataSize) ? dataSize - requestDataSize : 0;

    for (int i = index; i < dataSize; ++i)
    {
        short value = rrDataBuf->at(i);
        short mapYValue;
        if (value == InvData())
        {
            mapYValue = 0;
        }
        else
        {
            mapYValue = waveRegion.bottom() -
                    (value - rrRulerLow) * waveRegion.height() / (rrRulerHigh - rrRulerLow);
        }

        if (rrPointGapSumFraction > 1.0)
        {
            rrWaveBuffer->pushPointData(mapYValue, true);
            rrPointGapSumFraction -= static_cast<int>(rrPointGapSumFraction);
        }
        else
        {
            rrWaveBuffer->pushPointData(mapYValue, false);
        }

        rrPointGapSumFraction += rrPointGap;
    }
    q_ptr->update();
}

OxyCRGRRHRWaveWidget::OxyCRGRRHRWaveWidget(const QString &waveName)
    : OxyCRGTrendWaveWidget(waveName,
                            new OxyCRGRRHRWaveWidgetPrivate(this))
{
    Q_D(OxyCRGRRHRWaveWidget);
    d->init();
}

OxyCRGRRHRWaveWidget::~OxyCRGRRHRWaveWidget()
{
}

void OxyCRGRRHRWaveWidget::addRrTrendData(int value)
{
    Q_D(OxyCRGRRHRWaveWidget);
    d->rrDataBuf->push(value);
    if (!d->rrWaveBuffer)
    {
        return;
    }
    Q_ASSERT(d->rrRulerHigh != d->rrRulerLow);
    d->rrPointGapSumFraction += d->rrPointGap;

    short mapYValue;
    if (value == InvData())
    {
        mapYValue = 0;
    }
    else
    {
        mapYValue = d->waveRegion.bottom() -
                (value - d->rrRulerLow) * d->waveRegion.height() / (d->rrRulerHigh - d->rrRulerLow);
    }

    if (d->rrPointGapSumFraction > 1.0)
    {
        d->rrWaveBuffer->pushPointData(mapYValue, true);
        d->rrPointGapSumFraction -= static_cast<int>(d->rrPointGapSumFraction);
    }
    else
    {
        d->rrWaveBuffer->pushPointData(mapYValue, false);
    }
}

void OxyCRGRRHRWaveWidget::addHrTrendData(int value)
{
    OxyCRGTrendWaveWidget::addWaveData(value);
}

void OxyCRGRRHRWaveWidget::setRrRulerValue(int valueHigh, int valueLow)
{
    Q_D(OxyCRGRRHRWaveWidget);
    if (d->rrRulerHigh == valueHigh && d->rrRulerLow == valueLow)
    {
        return;
    }

    d->rrRulerHigh = valueHigh;
    d->rrRulerLow = valueLow;
    d->reloadRRwaveBuffer();
}

void OxyCRGRRHRWaveWidget::setHrRulerValue(int valueHigh, int valueLow)
{
    Q_D(OxyCRGRRHRWaveWidget);
    if (d->rulerHigh == valueHigh && d->rulerLow == valueLow)
    {
        return;
    }

    d->rulerHigh = valueHigh;
    d->rulerLow = valueLow;
    d->OxyCRGTrendWaveWidgetPrivate::reloadWaveBuffer();
}

void OxyCRGRRHRWaveWidget::setRrTrendShowStatus(bool isShow)
{
    Q_D(OxyCRGRRHRWaveWidget);
    if (d->isShowRr == isShow)
    {
        return;
    }

    d->isShowRr = isShow;
    update();
}

void OxyCRGRRHRWaveWidget::clearData()
{
    Q_D(OxyCRGRRHRWaveWidget);
    d->rrDataBuf->clear();
    d->reloadRRwaveBuffer();
    OxyCRGTrendWaveWidget::clearData();
}

void OxyCRGRRHRWaveWidget::setInterval(OxyCRGInterval interval)
{
    Q_D(OxyCRGRRHRWaveWidget);
    if (d->interval == interval)
    {
        return;
    }

    OxyCRGTrendWaveWidget::setInterval(interval);
    d->reloadRRwaveBuffer();
}

void OxyCRGRRHRWaveWidget::paintEvent(QPaintEvent *e)
{
    Q_D(OxyCRGRRHRWaveWidget);
    OxyCRGTrendWaveWidget::paintEvent(e);

    QPainter painter(this);

    d->drawWave(&painter, d->waveRegion, d->waveBuffer, d->waveColor);

    if (d->isShowRr)
    {
        int w = rect().width() - WX_SHIFT * 2;
        int h = rect().height();
        // 添加波形名字
        int xShift = X_SHIFT;
        int yShift = Y_SHIFT;
        int wxShift = WX_SHIFT;

        painter.setPen(QPen(d->rrWaveColor, 1, Qt::SolidLine));
        painter.setFont(fontManager.textFont(16));
        painter.drawText(xShift + w + wxShift,
                         yShift,
                         wxShift, h / 3,
                         Qt::AlignLeft | Qt::AlignTop,
                         d->rrWaveName);

        // 添加标尺高低值
        if (d->rrRulerHigh != InvData())
        {
            painter.setPen(QPen(d->rrWaveColor, 1, Qt::SolidLine));
            painter.drawText(w - xShift,
                             yShift,
                             wxShift, wxShift / 2,
                             Qt::AlignTop | Qt::AlignRight,
                             QString::number(d->rrRulerHigh));
        }
        if (d->rrRulerLow != InvData())
        {
            painter.setPen(QPen(d->rrWaveColor, 1, Qt::SolidLine));
            painter.drawText(w - xShift,
                             h - yShift - wxShift / 2,
                             wxShift, wxShift / 2,
                             Qt::AlignBottom | Qt::AlignRight,
                             QString::number(d->rrRulerLow));
        }

        d->drawWave(&painter, d_ptr->waveRegion, d->rrWaveBuffer, d->rrWaveColor);
    }
}
