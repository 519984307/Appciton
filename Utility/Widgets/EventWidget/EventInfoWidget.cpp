#include "EventInfoWidget.h"
#include "FontManager.h"
#include <QPainter>

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
EventInfoWidget::EventInfoWidget(QWidget *parent)
    :QWidget(parent)
{
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
}

/**************************************************************************************************
 * 绘图事件。
 *************************************************************************************************/
void EventInfoWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);
    painter.setPen(QPen(Qt::yellow));
    int fontSize = fontManager.getFontSize(4);
    QFont font = fontManager.textFont(fontSize);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignLeft, eventInfo);
    painter.drawText(rect(), Qt::AlignCenter, eventTime);
    painter.drawText(rect(), Qt::AlignRight, eventIndex);
}
