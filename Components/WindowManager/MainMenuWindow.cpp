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
#include "ECGMenuContent.h"
#include "RESPMenuContent.h"
#include "AGMenuContent.h"
#include "IBPMenuContent.h"
#include "COMenuContent.h"
#include "SPO2MenuContent.h"
#include "AlarmLimitMenuContent.h"
#include "NIBPMenuContent.h"
#include "CO2MenuContent.h"
#include "DisplayMenuContent.h"
#include "WiFiProfileMenuContent.h"
#include "SystemMenuContent.h"
#include "LoadConfigMenuContent.h"
#include "TEMPMenu.h"
#include "ScreenMenuContent.h"
#include "SystemNightModeMenuContent.h"
#include "DataReviewMenuContent.h"
#include "SystemMaintenance.h"
#include "CalculateMenuContent.h"
#include "PrintSettingMenuContent.h"

MainMenuWindow *MainMenuWindow::getInstance()
{
    static MainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new MainMenuWindow;

        // initialize the window content
        instance->addMenuContent(new ECGMenuContent);
        instance->addMenuContent(new RESPMenuContent);
        instance->addMenuContent(new TEMPMenu);
        if (systemManager.isSupport(CONFIG_AG))
        {
            instance->addMenuContent(new AGMenuContent);
        }
        if (systemManager.isSupport(CONFIG_IBP))
        {
            instance->addMenuContent(new IBPMenuContent);
        }
        if (systemManager.isSupport(CONFIG_CO))
        {
            instance->addMenuContent(new COMenuContent);
        }
        if (systemManager.isSupport(CONFIG_SPO2))
        {
            instance->addMenuContent(new SPO2MenuContent);
        }
        if (systemManager.isSupport(CONFIG_NIBP))
        {
            instance->addMenuContent(new NIBPMenuContent);
        }
        if (systemManager.isSupport(CONFIG_CO2))
        {
            instance->addMenuContent(new CO2MenuContent);
        }
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
        instance->addMenuContent(new PrintSettingMenuContent);
        instance->addMenuContent(new SystemMaintenance);
        instance->addMenuContent(new SystemMenuContent);
        instance->setWindowTitle(trs("ECGMenuDesc"));
    }

    return instance;
}

MainMenuWindow::MainMenuWindow()
    : MenuWindow()
{
}
