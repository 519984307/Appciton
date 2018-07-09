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
#include "Window.h"
#include "MenuContent.h"

class MenuWindowPrivate;
class MenuWindow : public Window
{
    Q_OBJECT
public:
    MenuWindow();
    ~MenuWindow();

    void addMenuGroup(const QString &title, QWidget *w = NULL);

    void addMenuContent(MenuContent *menu);

private slots:
    void onSelectItemChanged(int index);

private:
    QScopedPointer<MenuWindowPrivate> d_ptr;
};
