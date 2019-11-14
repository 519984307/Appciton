/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/6/29
 **/


#pragma once
#include <QWidget>
#include "ScrollArea.h"

class MenuSidebarPrivate;
class MenuSidebarItem;
class MenuSidebar : public ScrollArea
{
    Q_OBJECT
public:
    explicit MenuSidebar(QWidget *parent = NULL);
    ~MenuSidebar();

    /**
     * @brief addItem add a string item
     * @param text the string
     */
    void addItem(const QString &text);

    /**
     * @brief itemCount get the number of the items
     */
    int itemCount() const;

    /**
     * @brief setChecked check the string item
     * @param text the string item text
     * @return true if the string item exists
     */
    bool setChecked(const QString &text);

    /**
     * @brief getChecked get get focus item
     * @return  the focus item
     */
    MenuSidebarItem *getChecked() const;

    /**
     * @brief indexOf get the index of the item
     * @param item the menu item
     * @return index of the item or -1 if the item is not exist
     */
    int indexOf(MenuSidebarItem *item) const;

    /**
     * @brief itemAt get the item at specific index
     * @param index the item index
     * @return pointer to the item or NULL if the index is invalid
     */
    MenuSidebarItem *itemAt(int index) const;

    /* reimplement */
    QSize sizeHint() const;

signals:
    void selectItemChanged(int index);
    void visiableItemChanged(int index);

protected:
    void showEvent(QShowEvent *ev);

private:
    Q_PRIVATE_SLOT(d_func(), void onItemClicked())
    Q_PRIVATE_SLOT(d_func(), void onItemFocusChanged(bool, Qt::FocusReason))
    Q_DECLARE_PRIVATE(MenuSidebar)
};

