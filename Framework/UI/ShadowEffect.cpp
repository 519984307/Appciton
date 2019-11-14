/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/13
 **/

#include "ShadowEffect.h"
#include <QPainter>

ShadowEffect::ShadowEffect(QObject *parent) :
    QGraphicsEffect(parent),
    m_distance(4.0f),
    m_blurRadius(10.0f),
    m_color(0, 0, 0, 80)
{
}

QT_BEGIN_NAMESPACE
extern Q_GUI_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius,     // NOLINT
                                      bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

void ShadowEffect::draw(QPainter* painter)
{
    // if nothing to show outside the item, just draw source
    if ((m_blurRadius + m_distance) <= 0) {
        drawSource(painter);
        return;
    }

    PixmapPadMode mode = QGraphicsEffect::PadToEffectiveBoundingRect;
    QPoint offset;
    const QPixmap px = sourcePixmap(Qt::DeviceCoordinates, &offset, mode);

    // return if no source
    if (px.isNull())
        return;

    // save world transform
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());

    // Calculate size for the background image
    QSize szi(px.size().width() + 2 * m_distance, px.size().height() + 2 * m_distance);

    QImage tmp(szi, QImage::Format_ARGB32_Premultiplied);
    QPixmap scaled = px.scaled(szi);
    tmp.fill(0);
    QPainter tmpPainter(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmpPainter.drawPixmap(QPointF(-m_distance, -m_distance), scaled);
    tmpPainter.end();

    // blur the alpha channel
    QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
    blurred.fill(0);
    QPainter blurPainter(&blurred);
    qt_blurImage(&blurPainter, tmp, m_blurRadius, false, true);
    blurPainter.end();

    tmp = blurred;

    // blacken the image...
    tmpPainter.begin(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tmpPainter.fillRect(tmp.rect(), color());
    tmpPainter.end();

    // draw the blurred shadow...
    painter->drawImage(offset, tmp);

    // draw the actual pixmap...
    painter->drawPixmap(offset, px, QRectF());

    // restore world transform
    painter->setWorldTransform(restoreTransform);
}

QRectF ShadowEffect::boundingRectFor(const QRectF& rect) const
{
    qreal delta = blurRadius() + distance();
    return rect.united(rect.adjusted(-delta, -delta, delta, delta));
}

void ShadowEffect::setDistance(qreal distance)
{
    m_distance = distance;
    updateBoundingRect();
}

qreal ShadowEffect::distance() const {
    return m_distance;
}

void ShadowEffect::setBlurRadius(qreal blurRadius)
{
    m_blurRadius = blurRadius;
    updateBoundingRect();
}

qreal ShadowEffect::blurRadius() const
{
    return m_blurRadius;
}

void ShadowEffect::setColor(const QColor &color) {
    m_color = color;
}

QColor ShadowEffect::color() const
{
    return m_color;
}
