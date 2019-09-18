/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/1
 **/

#pragma once
#include <QAbstractTableModel>
#include <QScopedPointer>

class EventReviewModelPrivate;
class EventReviewModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit EventReviewModel(QObject *parent = NULL);
    ~EventReviewModel();

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

    /* reimplement */
    bool eventFilter(QObject *obj, QEvent *ev);

    void updateEvent(QList<QString> &timeList, QList<QString> &eventList);

    /**
     * @brief setPageRowCount 设置每一页显示的行数
     * @param count
     */
    void setPageRowCount(int count);

    /**
     * @brief getHeightHint 获取每一行高度
     * @return
     */
    int getHeightHint() const;

private:
    QScopedPointer<EventReviewModelPrivate> d_ptr;
};
