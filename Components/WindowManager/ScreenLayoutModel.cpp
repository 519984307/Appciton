/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#include "ScreenLayoutModel.h"
#include "WindowManager.h"

#define WAVE_SPAN 4
#define PARAm_SPAN 2
#define COLUMN_COUNT 6
#define ROW_COUNT 8
class ScreenLayoutModelPrivate
{
public:
    ScreenLayoutModelPrivate() {}
};

ScreenLayoutModel::ScreenLayoutModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new ScreenLayoutModelPrivate)
{
}

int ScreenLayoutModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

int ScreenLayoutModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ROW_COUNT;
}

QVariant ScreenLayoutModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::DisplayRole:
        return QString("(%1, %2)").arg(index.row()).arg(index.column());
    default:

        break;
    }
    return QVariant();
}

Qt::ItemFlags ScreenLayoutModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QSize ScreenLayoutModel::span(const QModelIndex &index) const
{
    if (index.isValid())
    {
        if (index.row() < 6 && index.column() == 0)
        {
            return QSize(WAVE_SPAN, 1);
        }
    }
    return QAbstractTableModel::span(index);
}

ScreenLayoutModel::~ScreenLayoutModel()
{
    delete d_ptr;
}
