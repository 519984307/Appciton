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
#include "RingBuff.h"
#include <QPainterPath>
#include <QPainter>
#include <QPointer>
#include "ColorManager.h"
#include "ParamInfo.h"
#include "IConfig.h"
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
    OxyCRGRRHRWaveWidgetPrivate()
        : OxyCRGTrendWaveWidgetPrivate(),
          rrDataBuf(NULL),
          rrDataBufIndex(0),
          rrDataBufLen(0),
          rrRulerHigh(InvData()),
          rrRulerLow(InvData()),
          rrWaveColor(Qt::green),
          isShowRr(false),
          rrWaveName("")
    {
    }

    RingBuff<short> *rrDataBuf;          // 波形数据缓存

    int rrDataBufIndex;                  // 波形数据缓存下标

    int rrDataBufLen;                    // 波形数据长度

    int rrRulerHigh;                     // 标尺高值

    int rrRulerLow;                      // 标尺低值

    QColor rrWaveColor;                  // 波形颜色

    bool isShowRr;                       // 是否显示RR趋势波形

    QString rrWaveName;                  // rr波形名称
};


OxyCRGRRHRWaveWidget::OxyCRGRRHRWaveWidget(const QString &waveName)
    : OxyCRGTrendWaveWidget(waveName,
                            new OxyCRGRRHRWaveWidgetPrivate)
{
    init();
}

OxyCRGRRHRWaveWidget::~OxyCRGRRHRWaveWidget()
{
}

void OxyCRGRRHRWaveWidget::addRrTrendData(int value)
{
    Q_D(OxyCRGRRHRWaveWidget);
    d->rrDataBuf->push(value);
}

void OxyCRGRRHRWaveWidget::addHrTrendData(int value)
{
    Q_D(OxyCRGRRHRWaveWidget);
    d->dataBuf->push(value);
}

void OxyCRGRRHRWaveWidget::setRrRulerValue(int valueHigh, int valueLow)
{
    Q_D(OxyCRGRRHRWaveWidget);
    d->rrRulerHigh = valueHigh;
    d->rrRulerLow = valueLow;
}

void OxyCRGRRHRWaveWidget::setHrRulerValue(int valueHigh, int valueLow)
{
    setRulerValue(valueHigh, valueLow);
}

void OxyCRGRRHRWaveWidget::setRrTrendShowStatus(bool isShow)
{
    Q_D(OxyCRGRRHRWaveWidget);
    d->isShowRr = isShow;
}

void OxyCRGRRHRWaveWidget::paintEvent(QPaintEvent *e)
{
    Q_D(OxyCRGRRHRWaveWidget);
    OxyCRGTrendWaveWidget::paintEvent(e);

    int w = rect().width() - WX_SHIFT * 2;
    int h = rect().height();
    QPainter painter(this);

    if (d->isShowRr)
    {
        // 添加波形名字
        int x1 = rect().x();
        int y1 = rect().y();
        int xShift = X_SHIFT;
        int yShift = Y_SHIFT;
        int wxShift = WX_SHIFT;

        painter.setPen(QPen(d->rrWaveColor, 1, Qt::SolidLine));
        painter.setFont(fontManager.textFont(16));
        painter.drawText(x1 + xShift + w + wxShift,
                         y1 + yShift,
                         wxShift, h / 3,
                         Qt::AlignLeft | Qt::AlignTop,
                         d->rrWaveName);

        // 添加标尺高低值
        if (d->rrRulerHigh != InvData())
        {
            painter.setPen(QPen(d->rrWaveColor, 1, Qt::SolidLine));
            painter.drawText(x1 + w - xShift,
                             y1 + yShift,
                             wxShift, wxShift / 2,
                             Qt::AlignTop | Qt::AlignRight,
                             QString::number(d->rrRulerHigh));
        }
        if (d->rrRulerLow != InvData())
        {
            painter.setPen(QPen(d->rrWaveColor, 1, Qt::SolidLine));
            painter.drawText(x1 + w - xShift,
                             y1 + h - yShift - wxShift / 2,
                             wxShift, wxShift / 2,
                             Qt::AlignBottom | Qt::AlignRight,
                             QString::number(d->rrRulerLow));
        }
    }

    // 数据速率
    int dataRate = d->waveDataRate;
    OxyCRGInterval interval = getIntervalTime();
    int intervalTime = 1 * 60;

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


    // HR
    painter.setPen(QPen(d->waveColor, 2, Qt::SolidLine));
    QPainterPath pathHr;


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
    pathHr.moveTo(curX, dataH);
    for (int i = index; i < dataSize; i++)
    {
        dataH = h - (d->dataBuf->at(i) - rulerLow) * 1.0 /
                (rulerHigh - rulerLow) * h;
        curX += xStep;
        if (curX > (rect().width() + rect().x() - WX_SHIFT))
        {
            curX = rect().width() + rect().x() - WX_SHIFT;
            pathHr.lineTo(curX, dataH);
            break;
        }

        pathHr.lineTo(curX, dataH);
    }
    painter.drawPath(pathHr);

    // RR
    if (!d->isShowRr)
    {
        return;
    }

    painter.setPen(QPen(d->rrWaveColor, 2, Qt::SolidLine));
    QPainterPath pathResp;

    xAllShift = w * 1.0 / intervalTime;
    xStep = xAllShift / dataRate;
    rulerHigh = d->rrRulerHigh;
    rulerLow = d->rrRulerLow;
    dataSize = d->rrDataBuf->dataSize();
    index = (dataSize - dataCount > 0) ? (dataSize - dataCount) : (0);

    curX = rect().width()
           + rect().x()
           - WX_SHIFT
           - dataSize * xStep;
    if (curX < rect().x() + WX_SHIFT)
    {
        curX = rect().x() + WX_SHIFT;
    }
    dataH = h - (d->rrDataBuf->at(index) - rulerLow) * 1.0 /
            (rulerHigh - rulerLow) * h;
    pathResp.moveTo(curX, dataH);
    for (int i = index; i < dataSize; i++)
    {
        dataH = h - (d->rrDataBuf->at(i) - rulerLow) * 1.0 /
                (rulerHigh - rulerLow) * h;

        curX += xStep;
        if (curX > (rect().width() + rect().x() - WX_SHIFT))
        {
            curX = rect().width() + rect().x() - WX_SHIFT;
            pathResp.lineTo(curX, dataH);
            break;
        }

        pathResp.lineTo(curX, dataH);
    }
    painter.drawPath(pathResp);
}

