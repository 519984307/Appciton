/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/12
 **/

#include "MainMenuWindow.h"
#include "SystemManager.h"
#include "AlarmLimitMenuContent.h"
#include "DisplayMenuContent.h"
#include "WiFiProfileMenuContent.h"
#include "SystemMenuContent.h"
#include "LoadConfigMenuContent.h"
#include "ScreenMenuContent.h"
#include "SystemNightModeMenuContent.h"
#include "DataReviewMenuContent.h"
#include "SystemMaintenance.h"
#include "CalculateMenuContent.h"
#include "MeasureSettingMenuContent.h"

MainMenuWindow *MainMenuWindow::getInstance()
{
    static MainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new MainMenuWindow;

        // initialize the window content
        instance->addMenuContent(new AlarmLimitMenuContent);
        if (systemManager.isSupport(CONFIG_WIFI))
        {
            instance->addMenuContent(new WiFiProfileMenuContent);
        }
        instance->addMenuContent(new DisplayMenuContent);
        instance->addMenuContent(new SystemNightModeMenuContent);
        instance->addMenuContent(new ScreenMenuContent);
        instance->addMenuContent(new LoadConfigMenuContent);
        instance->addMenuContent(new DataReviewMenuContent);
        instance->addMenuContent(new CalculateMenuContent);
        instance->addMenuContent(new MeasureSettingMenuContent);
        instance->addMenuContent(new SystemMaintenance);
        instance->addMenuContent(new SystemMenuContent);
    }

    return instance;
}

MainMenuWindow::MainMenuWindow()
    : MenuWindow()
{
}
