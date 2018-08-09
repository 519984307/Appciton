/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/7
 **/

#pragma once
#include <QAbstractTableModel>
#include <QScopedPointer>

class HistoryDataSelModelPrivate;
class HistoryDataSelModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit HistoryDataSelModel(QObject *parent = NULL);
    ~HistoryDataSelModel();

    /* reimplement */
    int columnCount(const QModelIndex &parent) const;

    /* reimplement */
    int rowCount(const QModelIndex &parent) const;

    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;

    /* reimplement */
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /**
     * @brief getRowHeightHint 获取行高
     * @return
     */
    int getRowHeightHint(void);

    /**
     * @brief updateData 更新载入历史数据文件名
     */
    void updateData(void);

    /**
     * @brief getDateTimeStr 获取转换后的日期字符串
     * @param index 指定索引
     * @return
     */
    QString getDateTimeStr(int index);
private:
    QScopedPointer<HistoryDataSelModelPrivate> d_ptr;
};
