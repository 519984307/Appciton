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
#include "ItemEditInfo.h"
#include "ParamManager.h"
#include "ThemeManager.h"
#include "LanguageManager.h"


#define ROW_HEIGHT_HINT (themeManger.getAcceptableControlHeight())
#define HEADER_HEIGHT_HINT (themeManger.getAcceptableControlHeight())

class AlarmLimitModelPrivate
{
public:
    AlarmLimitModelPrivate()
        : viewWidth(400),  // set default value to 400
          editRow(-1),
          eachPageRowCount(0)
    {
    }

    QList<AlarmDataInfo> alarmDataInfos;
    int viewWidth;
    int editRow;
    QModelIndex editIndex;
    int eachPageRowCount;

    /**
     * @brief calTotalRowCount 计算总页数
     * @return
     */
    int calTotalPage();
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
    int page = d_ptr->calTotalPage();
    if (page == 0)
    {
        // 没有数据时也仍然占一页
        page = 1;
    }
    return page * d_ptr->eachPageRowCount;
}

void AlarmLimitModel::alarmDataUpdate(const AlarmDataInfo &info, int type)
{
    UnitType unit = paramManager.getSubParamUnit(info.paramID,
                    info.subParamID);
    switch (type)
    {
    case SECTION_STATUS:
        alarmConfig.setLimitAlarmEnable(info.subParamID, info.status);
        break;
    case SECTION_LEVEL:
        alarmConfig.setLimitAlarmPriority(info.subParamID,
                                          static_cast<AlarmPriority>(info.alarmLevel));
        break;
    case SECTION_HIGH_LIMIT:
    {
        alarmConfig.setLimitAlarmConfig(info.subParamID,
                                        unit, info.limitConfig);
        Param *param = paramManager.getParam(info.paramID);
        if (param)
        {
            param->updateSubParamLimit(info.subParamID);
        }
        break;
    }
    case SECTION_LOW_LIMIT:
    {
        alarmConfig.setLimitAlarmConfig(info.subParamID,
                                        unit, info.limitConfig);
        Param *param = paramManager.getParam(info.paramID);
        if (param)
        {
            param->updateSubParamLimit(info.subParamID);
        }
        break;
    }
    }
}

bool AlarmLimitModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        if (value.type() == QVariant::Int)
        {
            int newValue = value.toInt();
            int row = index.row();
            switch (index.column())
            {
            case SECTION_STATUS:
                d_ptr->alarmDataInfos[row].status = newValue;
                alarmDataUpdate(d_ptr->alarmDataInfos[row], index.column());
                break;
            case SECTION_LEVEL:
                d_ptr->alarmDataInfos[row].alarmLevel = newValue + 1;
                alarmDataUpdate(d_ptr->alarmDataInfos[row], index.column());
                break;
            case SECTION_HIGH_LIMIT:
                d_ptr->alarmDataInfos[row].limitConfig.highLimit = newValue;
                alarmDataUpdate(d_ptr->alarmDataInfos[row], index.column());
                break;
            case SECTION_LOW_LIMIT:
                d_ptr->alarmDataInfos[row].limitConfig.lowLimit = newValue;
                alarmDataUpdate(d_ptr->alarmDataInfos[row], index.column());
                break;
            default:
                break;
            }
        }
    }

    if (role == Qt::CheckStateRole)
    {
        Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());

        if (checkState == Qt::PartiallyChecked)
        {
            d_ptr->editIndex = QModelIndex();
        }
        else if (checkState == Qt::Checked)
        {
            d_ptr->editIndex = index;
        }
        emit dataChanged(d_ptr->editIndex, d_ptr->editIndex);
    }


    return true;
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
        if (row >= d_ptr->alarmDataInfos.count())
        {
            break;
        }
        switch (column)
        {
        case SECTION_PARAM_NAME:
        {
            SubParamID subId = d_ptr->alarmDataInfos.at(row).subParamID;
            ParamID paramId = d_ptr->alarmDataInfos.at(row).paramID;
            UnitType unit = paramManager.getSubParamUnit(paramId, subId);
            QString name = QString("%1(%2)")
                    .arg(trs(paramInfo.getSubParamName(subId)))
                    .arg(trs(Unit::getSymbol(unit)));
            return name;
        }
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
                return trs("Medium");
            }
            else if (limit == 2)
            {
                return trs("High");
            }
        }
        break;
        default:
            break;
        }
    }
    break;

    case Qt::EditRole:
    {
        if (index == d_ptr->editIndex && row < d_ptr->alarmDataInfos.count())
        {
            int row = index.row();
            ItemEditInfo editInfo;
            switch (index.column())
            {
            case SECTION_STATUS:
                editInfo.type = ItemEditInfo::LIST;
                editInfo.list << trs("Off") << trs("On");
                editInfo.curValue = d_ptr->alarmDataInfos.at(row).status;
                break;
            case SECTION_LEVEL:
                editInfo.type = ItemEditInfo::LIST;
                editInfo.list << trs("Medium") << trs("High");
                editInfo.curValue = d_ptr->alarmDataInfos.at(row).alarmLevel - 1;
                break;
            case SECTION_LOW_LIMIT:
                editInfo.type = ItemEditInfo::VALUE;
                editInfo.scale = d_ptr->alarmDataInfos.at(row).limitConfig.scale;
                editInfo.step = d_ptr->alarmDataInfos.at(row).limitConfig.step;
                editInfo.curValue = d_ptr->alarmDataInfos.at(row).limitConfig.lowLimit;
                editInfo.highLimit = d_ptr->alarmDataInfos.at(row).limitConfig.highLimit - editInfo.step;
                editInfo.lowLimit = d_ptr->alarmDataInfos.at(row).limitConfig.minLowLimit;
                break;
            case SECTION_HIGH_LIMIT:
                editInfo.type = ItemEditInfo::VALUE;
                editInfo.scale = d_ptr->alarmDataInfos.at(row).limitConfig.scale;
                editInfo.step = d_ptr->alarmDataInfos.at(row).limitConfig.step;
                editInfo.curValue = d_ptr->alarmDataInfos.at(row).limitConfig.highLimit;
                editInfo.highLimit = d_ptr->alarmDataInfos.at(row).limitConfig.maxHighLimit;
                editInfo.lowLimit = d_ptr->alarmDataInfos.at(row).limitConfig.lowLimit + editInfo.step;
                break;
            default:
                break;
            }
            return qVariantFromValue(editInfo);
        }
    }
    break;

    case Qt::CheckStateRole:
        if (index.row() == d_ptr->editRow && index.column() > SECTION_PARAM_NAME
                && row < d_ptr->alarmDataInfos.count())
        {
            if (index == d_ptr->editIndex)
            {
                return QVariant(Qt::Checked);
            }

            return QVariant(Qt::PartiallyChecked);
        }
        break;

    case Qt::ForegroundRole:
        return QBrush(QColor("#2C405A"));
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
                w += w / 2;
                w += d_ptr->viewWidth % (SECTION_NR + 1);
            }
            else
            {
                // 平均分配多余的空间
                int remainWidth = w / 2;
                w = w + remainWidth / (SECTION_NR - 1);
            }

            return QSize(w, HEADER_HEIGHT_HINT);
        }
        break;
    case Qt::BackgroundRole:
        return themeManger.getColor(ThemeManager::ControlTypeNR,
                                    ThemeManager::ElementBackgound,
                                    ThemeManager::StateDisabled);
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

    Qt::ItemFlags flags;
    if (d_ptr->editRow == index.row())
    {
        if (index.column() != SECTION_PARAM_NAME)
        {
            flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }
        else
        {
            flags = Qt::ItemIsEnabled;
        }
    }
    else
    {
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    return flags;
}

