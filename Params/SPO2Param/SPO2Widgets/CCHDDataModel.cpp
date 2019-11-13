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
#include "Framework/Language/LanguageManager.h"
#include "Framework/TimeDate/TimeDate.h"
#include "ThemeManager.h"
#include "WindowManager.h"

#define ROW_COUNT 3
#define ROW_HEIGHT_HINT (themeManager.getAcceptableControlHeight())


enum ColumnHeader
{
    HeaderTime,
    HeaderHandValue,
    HeaderFootValue,
    HeaderResult,
    HeaderNR
};

class CCHDDataModelPrivate
{
public:
    CCHDDataModelPrivate()
        : tableViewWidth(0)
    {}
    ~CCHDDataModelPrivate(){}

    QList<cchdData> cchdDataList;
    int tableViewWidth;
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
    return static_cast<int>(HeaderNR);
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
        if (column == HeaderTime)
        {
            unsigned timeStamp = d_ptr->cchdDataList.at(row).time;
            if (timeStamp)
            {
                QString time = timeDate->getTime(timeStamp, true);
                return time;
            }
            else
            {
                return QString();
            }
        }
        else if (column == HeaderHandValue)
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
        else if (column == HeaderFootValue)
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
        else if (column == HeaderResult)
        {
            CCHDResult result = d_ptr->cchdDataList.at(row).result;
            if (result == CCHD_NR)
            {
                return QString();
            }
            else
            {
                return trs(SPO2Symbol::convert(result));
            }
        }
        break;
    }
    case Qt::SizeHintRole:
        return QSize(10, ROW_HEIGHT_HINT);
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant CCHDDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
    {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
    {
        if (section == HeaderTime)
        {
            return QString(trs("Time"));
        }
        else if (section == HeaderHandValue)
        {
            return QString(trs("handValue"));
        }
        else if (section == HeaderFootValue)
        {
            return QString(trs("footValue"));
        }
        else if (section == HeaderResult)
        {
            return QString(trs("Result"));
        }
        break;
    }
    case Qt::BackgroundColorRole:
        return themeManager.getColor(ThemeManager::ControlTypeNR,
                                    ThemeManager::ElementBackgound,
                                    ThemeManager::StateDisabled);
        break;
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
        break;
    case Qt::SizeHintRole:
    {
        if (d_ptr->tableViewWidth == 0)
        {
            break;
        }
        switch (section)
        {
        case HeaderTime:
        case HeaderHandValue:
        case HeaderFootValue:
            return QSize(d_ptr->tableViewWidth / 6, ROW_HEIGHT_HINT);
        case HeaderResult:
            return QSize(d_ptr->tableViewWidth / 2, ROW_HEIGHT_HINT);
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    return QVariant();
}

int CCHDDataModel::getHeightEachRow()
{
    return ROW_HEIGHT_HINT;
}

void CCHDDataModel::updateData()
{
    beginResetModel();
    d_ptr->cchdDataList = spo2Param.getCCHDDataList();
    endResetModel();
}

void CCHDDataModel::setTableViewWidth(int width)
{
    d_ptr->tableViewWidth = width;
}

