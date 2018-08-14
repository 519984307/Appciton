/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/9
 **/

#include "ErrorLogTableModel.h"
#include "ThemeManager.h"
#include "LanguageManager.h"
#include "ErrorLogItem.h"
#include "ErrorLog.h"

#define COLUMN_COUNT        2
#define MAX_ROW_COUNT       7

#define ROW_HEIGHT_HINT (themeManger.getAcceptableControlHeight())
#define HEADER_HEIGHT_HINT (themeManger.getAcceptableControlHeight())

class ErrorLogTableModelPrivate
{
public:
    ErrorLogTableModelPrivate()
        : currentPage(0), totalPage(0)
    {}

public:
    QList<QString> timeList;
    QList<QString> infoList;
    QList<QColor> colorList;
    QList<int> dataIndex;     // 当前选中事件项对应的数据所在索引

    int currentPage;
    int totalPage;
};

ErrorLogTableModel::ErrorLogTableModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new ErrorLogTableModelPrivate())
{
}

ErrorLogTableModel::~ErrorLogTableModel()
{
}

int ErrorLogTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

int ErrorLogTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d_ptr->timeList.count();
}

QVariant ErrorLogTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || d_ptr->timeList.count() == 0 ||
            d_ptr->infoList.count() == 0)
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
            return d_ptr->infoList.at(row);
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
    case Qt::ForegroundRole:
        if (column == 1)
        {
            return d_ptr->colorList.at(row);
        }
        else
        {
            return QBrush(QColor("#2C405A"));
        }
    default:
        break;
    }

    return QVariant();
}

QVariant ErrorLogTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                return trs("ErrorLogInfo");
            }
        }
        break;
    }
    case Qt::ForegroundRole:
        return QBrush(QColor("#2C405A"));
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags ErrorLogTableModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    Qt::ItemFlags flags;
    flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

int ErrorLogTableModel::getHeaderHeightHint()
{
    return HEADER_HEIGHT_HINT;
}

int ErrorLogTableModel::getRowHeightHint()
{
    return ROW_HEIGHT_HINT;
}

void ErrorLogTableModel::loadData()
{
    beginResetModel();
    d_ptr->timeList.clear();
    d_ptr->infoList.clear();
    d_ptr->colorList.clear();
    d_ptr->dataIndex.clear();
    int index = d_ptr->currentPage * MAX_ROW_COUNT;
    int count = errorLog.count();
    d_ptr->totalPage = (count + MAX_ROW_COUNT - 1) / MAX_ROW_COUNT;
    for (int i = 0; i < MAX_ROW_COUNT; i ++)
    {
        ErrorLogItemBase *errLogItem;
        errLogItem = errorLog.getLog(index);
        if (errLogItem)
        {
            d_ptr->timeList.append(errLogItem->getTime());
            d_ptr->infoList.append(errLogItem->name());
            if (errLogItem->type() == CriticalFaultLogItem::Type)
            {
                d_ptr->colorList.append(Qt::red);
            }
            else
            {
                d_ptr->colorList.append(QColor("#2C405A"));
            }
            delete errLogItem;
            errLogItem = NULL;
        }
        else
        {
            break;
        }
        d_ptr->dataIndex.append(index);
        index++;
    }
    endResetModel();
}

int ErrorLogTableModel::getErrorLogIndex(int index)
{
    return d_ptr->dataIndex.at(index);
}

void ErrorLogTableModel::upBtnReleased()
{
    if (d_ptr->currentPage > 0)
    {
        d_ptr->currentPage--;
        loadData();
    }
}

void ErrorLogTableModel::downBtnReleased()
{
    if (d_ptr->currentPage < d_ptr->totalPage - 1)
    {
        d_ptr->currentPage++;
        loadData();
    }
}
