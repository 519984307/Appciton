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
    return d_ptr->timeList.count();
}

QVariant EventReviewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || d_ptr->timeList.count() == 0 ||
            d_ptr->eventList.count() == 0)
    {
        return QVariant();
    }
    int row = index.row();
    int column = index.column();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        if (column == 0)
        {
            return d_ptr->timeList.at(row);
        }
        else if (column == 1)
        {
            return d_ptr->eventList.at(row);
        }
        break;
    }
    case Qt::SizeHintRole:
    {
        int w = 800 / COLUMN_COUNT;
        return QSize(w, HEADER_HEIGHT_HINT);
    }
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    default:
        break;
    }

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
    flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

bool EventReviewModel::eventFilter(QObject *obj, QEvent *ev)
{
    return QObject::eventFilter(obj, ev);
}

void EventReviewModel::updateEvent(QList<QString> &timeList, QList<QString> &eventList)
{
    beginResetModel();
    d_ptr->timeList = timeList;
    d_ptr->eventList = eventList;
    endResetModel();
}
