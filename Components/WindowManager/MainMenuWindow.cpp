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
#include "NormalFunctionMenuContent.h"
#include "LoadConfigMenuContent.h"
#include "ScreenMenuContent.h"
#include "DataReviewMenuContent.h"
#include "SystemMaintenanceMenuContent.h"
#include "PrintSettingMenuContent.h"
#include "MeasureSettingMenuContent.h"
#include "PatientManagementMenuContent.h"

MainMenuWindow *MainMenuWindow::getInstance()
{
    static MainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new MainMenuWindow;

        // initialize the window content
        instance->addMenuContent(new PatientManagementMenuContent);
        instance->addMenuContent(new AlarmLimitMenuContent);
        instance->addMenuContent(new DataReviewMenuContent);
        instance->addMenuContent(new ScreenMenuContent);
        if (systemManager.isSupport(CONFIG_PRINTER))
        {
            instance->addMenuContent(new PrintSettingMenuContent);
        }

        instance->addMenuContent(new MeasureSettingMenuContent);
        instance->addMenuContent(new SystemMaintenanceMenuContent);
        instance->addMenuContent(new LoadConfigMenuContent);
        instance->addMenuContent(new NormalFunctionMenuContent);
    }

    return instance;
}

MainMenuWindow::MainMenuWindow()
    : MenuWindow()
{
}
