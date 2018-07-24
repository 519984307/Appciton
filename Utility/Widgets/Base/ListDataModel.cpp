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
#include "ThemeManager.h"

#define DEFAULT_ROW_HEIGHT (themeManger.getAcceptableControlHeight())

class ListDataModelPrivate
{
public:
    ListDataModelPrivate() {}
    QStringList list;
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

    case Qt::SizeHintRole:
    {
        return QSize(DEFAULT_ROW_HEIGHT, 10);
    }
    break;

    case Qt::TextAlignmentRole:
    {
        return QVariant(Qt::AlignVCenter | Qt::AlignLeft);
    }
    break;
    case Qt::BackgroundRole:
        return QBrush(QColor(247, 247, 247));
        break;
    case Qt::ForegroundRole:
        return QBrush(QColor("#2C405A"));
        break;
    default:
        break;
    }

    return QVariant();
}

void ListDataModel::setStringList(const QStringList &list)
{
    beginResetModel();
    d_ptr->list = list;
    endResetModel();
}

QStringList ListDataModel::getStringList() const
{
    return d_ptr->list;
}
