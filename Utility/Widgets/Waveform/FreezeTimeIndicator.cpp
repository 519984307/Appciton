#include "FreezeTimeIndicator.h"
#include <QPainter>
#include "FontManager.h"

FreezeTimeIndicator::FreezeTimeIndicator(WaveWidget *w)
    : WaveWidgetItem(w, true),
      _reviewTime(0)
{
    setFont(fontManager.textFont(14));
}

#define ARROW_HEIGHT 16
#define ARROW_WIDTH 12
#define TEXT_WIDTH 80   // should be width enough
void FreezeTimeIndicator::paintItem(QPainter &painter)
{
    if(!geometry().isValid())
    {
        return;
    }

    painter.save();
    QPen p(palette().color(QPalette::WindowText));
    painter.setPen(p);
    painter.setFont(font());
    painter.translate(geometry().bottomRight());
    painter.setRenderHint(QPainter::Antialiasing);

    int middle = - ARROW_WIDTH / 2;
    int fontH = fontManager.textHeightInPixels(font());
    int top = - (ARROW_HEIGHT + fontH);

    QPainterPath path;
    path.moveTo(middle, top);
    path.lineTo(middle - ARROW_WIDTH / 2, top + ARROW_HEIGHT / 2);
    path.lineTo(middle - ARROW_WIDTH / 4, top + ARROW_HEIGHT / 2);
    path.lineTo(middle - ARROW_WIDTH / 4, top + ARROW_HEIGHT);
    path.lineTo(middle + ARROW_WIDTH / 4, top + ARROW_HEIGHT);
    path.lineTo(middle + ARROW_WIDTH / 4, top + ARROW_HEIGHT / 2);
    path.lineTo(middle + ARROW_WIDTH / 2, top + ARROW_HEIGHT / 2);
    path.lineTo(middle, top);

    painter.fillPath(path, palette().windowText());

    QRect rect;
    rect.setTopLeft(QPoint(-TEXT_WIDTH, -fontH));
    rect.setWidth(TEXT_WIDTH);
    rect.setHeight(fontH);
    QString t = QString("-%1s").arg(_reviewTime);
    painter.drawText(rect, Qt::AlignRight|Qt::AlignVCenter, t);

    //draw a up arrow
    painter.restore();
}

void FreezeTimeIndicator::setReviewTime(int t)
{
    if(t == _reviewTime)
    {
        return;
    }

    _reviewTime = t;

    if(isVisible())
    {
        update();
    }
}
