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

#include "Framework/UI/MenuWindow.h"

/**
 * @brief The MainMenuWindow class
 *        This is the main menu window, this window should be shown when the
 *        user click the main menu key. The applicantion can create only one
 *        main menu window during the applicantiion life cycle.
 *
 *        Main menu window provider setting menu for the parameters and system.
 *        In the meantime, main window also provider entrance for the configuration
 *        menu, user maintance menu, factory maintance menu.
 */
class MainMenuWindow : public MenuWindow
{
    Q_OBJECT
public:
    /**
     * @brief getInstance get the instance of the Main Menu window
     * @return
     */
    static MainMenuWindow *getInstance();

private:
    MainMenuWindow();
};
