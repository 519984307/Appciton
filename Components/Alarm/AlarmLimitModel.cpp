/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#include "AlarmLimitModel.h"
#include "ParamInfo.h"
#include "Utility.h"
#include <QDebug>
#include <QColor>

enum
{
    SECTION_PARAM_NAME,
    SECTION_STATUS,
    SECTION_HIGH_LIMIT,
    SECTION_LOW_LIMIT,
    SECTION_LEVEL,
    SECTION_NR
};

class AlarmLimitModelPrivate
{
public:
    AlarmLimitModelPrivate() {}

    QList<AlarmDataInfo> alarmDataInfos;
};

AlarmLimitModel::AlarmLimitModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new AlarmLimitModelPrivate())
{
}

AlarmLimitModel::~AlarmLimitModel()
{
    delete d_ptr;
}

int AlarmLimitModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return SECTION_NR;
}

int AlarmLimitModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d_ptr->alarmDataInfos.count();
}

bool AlarmLimitModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)
    qDebug() << Q_FUNC_INFO;
    return false;
}

QVariant AlarmLimitModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();

    switch (role)
    {
    case Qt::TextColorRole:
        return QColor(Qt::black);
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    case Qt::DisplayRole:
    {
        switch (column)
        {
        case SECTION_PARAM_NAME:
            return trs(paramInfo.getSubParamName(d_ptr->alarmDataInfos.at(row).subParamID));
            break;
        case SECTION_STATUS:
            return d_ptr->alarmDataInfos.at(row).status ? trs("On") : trs("Off");
            break;
        case SECTION_HIGH_LIMIT:
            return Util::convertToString(d_ptr->alarmDataInfos.at(row).limitConfig.highLimit,
                                         d_ptr->alarmDataInfos.at(row).limitConfig.scale);
            break;
        case SECTION_LOW_LIMIT:
            return Util::convertToString(d_ptr->alarmDataInfos.at(row).limitConfig.lowLimit,
                                         d_ptr->alarmDataInfos.at(row).limitConfig.scale);
            break;
        case SECTION_LEVEL:
        {
            char limit = d_ptr->alarmDataInfos.at(row).alarmLevel;
            if (limit == 1)
            {
                return trs("normal");
            }
            else if (limit == 2)
            {
                return trs("high");
            }
            else
            {
                return trs("low");
            }
        }
        break;
        default:
            break;
        }
    }
    break;
    default:
        break;
    }

    return QVariant();
}

QVariant AlarmLimitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        break;
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case SECTION_PARAM_NAME:
                return trs("Parameters");
                break;
            case SECTION_STATUS:
                return trs("Status");
                break;
            case SECTION_HIGH_LIMIT:
                return trs("HighLimit");
                break;
            case SECTION_LOW_LIMIT:
                return trs("LowLimit");
                break;
            case SECTION_LEVEL:
                return trs("Level");
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags AlarmLimitModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

void AlarmLimitModel::setupAlarmDataInfos(const QList<AlarmDataInfo> &dataInfos)
{
    beginResetModel();
    d_ptr->alarmDataInfos = dataInfos;
    endResetModel();
}
