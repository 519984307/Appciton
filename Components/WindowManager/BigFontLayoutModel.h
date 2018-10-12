/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/26
 **/

#pragma once
#include <QAbstractTableModel>

class BigFontLayoutModelPrivate;
class BigFontLayoutModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BigFontLayoutModel(QObject *parent = NULL);
    ~BigFontLayoutModel();

    /*reimplement*/
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /*reimplement*/
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*reimplement*/
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    /*reimplement*/
    QVariant data(const QModelIndex &index, int role) const;

    /*reimplement*/
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /*reimplement*/
    QSize span(const QModelIndex &index) const;

    /**
     * @brief saveLayoutInfo 保存布局信息到配置表中
     */
    void saveLayoutInfo();

    /**
     * @brief loadLayoutInfo 从配置表中加载布局信息
     */
    void loadLayoutInfo();

    /**
     * @brief updateWaveAndParamInfo  更新波形和参数信息
     */
    void updateWaveAndParamInfo();

signals:
    /**
     * @brief spanChanged 发送合并格信号
     * @param index item的序号
     */
    void spanChanged(const QModelIndex &index);

private:
    BigFontLayoutModelPrivate * const d_ptr;
};
