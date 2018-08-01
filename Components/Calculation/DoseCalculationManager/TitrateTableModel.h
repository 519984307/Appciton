/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/31
 **/
#pragma once
#include <QAbstractTableModel>

enum
{
    COLUMN_DOSE1 = 0,
    COLUMN_RATE1,
    COLUMN_DOSE2,
    COLUMN_RATE2,
    COLUMN_DOSE3,
    COLUMN_RATE3,
    COLUMN_NR
};

struct TitrateTableInfo
{
    QString doseValue;
    QString rateVlue;
};

class TitrateTableModelPrivate;
class TitrateTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TitrateTableModel(QObject *parent = NULL);
    ~TitrateTableModel();

    /* reimplement */
    int columnCount(const QModelIndex &parent) const;

    /* reimplement */
    int rowCount(const QModelIndex &parent) const;

    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;

    /* reimplement */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /* reimplement */
    bool eventFilter(QObject *, QEvent *);

    /**
     * @brief setupTitrateTableInfos
     * @param dataInfos
     */
    void setupTitrateTableInfos(const QList<TitrateTableInfo> &dataInfos);
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

private:
    TitrateTableModelPrivate *const d_ptr;
};
