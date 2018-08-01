/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/1
 **/

#include "EventReviewModel.h"
/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/1
 **/

#include "ThemeManager.h"
#include "LanguageManager.h"
#define COLUMN_COUNT            2

#define ROW_HEIGHT_HINT (themeManger.getAcceptableControlHeight())
#define HEADER_HEIGHT_HINT (themeManger.getAcceptableControlHeight())

class EventReviewModelPrivate
{
public:
    EventReviewModelPrivate(){}
public:
    QList<QString> timeList;
    QList<QString> eventList;
};

EventReviewModel::EventReviewModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new EventReviewModelPrivate())
{
}

EventReviewModel::~EventReviewModel()
{
}

int EventReviewModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

int EventReviewModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 0;
}

QVariant EventReviewModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    return QVariant();
}

QVariant EventReviewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::SizeHintRole:
    {
        if (orientation == Qt::Horizontal)
        {
            int w = 800 / (COLUMN_COUNT);
            return QSize(w, HEADER_HEIGHT_HINT);
        }
        break;
    }
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == 0)
            {
                return trs("Time");
            }
            else if (section == 1)
            {
                return trs("Incident");
            }
        }
        break;
    }
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags EventReviewModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    Qt::ItemFlags flags;
    return flags;
}

bool EventReviewModel::eventFilter(QObject *obj, QEvent *ev)
{
    return QObject::eventFilter(obj, ev);
}

void EventReviewModel::updateEvent(QList<QString> &timeList, QList<QString> &eventList)
{
    d_ptr->timeList = timeList;
    d_ptr->eventList = eventList;
}
