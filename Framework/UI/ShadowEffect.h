/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/13
 **/
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
