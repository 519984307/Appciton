/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/18
 **/

#pragma once
#include <QAbstractTableModel>

class AlarmInfoModelPrivate;
class AlarmInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AlarmInfoModel(QObject *parent = NULL);
    ~AlarmInfoModel();

    /* reimplement */
    int rowCount(const QModelIndex &parent) const;

    /* reimplement */
    int columnCount(const QModelIndex &parent) const;

    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;

    /**
     * @brief setStringList set the string list
     * @param list the string list
     */
    void setStringList(const QStringList &nameList, const QStringList &timeList);

    /**
     * @brief getRowHeightHint get the row height hint
     * @return the row height hint
     */
    int getRowHeightHint() const;
private:
    AlarmInfoModelPrivate * const d_ptr;
};
