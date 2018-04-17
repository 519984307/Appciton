#include "EventInfoWidget.h"

EventInfoWidget::EventInfoWidget(QWidget *parent)
    :QWidget(parent)
{
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, Qt::black);
    this->setPalette(pal);
    setAutoFillBackground(true);
}

EventInfoWidget::~EventInfoWidget()
{

}
