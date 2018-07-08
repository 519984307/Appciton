#pragma once
#include <QStyledItemDelegate>

class EventTrendItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum DataRole {
        ValueFontRole = Qt::UserRole + 1,
        TitleFontRole,
        UnitFontRole,
        ValueTextRole,
        TitleTextRole,
        UnitTextRole,
        TrendAlarmRole,
    };

    explicit EventTrendItemDelegate(QObject *parent = NULL);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
