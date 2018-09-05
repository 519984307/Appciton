/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#include "TableViewItemDelegate_p.h"
#include "ScreenLayoutItemDelegate.h"
#include "ScreenLayoutDefine.h"
#include "SystemManager.h"
#include <QPainter>
#include <QDebug>
#include <QPainterPath>
#include <QTableView>
#include "PopupList.h"

#define WAVE_LEFT_RIGHT_MARGIN 4
#define WAVE_TOP_BOTTOM_MARGIN 2

class ScreenLayoutItemDelegatePrivate : public TableViewItemDelegatePrivate
{
public:
    Q_DECLARE_PUBLIC(ScreenLayoutItemDelegate)

    explicit ScreenLayoutItemDelegatePrivate(ScreenLayoutItemDelegate *const q_ptr)
        : TableViewItemDelegatePrivate(q_ptr)
    {
    }

    void drawWave(QPainter *painter, const QRect &rect, const ScreenLayoutItemInfo &info) const;

    virtual bool showEditor(const QTableView *view, QAbstractItemModel *model, QModelIndex index);
};

void ScreenLayoutItemDelegatePrivate::drawWave(QPainter *painter, const QRect &rect,
        const ScreenLayoutItemInfo &info) const
{
    const unsigned char *data = reinterpret_cast<const unsigned char *>(info.waveContent.constData());
    int len = info.waveContent.length();
    if (len <= 0)
    {
        return;
    }

    QPainterPath path;

    painter->save();
    float xGap = info.drawSpeed / systemManager.getScreenPixelWPitch() /
                 info.sampleRate; // for the x axis distance betwwen each data point
    float x, y;
    int baseYValue = rect.center().y();
    if (info.waveMinValue == info.baseLine)
    {
        baseYValue = rect.bottom() - WAVE_TOP_BOTTOM_MARGIN;
    }

    int index = 0;
    float factor = 1.0 * (rect.height() - 2 * WAVE_TOP_BOTTOM_MARGIN) / (info.waveMaxValue - info.waveMinValue);
    float maxXvalue = rect.right() - WAVE_LEFT_RIGHT_MARGIN;

    // move to start point
    x = WAVE_LEFT_RIGHT_MARGIN;
    y = baseYValue - (data[index] - info.baseLine) * factor;
    path.moveTo(x, y);
    index = (index + 1) % len;

    while (x < maxXvalue)
    {
        x += xGap;
        y = baseYValue - (data[index] - info.baseLine) * factor;
        path.lineTo(x, y);
        index = (index + 1) % len;
    };

    QPen pen = painter->pen();
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path);
    painter->restore();
}

bool ScreenLayoutItemDelegatePrivate::showEditor(const QTableView *view, QAbstractItemModel *model, QModelIndex index)
{
    Q_Q(ScreenLayoutItemDelegate);

    QVariant value = model->data(index, Qt::EditRole);

    if (value.isValid() && value.canConvert<QStringList>())
    {
        QStringList list = qvariant_cast<QStringList>(value);

        QRect vrect = view->visualRect(index);
        QRect rect(view->viewport()->mapToGlobal(vrect.topLeft()),
                   view->viewport()->mapToGlobal(vrect.bottomRight()));

        PopupList *popup = new PopupList();
        popup->setFixedWidth(rect.width());
        popup->additemList(list);
        popup->setMaximumDisplayItem(8);
        QString currentText = model->data(index, Qt::DisplayRole).toString();
        popup->setCurrentText(currentText);
        popup->setPopAroundRect(rect);
        QObject::connect(popup, SIGNAL(selectItemChanged(QString)), q, SLOT(onSelectChanged(QString)));
        QObject::connect(popup, SIGNAL(destroyed(QObject*)), q, SLOT(onPopupDestroy()));
        popup->show();
        return true;
    }

    return false;
}

ScreenLayoutItemDelegate::ScreenLayoutItemDelegate(QObject *parent)
    : TableViewItemDelegate(new ScreenLayoutItemDelegatePrivate(this), parent)
{
}

void ScreenLayoutItemDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect,
        const QString &text) const
{
    Q_UNUSED(rect)
    Q_UNUSED(text)

    Q_D(const ScreenLayoutItemDelegate);
    QVariant val = d->curPaintingIndex.data(Qt::DisplayRole);
    if (val.canConvert<ScreenLayoutItemInfo>())
    {
        ScreenLayoutItemInfo info = qvariant_cast<ScreenLayoutItemInfo>(val);
        if (info.waveid != WAVE_NONE)
        {
            d->drawWave(painter, option.rect, info);
        }
        else
        {
            painter->drawText(option.rect, Qt::AlignCenter, info.name);
        }
    }
}

void ScreenLayoutItemDelegate::onSelectChanged(const QString &text)
{
    if (d_ptr->curEditingIndex.isValid())
    {
        d_ptr->curEditingModel->setData(d_ptr->curEditingIndex, qVariantFromValue(text), Qt::EditRole);
    }
}
