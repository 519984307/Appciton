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

enum
{
    SECTION_PARAM_NAME,
    SECTION_STATUS,
    SECTION_HIGH_LIMIT,
    SECTION_LOW_LIMIT,
    SECTION_LEVEL,
    SECTION_NR
};

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
     * @param updateImmediately 是否立即更新报警信息
     */
    void setupAlarmDataInfos(const QList<AlarmDataInfo> &dataInfos, bool updateImmediately = true);

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

    /**
     * @brief editRowData edit the row data
     * @param row row
     */
    void editRowData(int row);

    /**
     * @brief stopEditRow stop edit row data
     */
    void stopEditRow();

    /**
     * @brief curEditRow get the current edit row
     * @return the current edit row, -1 means not row is under edited
     */
    int curEditRow() const;

    /**
     * @brief setEachPageRowCount 设置每页显示多少行
     * @param rows
     */
    void setEachPageRowCount(int rows);

    /**
     * @brief editableParam  Whether the parameter alarm limit can be edited
     * @param editable
     */
    void editableParam(ParamID pid, bool editable);
protected:
    /**
     * @brief alarmDataUpdate callback when specific alarm config changed
     * @param info the change alarm info
     */
    virtual void alarmDataUpdate(const AlarmDataInfo & /*info*/, int /*type*/);

private:
    AlarmLimitModelPrivate *const d_ptr;
};


