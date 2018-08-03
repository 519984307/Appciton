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
class MenuSideBarItem;
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
     * @brief setChecked check the string item
     * @param text the string item text
     * @return true if the string item exists
     */
    bool setChecked(const QString &text);

    /**
     * @brief getChecked get get focus item
     * @return  the focus item
     */
    QWidget *getChecked() const;

    void popupWidget(const QString &text);


    /* reimplement */
    QSize sizeHint() const;

signals:
    void selectItemChanged(const QString &text);
    void selectItemChanged(int index);
    void itemClicked(int index);

protected:
    void showEvent(QShowEvent *ev);

private:
    Q_PRIVATE_SLOT(d_func(), void onItemClicked())
    Q_PRIVATE_SLOT(d_func(), void onItemFocusChanged(bool))
    Q_DECLARE_PRIVATE(MenuSidebar)
};

