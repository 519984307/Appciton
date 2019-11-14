/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/20
 **/

#pragma once
#include "qglobal.h"
#include <QFlags>

class Dialog;
class WindowManagerInterface
{
public:
    enum ShowBehaviorFlags
    {
        ShowBehaviorNone = 0x00,            // no any special behavior
        // close the window that request to show and remove it from the window stack
        // when the window is on the top of the window stack
        ShowBehaviorCloseIfVisiable = 0x01,
        // Show the window as modal window
        ShowBehaviorModal = 0x02,
        // show the window and hide others window, the other windows still in the window stack
        ShowBehaviorHideOthers = 0x04,
        // show the window and close others window, the other windows will remove from
        // the window stack
        ShowBehaviorCloseOthers = 0x08,
        // don't automatically close this window when there isn't any user action
        ShowBehaviorNoAutoClose = 0x10,
    };

    Q_DECLARE_FLAGS(ShowBehavior, ShowBehaviorFlags)

    virtual ~WindowManagerInterface(){}

    static WindowManagerInterface *registerWindowManager(WindowManagerInterface *instance);

    static WindowManagerInterface *getWindowManager(void);

    /**
     * @brief showWindow
     * @param w
     * @param behaviors
     */
    virtual void showWindow(Dialog *w, ShowBehavior behaviors = ShowBehaviorNone) = 0;

    /**
     * @brief topWindow get the top window
     * @return the top window or NULL if the not top window
     */
    virtual Dialog *topWindow() = 0;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowManagerInterface::ShowBehavior)
