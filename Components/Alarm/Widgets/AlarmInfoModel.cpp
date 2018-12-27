/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/18
 **/

#include "AlarmInfoModel.h"
#include <QStringList>
#include "ThemeManager.h"
#include <QIcon>

#define DEFAULT_ROW_HEIGHT (themeManger.getAcceptableControlHeight())

class AlarmInfoModelPrivate
{
public:
    AlarmInfoModelPrivate(){}
    QStringList nameList;
    QStringList timeList;
    QList<QIcon> iconList;
};

AlarmInfoModel::AlarmInfoModel(QObject *parent)
    : QAbstractTableModel(parent),
      d_ptr(new AlarmInfoModelPrivate())
{
}

AlarmInfoModel::~AlarmInfoModel()
{
    delete d_ptr;
}

int AlarmInfoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d_ptr->nameList.count();
}

int AlarmInfoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant AlarmInfoModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();
    int row = index.row();
    switch (role)
    {
    case Qt::DisplayRole:
        if (index.isValid())
        {

            if (column == 0)
            {
                return d_ptr->nameList.at(index.row());
            }
            else
            {
                return d_ptr->timeList.at(index.row());
            }
        }
        break;

    case Qt::DecorationRole:
    {
        if (column == 0)
        {
            return QVariant(d_ptr->iconList[index.row()]);
        }
        break;
    }
    case Qt::SizeHintRole:
    {
        return QSize(10, DEFAULT_ROW_HEIGHT);
    }
    break;

    case Qt::TextAlignmentRole:
    {
        if (column == 0)
        {
            return QVariant(Qt::AlignVCenter | Qt::AlignLeft);
        }
        else
        {
            return QVariant(Qt::AlignVCenter | Qt::AlignRight);
        }
    }
    break;
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

bool AlarmInfoModel::setData(const QModelIndex &index, const QVariant &value, int role) const
{
    int row = index.row();
    switch (role)
    {
    case Qt::DecorationRole:
        d_ptr->iconList[row] = qvariant_cast<QIcon>(value);
        break;
    default:
        break;
    }
    return true;
}

void AlarmInfoModel::setStringList(const QStringList &nameList, const QStringList &timeList)
{
    beginResetModel();
    d_ptr->nameList = nameList;
    d_ptr->timeList = timeList;
    d_ptr->iconList.clear();
    for (int i = 0; i < d_ptr->nameList.count(); i++)
    {
        d_ptr->iconList.append(QIcon());
    }
    endResetModel();
}

int AlarmInfoModel::getRowHeightHint() const
{
    return DEFAULT_ROW_HEIGHT;
}