void AlarmLimitModel::setupAlarmDataInfos(const QList<AlarmDataInfo> &dataInfos, bool updateImmediately)
{
    beginResetModel();
    d_ptr->alarmDataInfos = dataInfos;
    endResetModel();

    if (updateImmediately == false)
    {
        return;
    }

    for (int i = 0; i < dataInfos.count(); i++)
    {
        AlarmDataInfo info = dataInfos.at(i);
        UnitType unit = paramManager.getSubParamUnit(info.paramID,
                        info.subParamID);
        alarmConfig.setLimitAlarmConfig(info.subParamID,
                                        unit, info.limitConfig);
        Param *param = paramManager.getParam(info.paramID);
        if (param && systemManager.isSupport(param->getParamID()))
        {
            param->updateSubParamLimit(info.subParamID);
        }
    }
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

void AlarmLimitModel::editRowData(int row)
{
    if (row == d_ptr->editRow)
    {
        return;
    }

    QModelIndex topLeft;
    QModelIndex rightBottom;
    int oldEditRow = d_ptr->editRow;
    d_ptr->editRow = row;
    if (oldEditRow >= 0 && oldEditRow < d_ptr->alarmDataInfos.count())
    {
        topLeft = index(oldEditRow, 0);
        rightBottom = index(oldEditRow, SECTION_NR - 1);
        emit dataChanged(topLeft, rightBottom);
    }

    if (d_ptr->editRow >= 0 && d_ptr->editRow < d_ptr->alarmDataInfos.count())
    {
        topLeft = index(d_ptr->editRow, 0);
        rightBottom = index(d_ptr->editRow, SECTION_NR - 1);
        emit dataChanged(topLeft, rightBottom);
    }
}

void AlarmLimitModel::stopEditRow()
{
    int oldEditRow = d_ptr->editRow;
    d_ptr->editRow = -1;
    if (oldEditRow >= 0 && oldEditRow < d_ptr->alarmDataInfos.count())
    {
        QModelIndex topLeft = index(oldEditRow, 0);
        QModelIndex rightBottom = index(oldEditRow, SECTION_NR - 1);
        emit dataChanged(topLeft, rightBottom);
    }
}

int AlarmLimitModel::curEditRow() const
{
    return d_ptr->editRow;
}

void AlarmLimitModel::setEachPageRowCount(int rows)
{
    d_ptr->eachPageRowCount = rows;
}

int AlarmLimitModelPrivate::calTotalPage()
{
    if (!eachPageRowCount)
    {
        return 0;
    }

    int count = alarmDataInfos.count();
    int remainder = count % eachPageRowCount;
    int pages = count / eachPageRowCount;
    if (remainder > 0)
    {
        pages += 1;
    }
    return pages;
}
