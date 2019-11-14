/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/18
 **/

#include "ListDataModel.h"
#include <QStringList>
#include <QIcon>
#include "ThemeManager.h"

#define DEFAULT_ROW_HEIGHT (themeManager.getAcceptableControlHeight())

class ListDataModelPrivate
{
public:
    ListDataModelPrivate() {}
    QStringList list;
    QList<QIcon> iconList;
};

ListDataModel::ListDataModel(QObject *parent)
    : QAbstractListModel(parent),
      d_ptr(new ListDataModelPrivate())
{
}

ListDataModel::~ListDataModel()
{
    delete d_ptr;
}

int ListDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d_ptr->list.count();
}

QVariant ListDataModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        if (index.isValid())
        {
            return d_ptr->list.at(index.row());
        }
        break;

    case Qt::DecorationRole:
        return QVariant(d_ptr->iconList.at(index.row()));
        break;

    case Qt::SizeHintRole:
    {
        return QSize(10, DEFAULT_ROW_HEIGHT);
    }
    break;

    case Qt::TextAlignmentRole:
    {
        return QVariant(Qt::AlignVCenter | Qt::AlignLeft);
    }
    break;
    case Qt::BackgroundRole:
        if (index.row() % 2)
        {
            return themeManager.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                        ThemeManager::StateDisabled);
        }
        else
        {
            return themeManager.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
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

bool ListDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && index.row() >= d_ptr->list.count())
    {
        return false;
    }

    switch (role)
    {
    case Qt::DisplayRole:
        d_ptr->list[index.row()] = value.toString();
        emit dataChanged(index, index);
        return true;
    case Qt::DecorationRole:
        d_ptr->iconList[index.row()] = qvariant_cast<QIcon>(value);
        emit dataChanged(index, index);
        return true;
    default:
        break;
    }

    return false;
}

void ListDataModel::setStringList(const QStringList &list)
{
    beginResetModel();
    d_ptr->list = list;
    d_ptr->iconList.clear();
    for (int i = 0; i < d_ptr->list.count(); i++)
    {
        d_ptr->iconList.append(QIcon());
    }
    endResetModel();
}

QStringList ListDataModel::getStringList() const
{
    return d_ptr->list;
}

int ListDataModel::getRowHeightHint() const
{
    return DEFAULT_ROW_HEIGHT;
}

int ListDataModel::getRowCount() const
{
    return d_ptr->list.count();
}
