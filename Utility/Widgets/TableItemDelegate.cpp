#include "TableItemDelegate.h"
#include <QPainter>
#include <QStyle>
#include <QIcon>
#include <QApplication>

#define MARGIN 4

TableItemDelegate::TableItemDelegate()
    :QStyledItemDelegate(),
      _iconAlignment(Qt::AlignRight|Qt::AlignVCenter)
{

}

void TableItemDelegate::setIconAlignment(Qt::Alignment alignment)
{
    _iconAlignment = alignment;
}

void TableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    if(!icon.isNull())
    {
        painter->save();
        painter->setClipRect(option.rect);

        QRect pixmapRect = QRect(QPoint(0,0), option.decorationSize);
        QRect rect = option.rect.adjusted(MARGIN, 0, -MARGIN, 0);
        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));

        //draw background
        painter->fillRect(option.rect, qvariant_cast<QColor>(index.data(Qt::BackgroundColorRole)));

        //draw text
        QPen pen(qvariant_cast<QColor>(index.data(Qt::TextColorRole)));
        painter->setPen(pen);
        painter->drawText(rect, index.data(Qt::TextAlignmentRole).toInt(), index.data().toString());

        //draw icon
        if(pixmapRect.isValid())
        {
             QRect iconRect = QStyle::alignedRect(Qt::LayoutDirectionAuto, _iconAlignment, option.decorationSize, rect);
             icon.paint(painter,iconRect);
        }
        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
