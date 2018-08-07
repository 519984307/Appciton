/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/1
 **/

#include "EventInfoWidget.h"
#include "FontManager.h"
#include <QPainter>

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
EventInfoWidget::EventInfoWidget(QWidget *parent)
    :QWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, Qt::black);
    this->setPalette(pal);
    setAutoFillBackground(true);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
EventInfoWidget::~EventInfoWidget()
{
}

void EventInfoWidget::loadDataInfo(QString info, QString time, QString index)
{
    eventInfo = info;
    eventTime = time;
    eventIndex = index;
    update();
}

/**************************************************************************************************
 * 绘图事件。
 *************************************************************************************************/
void EventInfoWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);
    painter.setPen(QPen(Qt::gray));
    painter.drawLine(rect().bottomLeft(), rect().bottomRight());
    painter.setPen(QPen(Qt::yellow));
    int fontSize = fontManager.getFontSize(4);
    QFont font = fontManager.textFont(fontSize);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, eventInfo);
    painter.drawText(rect(), Qt::AlignCenter, eventTime);
    painter.drawText(rect(), Qt::AlignRight | Qt::AlignVCenter, eventIndex);
}
