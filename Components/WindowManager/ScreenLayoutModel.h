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
     * @brief loadLayoutInfo 从配置表中获取配置信息
     * @param isDefaultConfig 是否从默认配置表中获取信息，
     *                        true: 从默认配置表（system.original.xml）中获取信息
     *                        false: 从系统配置表（system.xml）中获取信息
     */
    void loadLayoutInfo(bool isDefaultConfig = false);

    /**
     * @brief updateInfo update the param and wave info
     */
    void updateWaveAndParamInfo();

signals:
    /**
     * @brief spanChanged emit when the span changed
     * @param index the item's index
     */
    void spanChanged(const QModelIndex &index);

private:
    ScreenLayoutModelPrivate * const d_ptr;
};
