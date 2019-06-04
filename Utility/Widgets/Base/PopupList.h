/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/

#pragma once
#include <QWidget>

class PopupListPrivate;
class PopupList : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief PopupList
     * @param concatToParent
     * @param parent
     */
    explicit PopupList(QWidget *parent = NULL, bool concatToParent = true);

    ~PopupList();

    /**
     * @brief addItemText add a popup list item with the @text
     * @param text item text
     */
    void addItemText(const QString &text);

    /**
     * @brief additemList add a list of item
     * @param list the item text list
     */
    void additemList(const QStringList &list);

    /**
     * @brief setCurrentIndex set the selected item, negative value meams uncheck
     * @param index
     */
    void setCurrentIndex(int index);

    /**
     * @brief setCurrentText set the current item to the item that has the @text
     * @param text the text of the item that need to set as current item
     */
    void setCurrentText(const QString &text);

    /**
     * @brief getCurrentIndex get he current selected item
     * @return get the selected items, negative value means no selected item
     */
    int getCurrentIndex() const;

    /**
     * @brief setPopAroundRect set a gloable rect that the popup should pop around with
     * @note It's better to set this property when the popup list has not parent
     * @param rect
     */
    void setPopAroundRect(const QRect &rect);

    /**
     * @brief setMaximumDisplayItem set the maximum display item number in one page
     * @param num the maximum number
     */
    void setMaximumDisplayItem(int num);

    int count() const;

    /* reimplement */
    QSize sizeHint() const;

protected:
    /* reimplement */
    void showEvent(QShowEvent *e);
    /* reimplement */
    void paintEvent(QPaintEvent *e);
    /* reimplement */
    void resizeEvent(QResizeEvent *e);

    /* reimplement */
    void keyPressEvent(QKeyEvent *e);
    /* reimplement */
    void keyReleaseEvent(QKeyEvent *e);

    /* reimplement */
    bool focusNextPrevChild(bool next);

signals:
    void selectItemChanged(int index);
    void selectItemChanged(const QString &text);

    /**
     * @brief itemFocusChanged
     * @param value
     */
    void itemFocusChanged(int value);
    void itemFoucsIndexChanged(int index);

private slots:
    void onItemSelected();

private:
    QScopedPointer<PopupListPrivate> d_ptr;
};
