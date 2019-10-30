/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/12
 **/

#pragma once
#include <QMap>
#include <QWidget>
#include <QList>
#include "WaveWidget.h"
#include "SystemDefine.h"
#include "WindowManagerInterface.h"

/***************************************************************************************************
 * 窗体管理器: 负责管理所有窗体布局
 **************************************************************************************************/
class IWidget;
class SoftKeyManager;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class Dialog;
class WindowManagerPrivate;
class WindowManager : public QWidget, public WindowManagerInterface
{
    Q_OBJECT
public:
    static WindowManager &getInstance(void);
    ~WindowManager();


    /**
     * @brief showWindow show a window and push the window to the window stack
     * @param w the window need to show
     * @param behaviors the behavior need to perform when showing the window
     */
    void showWindow(Dialog *w, ShowBehavior behaviors = ShowBehaviorNone);

    /**
     * @brief topWindow get the top window
     * @return the top window or NULL if the not top window
     */
    Dialog *topWindow();

    /**
     * @brief showDemoWidget show or hide the demo widget
     * @param flag true if need to show the widget
     *             or false to hide the demo widget
     */
    void showDemoWidget(bool flag);

    /**
     * @brief eventFilter event filer to filter the user input event,
     *                    this filer is use to setup on the qApp and
     *                    reset the auto close timer when not any user action
     * @param obj
     * @param ev
     * @return always false
     */
    bool eventFilter(QObject *obj, QEvent *ev);

public slots:
    /**
     * @brief closeAllWidows close all the windows
     */
    void closeAllWidows();

    /**
     * @brief onWindowHide handle the windows hide signal in the window stack
     * @param w
     */
    void onWindowHide(Dialog *w);

public:
    // 获取弹出菜单宽度, TODO: remove
    int getPopWindowWidth();

    // 获取弹出菜单高度, TODO: remove
    int getPopWindowHeight();

private:
    WindowManager();

private slots:
    void onLayoutChanged();

private:
    QMap<QString, IWidget *> _winMap;      // 保存所有的窗体。

    WindowManagerPrivate * const d_ptr;
};
#define windowManager (WindowManager::getInstance())
