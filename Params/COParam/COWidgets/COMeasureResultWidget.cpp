/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/11
 **/

#include "COMeasureResultWidget.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/TimeDate/TimeDate.h"
#include "FontManager.h"
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDateTime>

#define FOCUS_BORDER_WIDTH 2
#define CHECK_ICON_SIZE QSize(24, 24)
#define CHECK_ICON_COLOR Qt::green

#define MAX_TB_WAVE_VALUE   200     /* in unit of 0.01 celsius degree */
#define MAX_WAVE_DURATION   60      /* seconds, maximum lenght of wave to draw */

class COMeasureResultWidgetPrivate
{
public:
    COMeasureResultWidgetPrivate()
        : checked(false), pressed(false), checkIcon(CHECK_ICON_SIZE, QImage::Format_ARGB32)
    {
        /* create the check icon */
        checkIcon.fill(CHECK_ICON_COLOR);
        QPainter p(&checkIcon);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.drawPixmap(QRect(QPoint(0, 0), CHECK_ICON_SIZE),
                     themeManager.getPixmap(ThemeManager::IconChecked, CHECK_ICON_SIZE));
        p.end();
    }

    /**
     * @brief generateWavePixmap generate teh wave pixmap base on the measure data
     * @param measureData  the measure data
     * @param size the size of pixmap
     * @param color wave color
     */
    void generateWavePixmap(const COMeasureData &measureData, const QSize &size, const QColor &color);

    /**
     * @brief mapYValue map the Y value
     * @param h the h of the region
     * @param maxVal the value at the top of the region
     * @param val value to map
     * @return  the mapped Y pos
     */
    int mapYValue(int h, int maxVal, int val)
    {
        if (val > maxVal)
        {
            return 0;
        }
        if (val <= 0)
        {
            return h - 1;
        }
        return h - 1 - val * h / maxVal;
    }

    bool checked;
    bool pressed;
    QImage checkIcon;
    QPixmap wavePixmap;
    COMeasureData data;
};

COMeasureResultWidget::COMeasureResultWidget(QWidget *parent)
    : QWidget(parent), pimpl(new COMeasureResultWidgetPrivate())
{
    setFocusPolicy(Qt::StrongFocus);

    QPalette pal = this->palette();
    themeManager.setupPalette(ThemeManager::ControlTypeNR, &pal);
    pal.setColor(QPalette::Window, Qt::black);
    pal.setColor(QPalette::Active, QPalette::Window, Qt::darkGray);
    this->setPalette(pal);

    setFont(fontManager.textFont(fontManager.getFontSize(2)));
}

COMeasureResultWidget::~COMeasureResultWidget()
{
}

void COMeasureResultWidget::setChecked(bool check)
{
    if (check ==  pimpl->checked)
    {
        return;
    }

    pimpl->checked = check;
    emit chechedStateChanged(check);
    update();
}

bool COMeasureResultWidget::isChecked() const
{
    return pimpl->checked;
}

void COMeasureResultWidget::setMeasureData(const COMeasureData &data)
{
    pimpl->data = data;
    QSize s = this->size();
    s.setWidth(s.width() - themeManager.getBorderRadius());
    s.setHeight(s.height()  - themeManager.getBorderRadius());
    pimpl->generateWavePixmap(data, s, palette().color(QPalette::WindowText));
    update();
}

void COMeasureResultWidget::resizeEvent(QResizeEvent *ev)
{
    QSize s = ev->size();
    s.setWidth(s.width() - themeManager.getBorderRadius());
    s.setHeight(s.height()  - themeManager.getBorderRadius());
    pimpl->generateWavePixmap(pimpl->data, s, palette().color(QPalette::WindowText));
    update();
}

