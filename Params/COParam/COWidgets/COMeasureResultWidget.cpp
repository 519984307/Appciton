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
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>

#define FOCUS_BORDER_WIDTH 2
#define CHECK_ICON_SIZE QSize(24, 24)
#define CHECK_ICON_COLOR Qt::green

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

    bool checked;
    bool pressed;
    QImage checkIcon;
    QPixmap wavePixmap;
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

void COMeasureResultWidget::resizeEvent(QResizeEvent *ev)
{
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
