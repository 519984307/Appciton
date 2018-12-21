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
#include "MenuManager.h"

/***************************************************************************************************
 * 窗体管理器: 负责管理所有窗体布局
 **************************************************************************************************/
class IWidget;
class SoftKeyManager;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class Window;
class WindowManagerPrivate;
class WindowManager : public QWidget
{
    Q_OBJECT
public:
    enum ShowBehaviorFlags
    {
        ShowBehaviorNone = 0x00,            // no any special behavior
        ShowBehaviorCloseIfVisiable = 0x01, // close the window that request to show and remove it from the window stack
                                            // when the window is on the top of the window stack

        ShowBehaviorModal = 0x02,           // Show the window as modal window
        ShowBehaviorHideOthers = 0x04,      // show the window and hide others window, the other windows still in the window stack
        ShowBehaviorCloseOthers = 0x08,     // show the window and close others window, the other windows will remove from the window stack
        ShowBehaviorNoAutoClose = 0x10,     // don't automatically close this window when there isn't any user action
    };
    Q_DECLARE_FLAGS(ShowBehavior, ShowBehaviorFlags)

    static WindowManager &getInstance(void);
    ~WindowManager();


    /**
     * @brief showWindow show a window and push the window to the window stack
     * @param w the window need to show
     * @param behaviors the behavior need to perform when showing the window
     */
    void showWindow(Window *w, ShowBehavior behaviors = ShowBehaviorNone);

    /**
     * @brief topWindow get the top window
     * @return the top window or NULL if the not top window
     */
    Window *topWindow();

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
    void onWindowHide(Window *w);

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
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowManager::ShowBehavior)
