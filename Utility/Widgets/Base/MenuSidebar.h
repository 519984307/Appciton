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
class MenuSidebar : public ScrollArea
{
    Q_OBJECT
public:
    explicit MenuSidebar(QWidget *parent = NULL);
    ~MenuSidebar();

    /* add a string item */
    void addItem(const QString &text);

    /* reimplement */
    QSize sizeHint() const;

signals:
    void selectItemChanged(const QString &text);

protected:
    void showEvent(QShowEvent *ev);

private:
    Q_PRIVATE_SLOT(d_func(), void onItemClicked())
    Q_PRIVATE_SLOT(d_func(), void onItemFocusChanged(bool))
    Q_DECLARE_PRIVATE(MenuSidebar)
};

