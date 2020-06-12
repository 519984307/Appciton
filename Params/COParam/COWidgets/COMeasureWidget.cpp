/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/11
 **/

#include "COMeasureWidget.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"
#include "FontManager.h"
#include "Framework/Utility/Unit.h"
#include <QPainter>
#include <QDebug>

class COMeasureWidgetPrivate
{
public:
    COMeasureWidgetPrivate()
        : measureTime(0), tiStr(InvStr()), tbStr(InvStr()), coStr(InvStr()), ciStr(InvStr()),
          dataRate(25), ti(InvData()), tb(InvData()), co(InvData()), ci(InvData()),
          startTb(InvData()), maxTbWaveVal(100), maxMeasureDuration(30), isMeasuring(false)
    {}

    /**
     * @brief mapYValue map the y value to y Axis pos
     * @param bottomPos y pos of 0 value
     * @param topPos  y pos of maxTbWaveVal
     * @param val current wave value
     * @return  the mapped y pos
     */
    short mapYValue(short bottomPos, short topPos, short val)
    {
        if (val < 0)
        {
            return bottomPos;
        }

        int yPos = val * (topPos - bottomPos) / maxTbWaveVal + bottomPos;
        return yPos;
    }

    QVector<short> waveData;
    unsigned measureTime;
    QString message;
    QString tiStr;
    QString tbStr;
    QString coStr;
    QString ciStr;
    short dataRate;
    short ti;
    short tb;
    short co;
    short ci;
    short startTb;          /* tb value of start measurement */
    short maxTbWaveVal;     /* 100 for 1 celsius degree, 200 for 2 celsius degree */
    short maxMeasureDuration;   /* defautl 30 seconds,  60 seconds at most */
    bool isMeasuring;
};


COMeasureWidget::COMeasureWidget(QWidget *parent)
    : QWidget(parent), pimpl(new COMeasureWidgetPrivate())
{
    setFont(fontManager.textFont(fontManager.getFontSize(3)));
}

COMeasureWidget::~COMeasureWidget()
{
}

void COMeasureWidget::startMeasure()
{
    pimpl->startTb = pimpl->tb;
    pimpl->maxTbWaveVal = 100;  /* reset to 1 celsius degree range */
    pimpl->waveData.clear();
    pimpl->isMeasuring = true;
    update();
}

void COMeasureWidget::stopMeasure()
{
    pimpl->isMeasuring = false;
}

void COMeasureWidget::addMeasureWave(short wave)
{
    if (!pimpl->isMeasuring)
    {
        return;
    }
    pimpl->waveData.append(wave);
    if (pimpl->waveData.count() / pimpl->dataRate >= 30)
    {
        /* switch to 60 seconds wave duration */
        pimpl->maxMeasureDuration = 60;
    }

    if (wave > pimpl->maxTbWaveVal)
    {
        /* switch to a new scale */
        pimpl->maxTbWaveVal = 200;
    }

    update();
}

void COMeasureWidget::setWaveDataRate(short rate)
{
    if (rate > 0)
    {
        pimpl->dataRate = rate;
    }
}

void COMeasureWidget::setTi(short ti)
{
    if (pimpl->ti != ti)
    {
        pimpl->ti = ti;
        if (ti == InvData())
        {
            pimpl->tiStr = InvStr();
        }
        else
        {
            pimpl->tiStr = QString::number(ti * 1.0 / 10, 'f', 1);
        }
        update();
    }
}

void COMeasureWidget::setTb(short tb)
{
    if (pimpl->tb != tb)
    {
        pimpl->tb = tb;
        if (tb == InvData())
        {
            pimpl->tbStr = InvStr();
        }
        else
        {
            pimpl->tbStr = QString::number(tb * 1.0 / 10, 'f', 1);
        }
        update();
    }
}

void COMeasureWidget::setCo(short co)
{
    if (pimpl->co != co)
    {
        pimpl->co = co;
        if (co == InvData())
        {
            pimpl->coStr = InvStr();
        }
        else
        {
            pimpl->coStr = QString::number(co * 1.0 / 10, 'f', 1);
        }
        update();
    }
}

void COMeasureWidget::setCi(short ci)
{
    if (pimpl->ci != ci)
    {
        pimpl->ci = ci;
        if (ci == InvData())
        {
            pimpl->ciStr = InvStr();
        }
        else
        {
            pimpl->ciStr = QString::number(ci * 1.0 / 10, 'f', 1);
        }
        update();
    }
}

void COMeasureWidget::setMessage(const QString &str)
{
    if (pimpl->message == str)
    {
        return;
    }
    pimpl->message = str;
    update();
}

