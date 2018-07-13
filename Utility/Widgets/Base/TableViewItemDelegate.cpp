/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/11
 **/

#include "TableViewItemDelegate.h"
#include <QDebug>
#include "PopupList.h"
#include <QPainter>
#include "ThemeManager.h"
#include <QKeyEvent>
#include <QMouseEvent>

#define MARGIN 2

class TableViewItemDelegatePrivate
{
public:
    const QWidget *widget(const QStyleOptionViewItem &option) const
    {
        if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
        {
            return v3->widget;
        }

        return 0;
    }

    QModelIndex curIndex;   // record current item's index
    Qt::CheckState checkState;  // record current item's check state
    QPalette pal;   // palette to used when draw check state
};

TableViewItemDelegate::TableViewItemDelegate(QObject *parent)
    : QItemDelegate(parent), d_ptr(new TableViewItemDelegatePrivate())
{
    themeManger.setupPalette(ThemeManager::ControlComboBox, d_ptr->pal);
}

TableViewItemDelegate::~TableViewItemDelegate()
{
    delete d_ptr;
}

QWidget *TableViewItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    qDebug() << Q_FUNC_INFO << index.row();
    return QItemDelegate::createEditor(parent, option, index);
}

void TableViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // get the model index
    d_ptr->curIndex = index;
    QItemDelegate::paint(painter, option, index);
}

/**
 * In the QItemDelegate::paint, the api is call int the folllowing order:
 *   drawBackground (non virtual)
 *   drawCheck (virtual)
 *   drawDecoraion (virtual)
 *   drawFocus (virtual)
 *
 * when the item is editable, we use the drawcheck to draw the background and border. when the item is editable,
 * the model should also provider the Qt::Checkablerole
 */
void TableViewItemDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect,
                                      Qt::CheckState state) const
{
    d_ptr->checkState = state;
    if (!rect.isValid() || !(option.state & QStyle::State_Enabled))
    {
        // when uncheckable, the rect will be zero
        return;
    }

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

    // draw the item background color
    QVariant value = d_ptr->curIndex.data(Qt::BackgroundRole);
    if (value.canConvert<QBrush>())
    {
        QPointF oldBO = painter->brushOrigin();
        painter->setBrushOrigin(option.rect.topLeft());
        painter->fillRect(option.rect, qvariant_cast<QBrush>(value));
        painter->setBrushOrigin(oldBO);
    }

    // draw the background and border
    QColor borderColor;
    QColor bgColor;
    if (option.state & QStyle::State_HasFocus)
    {
        borderColor = d_ptr->pal.color(QPalette::Active, QPalette::Shadow);
        bgColor = d_ptr->pal.color(QPalette::Inactive, QPalette::Window);
    }
    else
    {
        borderColor = d_ptr->pal.color(QPalette::Inactive, QPalette::Shadow);
        bgColor = d_ptr->pal.color(QPalette::Inactive, QPalette::Window);
    }

    int borderWidth = themeManger.getBorderWidth();
    int radius = themeManger.getBorderRadius();
    QRect r = option.rect.adjusted(MARGIN, 0, -MARGIN, 0);
    r.adjust(borderWidth / 2, borderWidth / 2, -borderWidth / 2, -borderWidth / 2);
    QPen pen(borderColor, borderWidth);
    painter->setPen(pen);
    painter->setBrush(bgColor);
    painter->drawRoundedRect(r, radius, radius);
    painter->restore();
}

void TableViewItemDelegate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(rect)
}

void TableViewItemDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect,
                                        const QString &text) const
{
    if (d_ptr->checkState == Qt::Unchecked)
    {
        QItemDelegate::drawDisplay(painter, option, rect, text);
    }
    else
    {
        QStyleOptionViewItem opt(option);
        opt.state &= (~QStyle::State_Selected);
        QRect r = QItemDelegate::rect(opt, d_ptr->curIndex, Qt::DisplayRole);
        r = QStyle::alignedRect(option.direction, option.displayAlignment, r.size().boundedTo(option.rect.size()), option.rect);
        QItemDelegate::drawDisplay(painter, opt, r, text);
    }
}

bool TableViewItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                        const QModelIndex &index)
{

    Q_ASSERT(event);
    Q_ASSERT(model);

    // make sure that the item is editable
    Qt::ItemFlags flags = model->flags(index);
    if (!(option.state & QStyle::State_Enabled)
            || !(flags & Qt::ItemIsEditable)
            || !(flags & Qt::ItemIsEnabled))
    {
        return false;
    }

    // make sure we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
    {
        return false;
    }

    const QWidget *w = d_ptr->widget(option);

    // make sure that we have the right event type
    if ((event->type() == QEvent::MouseButtonRelease)
            || (event->type() == QEvent::MouseButtonDblClick)
            || (event->type() == QEvent::MouseButtonPress))
    {
        QMouseEvent *mv = static_cast<QMouseEvent *>(event);
        PopupList *popup = new PopupList();
        popup->addItemText("ON");
        popup->addItemText("OFF");
        popup->move(w->mapToGlobal(mv->pos()));
        popup->show();
    }
    else if (event->type() == QEvent::KeyPress)
    {
    }
    else
    {
        return false;
    }

    return false;
}
