/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/31
 **/
#include "TitrateTableModel.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"
#include <QEvent>
#include "TitrateTableManager.h"
#include "TitrateTableDefine.h"
#include <QResizeEvent>
#include "TitrateTableWindow.h"

#define ROW_HEIGHT_HINT (themeManager.getAcceptableControlHeight())
#define HEADER_HEIGHT_HINT (themeManager.getAcceptableControlHeight())

class TitrateTableModelPrivate
{
public:
    TitrateTableModelPrivate()
        : viewWidth(700),
          rowPrev(0)
    {}

    QList<TitrateTableInfo> titrateTableDataInfos;
    int viewWidth;
    int rowPrev;
};

TitrateTableModel::TitrateTableModel(QObject *parent)
    : QAbstractTableModel(parent),
      d_ptr(new TitrateTableModelPrivate)
{
}

TitrateTableModel::~TitrateTableModel()
{
    delete d_ptr;
}

int TitrateTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_NR;
}

int TitrateTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    int rowNext = d_ptr->titrateTableDataInfos.count() % (MAX_ROWCOUNT * NUM_ABREAST);
    int rowPrev = (d_ptr->titrateTableDataInfos.count() - rowNext) / NUM_ABREAST;
    d_ptr->rowPrev = rowPrev - 1;
    return (rowPrev + rowNext);
}

QVariant TitrateTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();
    int certainRow = 0;
    int ratioRow = 0;

    ratioRow = row / MAX_ROWCOUNT;
    certainRow = row + ratioRow * MAX_ROWCOUNT * (NUM_ABREAST - 1);

    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
        break;

    case Qt::DisplayRole:
    {
        switch (column)
        {
        case COLUMN_DOSE1:
            return d_ptr->titrateTableDataInfos.at(certainRow).doseValue;
            break;

        case COLUMN_RATE1:
            return d_ptr->titrateTableDataInfos.at(certainRow).rateVlue;
            break;

        case COLUMN_DOSE2:
            if (row > d_ptr->rowPrev)
            {
                return QVariant();
            }
            return d_ptr->titrateTableDataInfos.at(certainRow + MAX_ROWCOUNT).doseValue;
            break;

        case COLUMN_RATE2:
            if (row > d_ptr->rowPrev)
            {
                return QVariant();
            }
            return d_ptr->titrateTableDataInfos.at(certainRow + MAX_ROWCOUNT).rateVlue;
            break;

        case COLUMN_DOSE3:
            if (row > d_ptr->rowPrev)
            {
                return QVariant();
            }
            return d_ptr->titrateTableDataInfos.at(certainRow + MAX_ROWCOUNT * 2).doseValue;
            break;

        case COLUMN_RATE3:
            if (row > d_ptr->rowPrev)
            {
                return QVariant();
            }
            return d_ptr->titrateTableDataInfos.at(certainRow + MAX_ROWCOUNT * 2).rateVlue;
            break;

        default:
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

    case Qt::BackgroundRole:
        return Qt::white;
        break;

    case Qt::SizeHintRole:
    {
        int w = d_ptr->viewWidth / COLUMN_NR;
        return QSize(w, ROW_HEIGHT_HINT);
    }
        break;
    default:
        break;
    }

    return QVariant();
}

QVariant TitrateTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
        break;
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal)
        {
            QString doseValue(trs("Dosemin"));
            QString rateValue;
            if (titrateTableManager.getSetTableParam().datumTerm ==
                    DATUM_TERM_DRIPRATE)
            {
                rateValue = trs("DripRate");
            }
            else
            {
                rateValue = trs("InfusionRate");
            }

            switch (section)
            {
            case COLUMN_DOSE1:
                return doseValue;
                break;
            case COLUMN_RATE1:
                return rateValue;
                break;
            case COLUMN_DOSE2:
                return doseValue;
                break;
            case COLUMN_RATE2:
                return rateValue;
                break;
            case COLUMN_DOSE3:
                return doseValue;
                break;
            case COLUMN_RATE3:
                return rateValue;
                break;
            default:
                break;
            }
        }
        break;
    case Qt::SizeHintRole:
        if (orientation == Qt::Horizontal)
        {
            int w = d_ptr->viewWidth / (COLUMN_NR);
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

void TitrateTableModel::setupTitrateTableInfos(const QList<TitrateTableInfo> &dataInfos)
{
    beginResetModel();
    d_ptr->titrateTableDataInfos = dataInfos;
    endResetModel();
}

bool TitrateTableModel::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj->isWidgetType() && ev->type() == QEvent::Resize)
    {
        // update the header width
        QResizeEvent *re = static_cast<QResizeEvent *>(ev);
        d_ptr->viewWidth = re->size().width();
        emit headerDataChanged(Qt::Horizontal, 0, COLUMN_NR);
    }
    return QObject::eventFilter(obj, ev);
}


int TitrateTableModel::getRowHeightHint() const
{
    return ROW_HEIGHT_HINT;
}

int TitrateTableModel::getHeaderHeightHint() const
{
    return HEADER_HEIGHT_HINT;
}

















