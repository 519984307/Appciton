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

    /* reimplement */
    int columnCount(const QModelIndex &parent) const;
    /* reimplement */
    int rowCount(const QModelIndex &parent) const;
    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;
    /* reimplement */
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /* reimplement */
    QSize span(const QModelIndex &index) const;

    ~ScreenLayoutModel();

private:
    ScreenLayoutModelPrivate * const d_ptr;
};
