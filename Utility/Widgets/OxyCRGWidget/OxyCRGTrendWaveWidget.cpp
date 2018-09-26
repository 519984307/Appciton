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
#include "WaveWidgetLabel.h"
#include <QTimer>
#include <QScopedPointer>

OxyCRGTrendWaveWidget::OxyCRGTrendWaveWidget(const QString &waveName,
                                             OxyCRGTrendWaveWidgetPrivate *p)
                     : IWidget(waveName),
                       d_ptr(p)
{
   setFocusPolicy(Qt::NoFocus);

    QTimer *timer = new QTimer;
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOutExec()));
    d_ptr->timer = timer;
}

OxyCRGTrendWaveWidget::~OxyCRGTrendWaveWidget()
{
}

void OxyCRGTrendWaveWidget::addDataBuf(int value, int flag)
{
    d_ptr->dataBuf->push(value);

    d_ptr->flagBuf->push(flag);
}

void OxyCRGTrendWaveWidget::setRulerValue(int valueHigh, int valueLow)
{
    d_ptr->rulerHigh = valueHigh;
    d_ptr->rulerLow = valueLow;
}

OxyCRGInterval OxyCRGTrendWaveWidget::getIntervalTime()
{
    int index = OxyCRG_Interval_1;
    currentConfig.getNumValue("OxyCRG|Interval", index);

    return (OxyCRGInterval)index;
}

void OxyCRGTrendWaveWidget::setClearWaveDataStatus(bool clearStatus)
{
    d_ptr->isClearWaveData = clearStatus;
}

void OxyCRGTrendWaveWidget::setDataRate(int rate)
{
    d_ptr->waveDataRate = rate;
}

void OxyCRGTrendWaveWidget::onTimeOutExec()
{
    update();
}


void OxyCRGTrendWaveWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    int x1 = rect().x();
    int y1 = rect().y();
    int w = rect().width();
    int h = rect().height();
    int xShift = X_SHIFT;
    int yShift = Y_SHIFT;
    int wxShift = WX_SHIFT;

    QPainterPath pathRect;
    QPainter painter(this);
    // 添加边框
    pathRect.addRect(x1, y1, w, h);
    painter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    painter.drawPath(pathRect);

    // 添加波形名字
    painter.setPen(QPen(d_ptr->waveColor, 1, Qt::SolidLine));
    painter.setFont(fontManager.textFont(16));
    painter.drawText(x1 + xShift, y1 + yShift,
                     wxShift, h / 3,
                     Qt::AlignLeft|Qt::AlignTop,
                     d_ptr->name);

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

void OxyCRGTrendWaveWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);
    if (d_ptr->timer)
    {
        d_ptr->timer->start();
    }
}

void OxyCRGTrendWaveWidget::hideEvent(QHideEvent *e)
{
    IWidget::hideEvent(e);
    if (d_ptr->timer)
    {
        d_ptr->timer->stop();
    }
}
