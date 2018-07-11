/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#pragma once

#include <QAbstractTableModel>
#include "AlarmConfig.h"
#include <QList>

struct AlarmDataInfo
{
    ParamID paramID;
    SubParamID subParamID;
    LimitAlarmConfig limitConfig;
    char alarmLevel;     // 0:low, 1:medium or 2:high
    bool status;        // on or off
};

class AlarmLimitModelPrivate;
class AlarmLimitModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AlarmLimitModel(QObject *parent = NULL);
    ~AlarmLimitModel();

    /* reimplement */
    int columnCount(const QModelIndex &parent) const;

    /* reimplement */
    int rowCount(const QModelIndex &parent) const;

    /* reimplement */
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;

    /* reimplement */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /* reimplement */
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /**
     * @brief setupAlarmDataInfos setup the alarm data infos
     * @param dataInfos alarm data info list
     */
    void setupAlarmDataInfos(const QList<AlarmDataInfo> &dataInfos);

    /* reimplement */
    bool eventFilter(QObject *obj, QEvent *ev);

    /**
     * @brief getRowHeight get the row height hint
     * @return row height hint
     */
    int getRowHeightHint() const;

    /**
     * @brief getHeaderHeightHint get the header height hint
     * @return header height hint
     */
    int getHeaderHeightHint() const;

private:
    AlarmLimitModelPrivate *const d_ptr;
};