void COMeasureWidget::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    int borderRadius = themeManager.getBorderRadius();
    QRect contentRect = this->contentsRect();
    p.drawRoundedRect(contentRect, borderRadius, borderRadius);


    const QPalette &pal = palette();
    int waveWidth =  contentRect.width() * 2 / 3 - borderRadius / 2;
    int fontH = fontManager.textHeightInPixels(p.font());
    int topRuler = fontH;
    int bottomRuler = contentRect.height() - fontH * 3 / 2;
    int waveStartX = borderRadius;
    p.setPen(pal.color(QPalette::WindowText));

    QRect topTextArea(waveStartX, 0, waveWidth, topRuler);
    QRect bottomTextArea(waveStartX, bottomRuler, waveWidth, fontH * 3 / 2);
    /* draw the wave label */
    p.drawText(topTextArea, Qt::AlignBottom|Qt::AlignLeft, QString("TB (%1)").arg(trs(Unit::getSymbol(UNIT_TC))));

    /* draw the top ruler */
    p.drawLine(topTextArea.bottomLeft(), topTextArea.bottomRight());

    /* draw the bottom ruler */
    p.drawLine(bottomTextArea.topLeft(), bottomTextArea.topRight());

    if (pimpl->waveData.count())
    {
        /* draw the value of the ruler */
        QString topRulerStr = QString("%1 (%2)")
                .arg(QString::number((pimpl->startTb * 10 - pimpl->maxTbWaveVal) * 1.0 / 100, 'f', 1))
                .arg(trs(Unit::getSymbol(UNIT_TC)));
        p.drawText(topTextArea, Qt::AlignBottom|Qt::AlignRight, topRulerStr);

        QString bottomRulerStr = QString("%1 (%2)")
                .arg(QString::number(pimpl->startTb * 1.0 / 10, 'f', 1))
                .arg(trs(Unit::getSymbol(UNIT_TC)));
        p.drawText(bottomTextArea, Qt::AlignTop|Qt::AlignRight, bottomRulerStr);
    }

    /* draw the message */
    if (!pimpl->message.isEmpty())
    {
        p.drawText(bottomTextArea, Qt::AlignVCenter|Qt::AlignLeft, pimpl->message);
    }

    /* draw the trend */
    QRect trendRegion = contentRect.adjusted(waveStartX + waveWidth + 8, borderRadius + 8,
                                             -borderRadius / 2, -(borderRadius / 2));
    QRect tbRect(trendRegion.topLeft(), trendRegion.center());
    QRect ciRect(trendRegion.center(), trendRegion.bottomRight());
    QRect tiRect(tbRect.topRight(), ciRect.topRight());
    QRect coRect(tbRect.bottomLeft(), ciRect.bottomLeft());
    /* draw caption */
    p.drawText(tbRect, Qt::AlignLeft|Qt::AlignTop, QString("TB (%1)").arg(trs(Unit::getSymbol(UNIT_TC))));
    p.drawText(tiRect, Qt::AlignLeft|Qt::AlignTop, QString("Ti (%1)").arg(trs(Unit::getSymbol(UNIT_TC))));
    p.drawText(coRect, Qt::AlignLeft|Qt::AlignTop, QString("C.O."));
    p.drawText(ciRect, Qt::AlignLeft|Qt::AlignTop, QString("C.I."));
    /* draw value */
    p.setFont(fontManager.numFont(28, true));
    p.drawText(tbRect, Qt::AlignCenter, pimpl->tbStr);
    p.drawText(tiRect, Qt::AlignCenter, pimpl->tiStr);
    p.drawText(coRect, Qt::AlignCenter, pimpl->coStr);
    p.drawText(ciRect, Qt::AlignCenter, pimpl->ciStr);


    /* draw the wave */
    if (pimpl->waveData.count())
    {
        int minYPos = contentRect.top();
        float deltaX = waveWidth * 1.0 / (pimpl->maxMeasureDuration * pimpl->dataRate);
        int lastX = waveStartX;
        int endX = waveStartX + waveWidth;
        int lastY = pimpl->mapYValue(bottomRuler, topRuler, pimpl->waveData.at(0));
        if (lastY < minYPos)
        {
            lastY = minYPos;
        }
        for (int i = 0; i < pimpl->waveData.count(); ++i)
        {
            int curX = deltaX * i + waveStartX;
            if (curX >= endX)
            {
                break;
            }
            int curY = pimpl->mapYValue(bottomRuler, topRuler, pimpl->waveData.at(i));
            if (curY < minYPos)
            {
                curY = minYPos;
            }
            if (curX == lastX)
            {
                /* current point is at the same x pos as the last point, no need to draw */
                if (lastY > curY)
                {
                    lastY = curY;
                }
                continue;
            }
            else
            {
                /* draw last x */
                p.drawLine(lastX, bottomRuler, lastX, lastY);
                lastX = curX;
                lastY = curY;
            }
        }
        /* draw the last line */
        p.drawLine(lastX, bottomRuler, lastX, lastY);
    }
}
