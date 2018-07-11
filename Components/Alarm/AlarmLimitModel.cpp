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
#include <QResizeEvent>
#include <QBrush>

enum
{
    SECTION_PARAM_NAME,
    SECTION_STATUS,
    SECTION_HIGH_LIMIT,
    SECTION_LOW_LIMIT,
    SECTION_LEVEL,
    SECTION_NR
};

#define ROW_HEIGHT_HINT 40
#define HEADER_HEIGHT_HINT 40

class AlarmLimitModelPrivate
{
public:
    AlarmLimitModelPrivate()
        : viewWidth(400)
    {}

    QList<AlarmDataInfo> alarmDataInfos;
    int viewWidth;
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
    case Qt::TextAlignmentRole:
        if (column == SECTION_PARAM_NAME)
        {
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        }
        else
        {
            return QVariant(Qt::AlignCenter);
        }
        break;
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

    case Qt::ForegroundRole:
        return QBrush(QColor("#2C405A"));
        break;

    case Qt::BackgroundRole:
        return Qt::white;
        break;

    case Qt::SizeHintRole:
        return QSize(10, ROW_HEIGHT_HINT);
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
        if (section == SECTION_PARAM_NAME)
        {
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        }
        else
        {
            return QVariant(Qt::AlignCenter);
        }
        break;
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case SECTION_PARAM_NAME:
                return trs("Parameter");
                break;
            case SECTION_STATUS:
                return trs("AlarmStatus");
                break;
            case SECTION_HIGH_LIMIT:
                return trs("UpperLimit");
                break;
            case SECTION_LOW_LIMIT:
                return trs("LowerLimit");
                break;
            case SECTION_LEVEL:
                return trs("AlarmPriority");
                break;
            default:
                break;
            }
        }
        break;
    case Qt::SizeHintRole:
        if (orientation == Qt::Horizontal)
        {
            int w = d_ptr->viewWidth / (SECTION_NR + 1);
            if (section == SECTION_PARAM_NAME)
            {
                w += w;
            }
            else if (section == SECTION_NR - 1)
            {
                // add the remains for the last section
                w += d_ptr->viewWidth % (SECTION_NR + 1);
            }

            return QSize(w, HEADER_HEIGHT_HINT);
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

bool AlarmLimitModel::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj->isWidgetType() && ev->type() == QEvent::Resize)
    {
        // update the header width
        QResizeEvent *re = static_cast<QResizeEvent *>(ev);
        d_ptr->viewWidth = re->size().width();
        emit headerDataChanged(Qt::Horizontal, 0, SECTION_NR);
    }
    return QObject::eventFilter(obj, ev);
}

int AlarmLimitModel::getRowHeightHint() const
{
    return ROW_HEIGHT_HINT;
}

int AlarmLimitModel::getHeaderHeightHint() const
{
    return HEADER_HEIGHT_HINT;
}
