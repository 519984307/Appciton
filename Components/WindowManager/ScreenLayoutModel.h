/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/29
 **/

#pragma once
#include <QAbstractTableModel>

class ScreenLayoutModelPrivate;
class ScreenLayoutModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ScreenLayoutModel(QObject *parent = NULL);
    ~ScreenLayoutModel();

    /* reimplement */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    /* reimplement */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    /* reimplement */
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;
    /* reimplement */
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /* reimplement */
    QSize span(const QModelIndex &index) const;

    /**
     * @brief saveLayoutInfo save the layout info to the config file
     */
    void saveLayoutInfo();

    /**
     * @brief loadLayoutInfo load the layout info from the config file
     */
    void loadLayoutInfo();

    /**
     * @brief updateInfo update the param and wave info
     */
    void updateWaveAndParamInfo();

private:
    ScreenLayoutModelPrivate * const d_ptr;
};