void OxyCRGRRHRWaveWidget::showEvent(QShowEvent *e)
{
    Q_D(OxyCRGRRHRWaveWidget);
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

    if (d->rrDataBuf)
    {
        d->rrDataBuf->clear();
    }
}

void OxyCRGRRHRWaveWidget::hideEvent(QHideEvent *e)
{
    Q_D(OxyCRGRRHRWaveWidget);
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

    if (d->rrDataBuf)
    {
        d->rrDataBuf->clear();
    }
}

void OxyCRGRRHRWaveWidget::init()
{
    Q_D(OxyCRGRRHRWaveWidget);

    // rr标尺的颜色更深。
    QPalette palette = colorManager.getPalette(
                           paramInfo.getParamName(PARAM_RESP));
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    d->rrWaveColor = color;
    d->rrWaveName = "RR";
    // rr设置标尺值
    int valueLow = 0;
    int valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|RRLow", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|RRHigh", valueHigh);
    QString strValueLow =  OxyCRGSymbol::convert(RRLowTypes(valueLow));
    valueLow = strValueLow.toInt();
    QString strValueHigh =  OxyCRGSymbol::convert(RRHighTypes(valueHigh));
    valueHigh = strValueHigh.toInt();
    d->rrRulerHigh = valueHigh;
    d->rrRulerLow = valueLow;
    // 申请存储rr波形数据堆空间
    d->rrDataBufIndex = 0;
    d->rrDataBufLen = d->waveDataRate * 8 * 60; // 最大8分钟数据
    d->rrDataBuf = new RingBuff<short>(d->rrDataBufLen);

    // hr标尺的颜色更深。
    palette = colorManager.getPalette(
                  paramInfo.getParamName(PARAM_ECG));
    color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    d->waveColor = color;
    // hr设置标尺值
    valueLow = 0;
    valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|HRLow", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|HRHigh", valueHigh);
    strValueLow =  OxyCRGSymbol::convert(HRLowTypes(valueLow));
    valueLow = strValueLow.toInt();
    strValueHigh =  OxyCRGSymbol::convert(HRHighTypes(valueHigh));
    valueHigh = strValueHigh.toInt();
    d->rulerHigh = valueHigh;
    d->rulerLow = valueLow;
    // 申请存储hr波形数据堆空间
    int dataLen = d->waveDataRate * MAX_WAVE_DURATION * 60;  // 最大8分钟数据
    d->dataBuf = new RingBuff<short>(dataLen);
    d->name = "HR_PR";

    int index = 0;
    currentConfig.getNumValue("OxyCRG|Trend1", index);
    if (index == 0)
    {
        d->isShowRr = false;
    }
    else
    {
        d->isShowRr = true;
    }
}
