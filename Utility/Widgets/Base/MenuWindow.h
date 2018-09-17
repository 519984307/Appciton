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
#include <QVariant>

class MenuWindowPrivate;
class MenuWindow : public Window
{
    Q_OBJECT
public:
    MenuWindow();

    ~MenuWindow();

    /**
     * @brief addMenuContent add a menu to the window
     * @param menu menu content obj
     */
    void addMenuContent(MenuContent *menu);

    void popup(const QString &menuName, const QVariant &param = QVariant());

    /**
     * @brief setMenuPath  设置当前菜单路径
     * @param path
     */
    void setMenuPath(const QString &path);

protected:
    bool focusNextPrevChild(bool next);
    void showEvent(QShowEvent *ev);

    /* reimpelment */
    void paintEvent(QPaintEvent *ev);

private slots:
    void onSelectItemChanged(int index);
    void onReturnBtnClick();

private:
    QScopedPointer<MenuWindowPrivate> d_ptr;
};
