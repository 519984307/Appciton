/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/18
 **/

#pragma once
#include <QAbstractListModel>

class ListDataModelPrivate;

class ListDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ListDataModel(QObject *parent = NULL);
    ~ListDataModel();

    /* reimplement */
    int rowCount(const QModelIndex &parent) const;

    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;

    /* reimplement */
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    /**
     * @brief setStringList set the string list
     * @param list the string list
     */
    void setStringList(const QStringList &list);

    /**
     * @brief getStringList get the string list
     * @return return the internal string list
     */
    QStringList getStringList() const;

    /**
     * @brief getRowHeightHint get the row height hint
     * @return the row height hint
     */
    int getRowHeightHint() const;

    /**
     * @brief getRowCount  get row count
     * @return
     */
    int getRowCount() const;
private:
    ListDataModelPrivate * const d_ptr;
};
