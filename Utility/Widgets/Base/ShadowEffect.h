#pragma once
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEffect>

class ShadowEffect : public QGraphicsEffect
{
    Q_OBJECT
public:
    explicit ShadowEffect(QObject *parent = 0);

    void draw(QPainter* painter);
    QRectF boundingRectFor(const QRectF& rect) const;

    void setDistance(qreal distance);
    qreal distance() const;

    void setBlurRadius(qreal blurRadius);
    qreal blurRadius() const;

    void setColor(const QColor& color);
    QColor color() const;

private:
    qreal  m_distance;
    qreal  m_blurRadius;
    QColor m_color;
};
