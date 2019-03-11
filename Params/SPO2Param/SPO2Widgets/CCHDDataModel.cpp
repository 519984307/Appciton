/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/13
 **/

#include "CCHDDataModel.h"
#include "LanguageManager.h"
#include "ThemeManager.h"
#include "WindowManager.h"

#define ROW_COUNT 3
#define COLUMN_COUNT 2

class CCHDDataModelPrivate
{
public:
    CCHDDataModelPrivate(){}
    ~CCHDDataModelPrivate(){}

    QList<cchdData> cchdDataList;
};

CCHDDataModel::CCHDDataModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new CCHDDataModelPrivate)
{
}

CCHDDataModel::~CCHDDataModel()
{
    delete d_ptr;
}

int CCHDDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ROW_COUNT;
}

int CCHDDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

QVariant CCHDDataModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();
    if (!index.isValid() || (row + 1) > d_ptr->cchdDataList.count())
    {
        return QVariant();
    }

    switch (role) {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
        break;
    case Qt::DisplayRole:
    {
        if (column == 0)
        {
            short handValue = d_ptr->cchdDataList.at(row).handValue;
            if (handValue != InvData())
            {
                return QString::number(handValue);
            }
            else
            {
                return QString();
            }
        }
        else
        {
            short footValue = d_ptr->cchdDataList.at(row).footValue;
            if (footValue != InvData())
            {
                return QString::number(footValue);
            }
            else
            {
                return QString();
            }
        }
    }
    default:
        break;
    }
    return QVariant();
}

QVariant CCHDDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)

    switch (role) {
    case Qt::DisplayRole:
    {
        if (section == 0)
        {
            return QString(trs("handValue"));
        }
        else
        {
            return QString(trs("footValue"));
        }
        break;
    }
    case Qt::BackgroundColorRole:
        return themeManger.getColor(ThemeManager::ControlTypeNR,
                                    ThemeManager::ElementBackgound,
                                    ThemeManager::StateDisabled);
    default:
        break;
    }
    return QVariant();
}

int CCHDDataModel::getHeightEachRow()
{
    return themeManger.getAcceptableControlHeight();
}

void CCHDDataModel::addData(QList<cchdData> dataList)
{
    beginResetModel();
    d_ptr->cchdDataList = dataList;
    endResetModel();
}

void CCHDDataModel::addData(int value, bool isHand)
{
    beginResetModel();
    cchdData data;
    if (isHand)
    {
        if (d_ptr->cchdDataList.count() != 0 && d_ptr->cchdDataList.last().handValue == InvData())
        {
            d_ptr->cchdDataList.last().handValue = value;
            endResetModel();
            return;
        }
        data.handValue = value;
    }
    else
    {
        if (d_ptr->cchdDataList.count() != 0 && d_ptr->cchdDataList.last().footValue == InvData())
        {
            d_ptr->cchdDataList.last().footValue = value;
            endResetModel();
            return;
        }
        data.footValue = value;
    }
    d_ptr->cchdDataList.append(data);
    endResetModel();
}
