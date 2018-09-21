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

class OxyCRGRRHRWaveWidgetPrivate
{
public:
    OxyCRGRRHRWaveWidgetPrivate()
        : rrFlagBuf(NULL),
          rrDataBuf(NULL),
          rrDataBufIndex(0),
          rrDataBufLen(0),
          rrName(""),
          rrRulerHigh(InvData()),
          rrRulerLow(InvData()),
          rrWaveColor(Qt::green),
          rrWavePath(NULL),
          curX(0),
          curY(0),
          isReStart(false)
    {
    }
    RingBuff<int> *rrFlagBuf;            // 波形标记缓存， 值为1则表示该数据有误
    RingBuff<int> *rrDataBuf;            // 波形数据缓存
    int rrDataBufIndex;                  // 波形数据缓存下标
    int rrDataBufLen;                    // 波形数据长度

    QString rrName;                      // 波形名称
    int rrRulerHigh;                     // 标尺高值
    int rrRulerLow;                      // 标尺低值

    QColor rrWaveColor;                  // 波形颜色

    QPainterPath *rrWavePath;    // rr波形路径
    QPainterPath hrWavePath;             // hr波形路径

    float curX;
    float curY;
    bool isReStart;
};

OxyCRGRRHRWaveWidget::OxyCRGRRHRWaveWidget(const QString &waveName)
    : OxyCRGTrendWaveWidget(waveName),
      d_ptr(new OxyCRGRRHRWaveWidgetPrivate)
{
    init();
}

OxyCRGRRHRWaveWidget::~OxyCRGRRHRWaveWidget()
{
    delete d_ptr;
}

void OxyCRGRRHRWaveWidget::addRrDataBuf(int value, int flag)
{
    d_ptr->rrDataBuf->push(value);

    d_ptr->rrFlagBuf->push(flag);
}

void OxyCRGRRHRWaveWidget::addHrDataBuf(int value, int flag)
{
    addDataBuf(value, flag);
}

QColor &OxyCRGRRHRWaveWidget::getHrWaveColor()
{
    return getWaveColor();
}

int OxyCRGRRHRWaveWidget::getHrRulerHighValue() const
{
    return getRulerHighValue();
}

int OxyCRGRRHRWaveWidget::getHrRulerLowValue() const
{
    return getRulerLowValue();
}

RingBuff<int> *OxyCRGRRHRWaveWidget::getHrWaveBuf() const
{
    return getWaveBuf();
}

RingBuff<int> *OxyCRGRRHRWaveWidget::getHrFlagBuf() const
{
    return getFlagBuf();
}

void OxyCRGRRHRWaveWidget::paintEvent(QPaintEvent *e)
{
    OxyCRGTrendWaveWidget::paintEvent(e);

    int w = rect().width() - getWxShift() * 2;
    int h = rect().height();
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

    QPainter painter(this);

    // HR
    painter.setPen(QPen(getHrWaveColor(), 2, Qt::SolidLine));
    QPainterPath pathHr;

    double xAllShift = w  / intervalTime;
    double xStep = xAllShift / 1;
    int rulerHigh = getHrRulerHighValue();
    int rulerLow = getRulerLowValue();
    double curX = rect().width() + rect().x() - getWxShift();
    double dataH = (getHrWaveBuf()->at(0) - rulerLow) * 1.0 /
                   (rulerHigh - rulerLow) * h;
    pathHr.moveTo(curX, dataH);
    int dataSize = getHrWaveBuf()->dataSize();
    for (int i = 0; i < dataSize; i++)
    {
        dataH = (getHrWaveBuf()->at(i) - rulerLow) * 1.0 /
                (rulerHigh - rulerLow) * h;
        curX -= xStep;
        if (curX < (rect().x() + getWxShift()))
        {
            break;
        }
        pathHr.lineTo(curX, dataH);
    }
    painter.drawPath(pathHr);

    // rr
    painter.setPen(QPen(d_ptr->rrWaveColor, 2, Qt::SolidLine));
    QPainterPath pathResp;

    xAllShift = w  / intervalTime;
    xStep = xAllShift / 1;
    rulerHigh = d_ptr->rrRulerHigh;
    rulerLow = d_ptr->rrRulerLow;
    curX = rect().width() + rect().x() - getWxShift();
    dataH = (d_ptr->rrDataBuf->at(0) - rulerLow) * 1.0 /
            (rulerHigh - rulerLow) * h;
    pathResp.moveTo(curX, dataH);
    dataSize = d_ptr->rrDataBuf->dataSize();
    for (int i = 0; i < dataSize; i++)
    {
        dataH = (d_ptr->rrDataBuf->at(i) - rulerLow) * 1.0 /
                (rulerHigh - rulerLow) * h;
        curX -= xStep;
        if (curX < (rect().x() + getWxShift()))
        {
            break;
        }
        pathResp.lineTo(curX, dataH);
    }
    painter.drawPath(pathResp);
}

void OxyCRGRRHRWaveWidget::onTimeOutExec()
{
    update();
}

void OxyCRGRRHRWaveWidget::init()
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
    d_ptr->rrWaveColor = color;
    // rr设置标尺值
    int valueLow = 0;
    int valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|RRLow", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|RRHigh", valueHigh);
    QString strValueLow =  OxyCRGSymbol::convert(RRLowTypes(valueLow));
    valueLow = strValueLow.toInt();
    QString strValueHigh =  OxyCRGSymbol::convert(RRHighTypes(valueHigh));
    valueHigh = strValueHigh.toInt();
    d_ptr->rrRulerHigh = valueHigh;
    d_ptr->rrRulerLow = valueLow;
    // 申请存储rr波形数据堆空间
    d_ptr->rrDataBufIndex = 0;
    d_ptr->rrDataBufLen = 65; // 最大8分钟数据  测试1分钟左右的数据
    d_ptr->rrDataBuf = new RingBuff<int>(d_ptr->rrDataBufLen);
    d_ptr->rrFlagBuf = new RingBuff<int>(d_ptr->rrDataBufLen);

    d_ptr->isReStart = true;
    d_ptr->rrWavePath = new QPainterPath;

    // hr标尺的颜色更深。
    palette = colorManager.getPalette(
                  paramInfo.getParamName(PARAM_ECG));
    color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color = palette.color(QPalette::WindowText);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    setWaveColor(color);
    // hr设置标尺值
    valueLow = 0;
    valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|HRLow", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|HRHigh", valueHigh);
    strValueLow =  OxyCRGSymbol::convert(HRLowTypes(valueLow));
    valueLow = strValueLow.toInt();
    strValueHigh =  OxyCRGSymbol::convert(HRHighTypes(valueHigh));
    valueHigh = strValueHigh.toInt();
    setRulerValue(valueHigh, valueLow);
    // 申请存储hr波形数据堆空间
//    int dataLen = ecgParam.getWaveDataRate() * 8 *60;  // 最大8分钟数据
    int dataLen = 65;  // 最大8分钟数据
    setBufSize(dataLen);

    QTimer *timer = new QTimer;
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOutExec()));
    timer->start();
}

