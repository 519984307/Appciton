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
#include <QListView>

class ListsViewPrivate;
class ListView : public QListView
{
    Q_OBJECT
public:
    explicit ListView(QWidget *parent = NULL);
    ~ListView();

    /**
     * @brief curCheckedRow get the checked row
     * @return  the checked row or -1 if not row is checked
     */
    int curCheckedRow() const;

    /**
     * @brief clearCheckRow clear the checked row
     */
    void clearCheckRow();

    /* reimplement */
    void setModel(QAbstractItemModel *model);

protected:
    /* reimplement */
    void keyPressEvent(QKeyEvent *ev);
    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);
    /* reimplement */
    void paintEvent(QPaintEvent *ev);
    /* reimplement */
    void focusInEvent(QFocusEvent *ev);
    /* reimplement */
    void focusOutEvent(QFocusEvent *ev);
    /* reimplement */
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    /* reimplement */
    void rowsInserted(const QModelIndex &parent, int start, int end);

private slots:
    void onRowClicked(QModelIndex index);

private:
    ListsViewPrivate * const d_ptr;
};
