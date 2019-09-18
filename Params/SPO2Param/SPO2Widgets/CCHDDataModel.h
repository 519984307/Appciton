/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/13
 **/

#pragma once
#include <QAbstractTableModel>
#include "SPO2Param.h"

class CCHDDataModelPrivate;
class CCHDDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CCHDDataModel(QObject *parent = NULL);
    ~CCHDDataModel();

    /*reimplement*/
    int rowCount(const QModelIndex &parent) const;

    /*reimplement*/
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /**
     * @brief getHeightEachRow get height each row
     * @return
     */
    int getHeightEachRow();

    /**
     * @brief addData 增加数据
     */
    void updateData();

    /**
     * @brief setTableViewWidth
     * @param width
     */
    void setTableViewWidth(int width);


private:
    CCHDDataModelPrivate * const d_ptr;
};
