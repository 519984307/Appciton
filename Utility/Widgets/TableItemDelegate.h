#pragma once

#include <QStyledItemDelegate>

class TableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TableItemDelegate();

    void setIconAlignment(Qt::Alignment alignment);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    Qt::Alignment _iconAlignment;
};


