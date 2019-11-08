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
#include "Framework/Language/LanguageManager.h"
#include "WindowManager.h"
#define COLUMN_COUNT            2

#define ROW_HEIGHT_HINT (themeManger.getAcceptableControlHeight())
#define HEADER_HEIGHT_HINT (themeManger.getAcceptableControlHeight())

class EventReviewModelPrivate
{
public:
    EventReviewModelPrivate()
        : eachPageRowCount(0)
    {}
public:
    QList<QString> timeList;
    QList<QString> eventList;
    int eachPageRowCount;

    /**
     * @brief calTotalPage 计算总页数
     * @return
     */
    int calTotalPage();
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
    int page = d_ptr->calTotalPage();
    if (page == 0)
    {
        // 没有数据时也仍然占一页
        page = 1;
    }
    return page * d_ptr->eachPageRowCount;
}

QVariant EventReviewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || d_ptr->timeList.count() == 0 ||
            d_ptr->eventList.count() == 0)
    {
        if (role == Qt::DisplayRole)
        {
            return QVariant();
        }
    }
    int row = index.row();
    int column = index.column();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        if (column == 0 && row < d_ptr->timeList.count())
        {
            return d_ptr->timeList.at(row);
        }
        else if (column == 1 && row < d_ptr->eventList.count())
        {
            return d_ptr->eventList.at(row);
        }
        break;
    }
    case Qt::SizeHintRole:
    {
        int w = windowManager.getPopWindowWidth() / COLUMN_COUNT;
        return QSize(w, HEADER_HEIGHT_HINT);
    }
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::BackgroundRole:
        if (row % 2)
        {
            return themeManger.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                        ThemeManager::StateDisabled);
        }
        else
        {
            return themeManger.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                        ThemeManager::StateActive);
        }
        break;
    case Qt::ForegroundRole:
        return QBrush(QColor("#2C405A"));
        break;
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
            int w = windowManager.getPopWindowWidth() / (COLUMN_COUNT);
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
    case Qt::ForegroundRole:
        return QBrush(QColor("#2C405A"));
        break;
    case Qt::BackgroundColorRole:
        return themeManger.getColor(ThemeManager::ControlTypeNR,
                                    ThemeManager::ElementBackgound,
                                    ThemeManager::StateDisabled);
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags EventReviewModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;
    if (index.row() < d_ptr->timeList.count())
    {
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
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

void EventReviewModel::setPageRowCount(int count)
{
    d_ptr->eachPageRowCount = count;
}

int EventReviewModel::getHeightHint() const
{
    return ROW_HEIGHT_HINT;
}

int EventReviewModelPrivate::calTotalPage()
{
    if (!eachPageRowCount)
    {
        return 0;
    }

    int page = timeList.count() / eachPageRowCount;
    if (timeList.count() % eachPageRowCount)
    {
        page += 1;
    }
    return page;
}
