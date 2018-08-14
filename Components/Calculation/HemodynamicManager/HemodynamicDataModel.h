/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/7
 **/

#pragma once
#include <QAbstractTableModel>
#include "HemodynamicDefine.h"
#include "HemodynamicReviewWindow.h"

enum
{
    COLUMN_VALUE_TYPE = 0,
    COLUMN_UNIT_RANGE,
    COLUMN_TIME_1,
    COLUMN_TIME_2,
    COLUMN_TIME_3,
    COLUMN_TIME_4,
    COLUMN_NUM_NR
};

class HemodynamicDataModelPrivate;
class HemodynamicDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit HemodynamicDataModel(QObject *parent = NULL);
    ~HemodynamicDataModel();

    /* reimplement */
    int columnCount(const QModelIndex &parent) const;

    /* reimplement */
    int rowCount(const QModelIndex &parent) const;

    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;

    /* reimplement */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /**
     * @brief setDynamicInfos
     * @param dataInfos
     */
    void setDynamicInfos(const QList<ReviewDataInfo> &dataInfos);
    /**
     * @brief setDynamicColorInfos
     * @param colorInfos
     */
    void setDynamicColorInfos(const QList<ReviewDataInfo> &colorInfos);
    /**
     * @brief setHeadInfos
     * @param dataInfos
     */
    void setHeadInfos(const ReviewDataInfo &dataInfos);
    /**
     * @brief getRowHeightHint
     * @return
     */
    int getRowHeightHint() const;
    /**
     * @brief getHeaderHeightHint
     * @return
     */
    int getHeaderHeightHint() const;
    /**
     * @brief getRowCount
     * @return
     */
    int getRowCount() const;
private:
    HemodynamicDataModelPrivate *const d_ptr;
};
