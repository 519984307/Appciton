/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/19
 **/

#include "OxyCRGTrendWaveWidget.h"
#include <QPainter>
#include <QPainterPath>
#include "BaseDefine.h"
#include "FontManager.h"
#include "ThemeManager.h"
#include "IConfig.h"

class OxyCRGTrendWaveWidgetPrivate
{
public:
    OxyCRGTrendWaveWidgetPrivate()
                : flagBuf(NULL),
                  dataBuf(NULL),
                  dataBufIndex(0),
                  dataBufLen(0),
                  name(""),
                  rulerHigh(InvData()),
                  rulerLow(InvData()),
                  waveColor(Qt::green)
    {
    }
    RingBuff<int> *flagBuf;      // 波形标记缓存， 值为1则表示该数据有误
    RingBuff<int> *dataBuf;      // 波形数据缓存
    int dataBufIndex;            // 波形数据缓存下标
    int dataBufLen;              // 波形数据长度

    QString name;                // 波形名称
    int rulerHigh;               // 标尺高值
    int rulerLow;                // 标尺低值

    QColor waveColor;            // 波形颜色

    static const int xShift = 2;
    static const int yShift = 2;
    static const int wxShift = 50;
};


OxyCRGTrendWaveWidget::OxyCRGTrendWaveWidget(const QString &waveName)
                     : IWidget(waveName),
                       d_ptr(new OxyCRGTrendWaveWidgetPrivate)
{
   setFocusPolicy(Qt::NoFocus);
}

OxyCRGTrendWaveWidget::~OxyCRGTrendWaveWidget()
{
    delete d_ptr;
}

void OxyCRGTrendWaveWidget::addDataBuf(int value, int flag)
{
    d_ptr->dataBuf->push(value);

    d_ptr->flagBuf->push(flag);
}

int OxyCRGTrendWaveWidget::getXShift() const
{
    return d_ptr->xShift;
}

int OxyCRGTrendWaveWidget::getYShift() const
{
    return d_ptr->yShift;
}

int OxyCRGTrendWaveWidget::getWxShift() const
{
    return d_ptr->wxShift;
}

int OxyCRGTrendWaveWidget::getBufLen() const
{
    return d_ptr->dataBufLen;
}

int OxyCRGTrendWaveWidget::getBufIndex() const
{
    return d_ptr->dataBufIndex;
}

QColor &OxyCRGTrendWaveWidget::getWaveColor() const
{
    return d_ptr->waveColor;
}

int OxyCRGTrendWaveWidget::getRulerHighValue() const
{
    return d_ptr->rulerHigh;
}

int OxyCRGTrendWaveWidget::getRulerLowValue() const
{
    return d_ptr->rulerLow;
}

RingBuff<int> *OxyCRGTrendWaveWidget::getWaveBuf() const
{
    return d_ptr->dataBuf;
}

RingBuff<int> *OxyCRGTrendWaveWidget::getFlagBuf() const
{
    return d_ptr->flagBuf;
}

void OxyCRGTrendWaveWidget::setWaveColor(const QColor &color)
{
    d_ptr->waveColor = color;
}

void OxyCRGTrendWaveWidget::setRulerValue(int valueHigh, int valueLow)
{
    d_ptr->rulerHigh = valueHigh;
    d_ptr->rulerLow = valueLow;
}

void OxyCRGTrendWaveWidget::setBufSize(int bufSize)
{
    if (d_ptr->flagBuf)
    {
        delete d_ptr->flagBuf;
        d_ptr->flagBuf = NULL;
    }
    if (d_ptr->dataBuf)
    {
        delete d_ptr->dataBuf;
        d_ptr->dataBuf = NULL;
    }
    d_ptr->flagBuf = new RingBuff<int>(bufSize);
    d_ptr->dataBuf = new RingBuff<int>(bufSize);
    d_ptr->dataBufIndex = 0;
    d_ptr->dataBufLen = bufSize;
}

OxyCRGInterval OxyCRGTrendWaveWidget::getIntervalTime()
{
    int index = OxyCRG_Interval_1;
    currentConfig.getNumValue("RESP|Interval", index);

    return (OxyCRGInterval)index;
}


void OxyCRGTrendWaveWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    int x1 = rect().x();
    int y1 = rect().y();
    int w = rect().width();
    int h = rect().height();
    int xShift = d_ptr->xShift;
    int yShift = d_ptr->yShift;
    int wxShift = d_ptr->wxShift;

    QPainterPath pathRect;
    QPainter painter(this);
    // 添加边框
    pathRect.addRect(x1, y1, w, h);
    painter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    painter.drawPath(pathRect);

    // 添加标尺高低值
    if (d_ptr->rulerHigh != InvData())
    {
        painter.setPen(QPen(d_ptr->waveColor, 1, Qt::SolidLine));
        painter.drawText(x1 + wxShift + xShift, y1 + yShift,
                         wxShift, wxShift / 2,
                         Qt::AlignTop | Qt::AlignLeft,
                         QString::number(d_ptr->rulerHigh));
    }
    if (d_ptr->rulerLow != InvData())
    {
        painter.setPen(QPen(d_ptr->waveColor, 1, Qt::SolidLine));
        painter.drawText(x1 + wxShift + xShift, y1 + h - yShift - wxShift / 2,
                         wxShift, wxShift / 2,
                         Qt::AlignBottom | Qt::AlignLeft,
                         QString::number(d_ptr->rulerLow));
    }


    QPainterPath pathBackRuler;
    // 添加背景标尺虚线
    int xStep = qRound((w - wxShift * 2) * 1.0 / 4);
    for (int i = 1; i < 4; i++)
    {
        pathBackRuler.moveTo(x1 + wxShift + xStep * i, y1);
        pathBackRuler.lineTo(x1 + wxShift + xStep * i, y1 + h);
    }
    int yStep = qRound(h * 1.0 / 3);
    for (int i = 1; i < 3; i++)
    {
        pathBackRuler.moveTo(x1 + wxShift, y1 + yStep * i);
        pathBackRuler.lineTo(x1 + w - wxShift, y1 + yStep * i);
    }
    painter.setPen(QPen(Qt::white, 1, Qt::DotLine));
    painter.drawPath(pathBackRuler);
}