void COMeasureResultWidget::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPainter p(this);

    const QPalette &pal = palette();

    p.setRenderHint(QPainter::Antialiasing, true);

    QRect bgRecg = this->contentsRect();
    int borderWidth = themeManager.getBorderWidth();
    int borderRadius = themeManager.getBorderRadius();
    bool borderAdjVal = borderWidth / 2;
    if (hasFocus())
    {
        p.setPen(QPen(pal.color(QPalette::Active, QPalette::Highlight), borderWidth));
        p.setBrush(pal.brush(QPalette::Active, QPalette::Window));
        bgRecg.adjust(borderAdjVal, borderAdjVal, -borderAdjVal, -borderAdjVal);
    }
    else if (isEnabled())
    {
        p.setBrush(pal.brush(QPalette::Inactive, QPalette::Window));
        p.setPen(Qt::NoPen);
    }
    else
    {
        p.setBrush(pal.brush(QPalette::Disabled, QPalette::Window));
        p.setPen(Qt::NoPen);
    }

    p.drawRoundedRect(bgRecg, borderRadius, borderRadius);

    /* draw the wave */
    QRect waveRect = QStyle::alignedRect(Qt::LayoutDirectionAuto, Qt::AlignCenter,
                                         pimpl->wavePixmap.size(), this->rect());
    p.drawPixmap(waveRect, pimpl->wavePixmap);

    if (pimpl->data.isValid())
    {
        QFont f = p.font();
        int fontH = fontManager.textHeightInPixels(f);
        p.setPen(Qt::white);
        QString coStr = QString("C.O.: %1").arg(QString::number(pimpl->data.co * 1.0 / 10, 'f', 1));
        QPoint textPos = this->contentsRect().topLeft();
        textPos.rx() += borderRadius;
        textPos.ry() += fontH;
        p.drawText(textPos, coStr);

        textPos.ry() += fontH;
        QString timeStr = timeDate->getTime(pimpl->data.timestamp);
        p.drawText(textPos, timeStr);
    }

    if (pimpl->checked)
    {
        QRect r = this->contentsRect();
        r.adjust(0, borderWidth, -borderWidth, 0);
        QRect iconRect = QStyle::alignedRect(Qt::LayoutDirectionAuto, Qt::AlignTop | Qt::AlignRight,
                                             CHECK_ICON_SIZE, r);
        p.drawImage(iconRect, pimpl->checkIcon);
    }
}

void COMeasureResultWidget::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        pimpl->pressed = true;
        break;
    default:
        QWidget::keyPressEvent(ev);
        break;
    }
}

void COMeasureResultWidget::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (pimpl->pressed)
        {
            pimpl->pressed = false;
            setChecked(!pimpl->checked);
        }
        break;
    default:
        QWidget::keyPressEvent(ev);
        break;
    }
}

void COMeasureResultWidget::mousePressEvent(QMouseEvent *ev)
{
    QWidget::mousePressEvent(ev);
    themeManager.playClickSound();
    if (ev->button() == Qt::LeftButton)
    {
        pimpl->pressed = true;
    }
}

void COMeasureResultWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    QWidget::mouseReleaseEvent(ev);
    if (ev->button() == Qt::LeftButton)
    {
        pimpl->pressed = false;
        setChecked(!pimpl->checked);
    }
}

void COMeasureResultWidgetPrivate::generateWavePixmap(const COMeasureData &measureData,
                                                      const QSize &size, const QColor &color)
{
    wavePixmap = QPixmap(size);
    wavePixmap.fill(Qt::black);

    if (!measureData.isValid())
    {
        return;
    }

    int w = size.width();
    int h = size.height();

    QPainter p(&wavePixmap);
    p.setPen(color);

    /* draw the bottom line */
    p.drawLine(0, h - 1, w - 1, h - 1);

    if (measureData.measureWave.count() == 0)
    {
        return;
    }

    float deltaX = w * 1.0 / (MAX_WAVE_DURATION * measureData.dataRate);
    int lastX = 0;
    int lastY = mapYValue(h, MAX_TB_WAVE_VALUE, measureData.measureWave.at(0));
    for (int i = 0; i < measureData.measureWave.count(); ++i)
    {
        int curX = deltaX * i;
        if (curX >= w)
        {
            break;
        }
        int curY = mapYValue(h, MAX_TB_WAVE_VALUE, measureData.measureWave[i]);
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
            /* draw last X */
            p.drawLine(lastX, h - 1, lastX, lastY);
            lastX = curX;
            lastY = curY;
        }
    }
    /* draw the last line */
    p.drawLine(lastX, h - 1, lastX, lastY);
}

