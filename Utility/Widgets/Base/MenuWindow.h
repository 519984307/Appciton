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
#include "Dialog.h"
#include "MenuContent.h"
#include <QVariant>
#include "WindowManager.h"

class MenuSidebar;
class MenuWindowPrivate;
class MenuWindow : public Dialog
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

    /**
     * @brief popup  弹出菜单
     * @param menuName  弹出菜单的名字
     * @param param  传递给菜单的参数
     * @param showFlags 菜单显示行为
     */
    void popup(const QString &menuName, const QVariant &param = QVariant(),
               const WindowManager::ShowBehaviorFlags &showFlags = WindowManager::ShowBehaviorCloseOthers);

    /**
     * @brief setWindowTitlePrefix
     * @param prefix  窗口标题前缀
     */
    void setWindowTitlePrefix(const QString &prefix);

    /**
     * @brief focusFirstMenuItem  设置聚焦点为菜单栏第一个item
     */
    void focusFirstMenuItem();

    /**
     * @brief ensureWidgetVisiable ensure the widget in the window visiable
     * @param w the pointer to the window
     */
    void ensureWidgetVisiable(QWidget *w);

protected:
    bool focusNextPrevChild(bool next);
    void showEvent(QShowEvent *ev);

private slots:
    void onSelectItemChanged(int index);
    void onVisiableItemChanged(int index);
    void onReturnBtnClick();

private:
    QScopedPointer<MenuWindowPrivate> d_ptr;
};
