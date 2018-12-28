/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/9
 **/

#pragma once
#include <QAbstractTableModel>
#include <QScopedPointer>

class ErrorLogTableModelPrivate;
class ErrorLogTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ErrorLogTableModel(QObject *parent = NULL);
    ~ErrorLogTableModel();

    /* reimplement */
    int columnCount(const QModelIndex &parent) const;

    /* reimplement */
    int rowCount(const QModelIndex &parent) const;

    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;

    /* reimplement */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /* reimplement */
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /**
     * @brief getHeaderHeightHint 获取列头高度
     * @return
     */
    int getHeaderHeightHint();

    /**
     * @brief getRowHeightHint 获取行高
     * @return
     */
    int getRowHeightHint();

    /**
     * @brief loadData 载入错误日志数据
     */
    void loadData(void);

    /**
     * @brief getErrorLogIndex 获取行号在数据中的索引位置
     * @param index 行号
     * @return
     */
    int getErrorLogIndex(int index);

signals:
    void pageInfoUpdate(int curPage, int totalPage);

private slots:
    void upBtnReleased(void);
    void downBtnReleased(void);

private:
    QScopedPointer<ErrorLogTableModelPrivate> d_ptr;
};
