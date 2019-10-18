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
#include "LanguageManager.h"
#include <QIcon>
#include <QResizeEvent>
#include <QDebug>

#define DEFAULT_ROW_HEIGHT (themeManger.getAcceptableControlHeight())
#define EACH_PAGE_ALARM_COUNT 7    // 一页最大显示数

enum
{
    SECTION_DATE_TIME,
    SECTION_ALARM_DESCRIPTION,
    SECTION_ALARM_PRIORITY,
    SECTION_NR
};

class AlarmInfoModelPrivate
{
public:
    AlarmInfoModelPrivate()
        : viewWidth(0),
          rowEachPage(0)
    {}
    QStringList nameList;
    QStringList timeList;
    QStringList prorityList;
    QList<QIcon> iconList;
    int viewWidth;
    int rowEachPage;
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
    // 计算行数
    int pageCount = d_ptr->nameList.count() / EACH_PAGE_ALARM_COUNT;
    if (d_ptr->nameList.count() % EACH_PAGE_ALARM_COUNT)
    {
        pageCount += 1;
    }
    else if (pageCount == 0)
    {
        // 没有数据时也仍然占一页
        pageCount = 1;
    }
    return pageCount * EACH_PAGE_ALARM_COUNT;
}

int AlarmInfoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return SECTION_NR;
}

QVariant AlarmInfoModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();
    int row = index.row();
    switch (role)
    {
    case Qt::DisplayRole:
        if (index.isValid() && index.row() < d_ptr->nameList.count())
        {
            if (column == SECTION_DATE_TIME)
            {
                return d_ptr->timeList.at(index.row());
            }
            else if (column == SECTION_ALARM_DESCRIPTION)
            {
                return d_ptr->nameList.at(index.row());
            }
            else if (column == SECTION_ALARM_PRIORITY)
            {
                return d_ptr->prorityList.at(index.row());
            }
        }
        break;

    case Qt::DecorationRole:
    {
        if (column == SECTION_ALARM_DESCRIPTION && index.row() < d_ptr->nameList.count())
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
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
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

QVariant AlarmInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        switch (role)
        {
        case Qt::DisplayRole:
            if (section == SECTION_DATE_TIME)
            {
                return QString(trs("Date") + "/" + trs("Time"));
            }
            else if (section == SECTION_ALARM_DESCRIPTION)
            {
                return trs("AlarmDescription");
            }
            else if (section == SECTION_ALARM_PRIORITY)
            {
                return trs("AlarmPriority");
            }
            break;
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case Qt::SizeHintRole:
        {
            int averageW = d_ptr->viewWidth / (SECTION_NR + 3);
            int remainW = d_ptr->viewWidth % (SECTION_NR + 3);
            if (section == SECTION_DATE_TIME)
            {
                return QSize(averageW * 2, DEFAULT_ROW_HEIGHT);
            }
            else if (section == SECTION_ALARM_DESCRIPTION)
            {
                return QSize(averageW * 3 + remainW, DEFAULT_ROW_HEIGHT);
            }
            else if (section == SECTION_ALARM_PRIORITY)
            {
                return QSize(averageW, DEFAULT_ROW_HEIGHT);
            }
        }
        case Qt::BackgroundColorRole:
            return themeManger.getColor(ThemeManager::ControlTypeNR,
                                        ThemeManager::ElementBackgound,
                                        ThemeManager::StateDisabled);
        default:
            break;
        }
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

void AlarmInfoModel::setStringList(const QStringList &nameList, const QStringList &timeList, const QStringList &priorityList)
{
    if (d_ptr->nameList == nameList && d_ptr->timeList == timeList)
    {
        return;
    }

    beginResetModel();
    d_ptr->nameList = nameList;
    d_ptr->timeList = timeList;
    d_ptr->iconList.clear();
    for (int i = 0; i < d_ptr->nameList.count(); i++)
    {
        d_ptr->iconList.append(QIcon());
    }
    d_ptr->prorityList = priorityList;
    endResetModel();
}

int AlarmInfoModel::getRowHeightHint() const
{
    return DEFAULT_ROW_HEIGHT;
}

int AlarmInfoModel::getRowEachPage()
{
    return EACH_PAGE_ALARM_COUNT;
}

Qt::ItemFlags AlarmInfoModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;
    if (index.row() < d_ptr->nameList.count())
    {
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    return flags;
}

bool AlarmInfoModel::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj->isWidgetType() && ev->type() == QEvent::Resize)
    {
        QResizeEvent *re = static_cast<QResizeEvent *>(ev);
        d_ptr->viewWidth = re->size().width();
        emit headerDataChanged(Qt::Horizontal, 0, SECTION_NR);
    }
    return QAbstractTableModel::eventFilter(obj, ev);
}
