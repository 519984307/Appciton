/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/7
 **/

#include "HemodynamicDataModel.h"
#include <QBrush>
#include <QColor>
#include "Framework/UI/ThemeManager.h"

#define MAX_ROW_COUNT  8
#define ROW_HEIGHT_HINT 45
#define HEADER_HEIGHT_HINT 42

class HemodynamicDataModelPrivate
{
public:
    HemodynamicDataModelPrivate();
    int viewWidth;
    QList<ReviewDataInfo> dataInfos;
    QList<ReviewDataInfo> colorInfos;
    ReviewDataInfo headDataInfo;
};

HemodynamicDataModelPrivate::HemodynamicDataModelPrivate()
                           : viewWidth(themeManager.defaultWindowSize().width())
{
    dataInfos.clear();
    headDataInfo.time.clear();
    headDataInfo.valueType = "";
    headDataInfo.unitRange = "";
}


HemodynamicDataModel::HemodynamicDataModel(QObject *parent)
                    : QAbstractTableModel(parent),
                      d_ptr(new HemodynamicDataModelPrivate)
{
}

HemodynamicDataModel::~HemodynamicDataModel()
{
    delete d_ptr;
}

int HemodynamicDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_NUM_NR;
}

int HemodynamicDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return MAX_ROW_COUNT;
}

QVariant HemodynamicDataModel::data(const QModelIndex &index, int role) const
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
            return QVariant(Qt::AlignCenter);
        break;

        case Qt::DisplayRole:
        {
            switch (column)
            {
            case COLUMN_VALUE_TYPE:
                return d_ptr->dataInfos.at(row).valueType;
                break;
            case COLUMN_UNIT_RANGE:
                return d_ptr->dataInfos.at(row).unitRange;
                break;
            case COLUMN_TIME_1:
                if (d_ptr->dataInfos.at(row).time.count() < column - 1)
                {
                    return QVariant();
                }
                return d_ptr->dataInfos.at(row).time.at(column - 2);
                break;
            case COLUMN_TIME_2:
                if (d_ptr->dataInfos.at(row).time.count() < column - 1)
                {
                    return QVariant();
                }
                return d_ptr->dataInfos.at(row).time.at(column - 2);
                break;
            case COLUMN_TIME_3:
                if (d_ptr->dataInfos.at(row).time.count() < column - 1)
                {
                    return QVariant();
                }
                return d_ptr->dataInfos.at(row).time.at(column - 2);
                break;
            case COLUMN_TIME_4:
                if (d_ptr->dataInfos.at(row).time.count() < column - 1)
                {
                    return QVariant();
                }
                return d_ptr->dataInfos.at(row).time.at(column - 2);
                break;
            }
        }
        break;

        case Qt::EditRole:
            break;

        case Qt::CheckStateRole:
            break;

        case Qt::ForegroundRole:
            return QBrush(QColor("#2C405A"));
            break;

        case Qt::BackgroundColorRole:
        {
            switch (column)
            {
            case COLUMN_VALUE_TYPE:
                return QBrush(QColor("#C7CfD6"));
                break;
            case COLUMN_UNIT_RANGE:
                return QBrush(QColor("#C7CfD6"));
                break;
            case COLUMN_TIME_1:
                if (d_ptr->colorInfos.at(row).time.count() < column - 1)
                {
                    return QBrush(QColor("#FFFFFF"));
                }
                return QBrush(QColor(d_ptr->colorInfos.at(row).time.at(column - 2)));
                break;
            case COLUMN_TIME_2:
                if (d_ptr->colorInfos.at(row).time.count() < column - 1)
                {
                    return QBrush(QColor("#FFFFFF"));
                }
                return QBrush(QColor(d_ptr->colorInfos.at(row).time.at(column - 2)));
                break;
            case COLUMN_TIME_3:
                if (d_ptr->colorInfos.at(row).time.count() < column - 1)
                {
                    return QBrush(QColor("#FFFFFF"));
                }
                return QBrush(QColor(d_ptr->colorInfos.at(row).time.at(column - 2)));
                break;
            case COLUMN_TIME_4:
                if (d_ptr->colorInfos.at(row).time.count() < column - 1)
                {
                    return QBrush(QColor("#FFFFFF"));
                }
                return QBrush(QColor(d_ptr->colorInfos.at(row).time.at(column - 2)));
                break;
            }
        }
        break;

        case Qt::SizeHintRole:
                int w = (d_ptr->viewWidth - 20)/ COLUMN_NUM_NR;
                return QSize(w, ROW_HEIGHT_HINT);
            break;
    };
    return QVariant();
}

QVariant HemodynamicDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
        break;
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case COLUMN_VALUE_TYPE:
                return d_ptr->headDataInfo.valueType;
                break;
            case COLUMN_UNIT_RANGE:
                return d_ptr->headDataInfo.unitRange;
                break;
            case COLUMN_TIME_1:
                return d_ptr->headDataInfo.time.at(0);
                break;
            case COLUMN_TIME_2:
                return d_ptr->headDataInfo.time.at(1);
                break;
            case COLUMN_TIME_3:
                return d_ptr->headDataInfo.time.at(2);
                break;
            case COLUMN_TIME_4:
                return d_ptr->headDataInfo.time.at(3);
                break;
            default:
                break;
            }
        }
        break;
    case Qt::SizeHintRole:
        if (orientation == Qt::Horizontal)
        {
            int w = (d_ptr->viewWidth - 20)/ COLUMN_NUM_NR;
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


void HemodynamicDataModel::setDynamicInfos(const QList<ReviewDataInfo> &dataInfos)
{
    beginResetModel();
    d_ptr->dataInfos = dataInfos;
    endResetModel();
}

void HemodynamicDataModel::setDynamicColorInfos(const QList<ReviewDataInfo> &colorInfos)
{
    beginResetModel();
    d_ptr->colorInfos = colorInfos;
    endResetModel();
}

void HemodynamicDataModel::setHeadInfos(const ReviewDataInfo &dataInfos)
{
    beginResetModel();
    d_ptr->headDataInfo = dataInfos;
    endResetModel();
}

int HemodynamicDataModel::getRowHeightHint() const
{
    return ROW_HEIGHT_HINT;
}

int HemodynamicDataModel::getHeaderHeightHint() const
{
    return HEADER_HEIGHT_HINT;
}

int HemodynamicDataModel::getRowCount() const
{
    return MAX_ROW_COUNT;
}
