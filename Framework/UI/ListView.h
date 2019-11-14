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
#include <QMap>

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

    /**
     * @brief clearAllCheckedRows clear all the checked row
     */
    void clearAllCheckedRows();

    /**
     * @brief setRowsSelectMode set rows select mode
     * @param isMultiMode false:single mode  true:multi mode
     */
    void setRowsSelectMode(bool isMultiMode = false);

    /**
     * @brief getRowsSelectMap get rows selected map
     * @return
     */
    QMap<int, bool> getRowsSelectMap() const;

    /* reimplement */
    void setModel(QAbstractItemModel *model);

    /**
     * @brief setDrawIcon   设置是否绘画选择图标
     * @param isDrawIcon
     */
    void setDrawIcon(bool isDrawIcon);

    /**
     * @brief setClickable set whether the item is clickable
     * @param clickable
     * @note the item is clicable by default
     */
    void setClickable(bool clickable);

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
    /* reimplement */
    void hideEvent(QHideEvent *ev);

signals:
    void enterSignal(void);

private slots:
    void onRowClicked(QModelIndex index);

private:
    ListsViewPrivate * const d_ptr;
};
