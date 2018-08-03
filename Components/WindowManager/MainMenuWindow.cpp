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
#include "ConfigManagerMenuContent.h"
#include "UserMaintainMenuContent.h"
#include "FactoryMaintainMenuContent.h"

MainMenuWindow *MainMenuWindow::getInstance()
{
    static MainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new MainMenuWindow;

        // initialize the window content
        instance->addMenuContent(new ECGMenuContent);
        instance->addMenuContent(RESPMenuContent::getInstance());
        if (systemManager.isSupport(CONFIG_AG))
        {
            instance->addMenuContent(AGMenuContent::getInstace());
        }
        if (systemManager.isSupport(CONFIG_IBP))
        {
            instance->addMenuContent(new IBPMenuContent);
        }
        if (systemManager.isSupport(CONFIG_CO))
        {
            instance->addMenuContent(COMenuContent::getInstance());
        }
        if (systemManager.isSupport(CONFIG_SPO2))
        {
            instance->addMenuContent(SPO2MenuContent::getInstance());
        }
        if (systemManager.isSupport(CONFIG_NIBP))
        {
            instance->addMenuContent(NIBPMenuContent::getInstance());
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
        instance->addMenuContent(new LoadConfigMenuContent);
        instance->addMenuContent(new SystemMenuContent);
        instance->addMenuContent(new ConfigManagerMenuContent);
        instance->addMenuContent(new UserMaintainMenuContent);
        instance->addMenuContent(new FactoryMaintainMenuContent);
    }

    return instance;
}

MainMenuWindow::MainMenuWindow()
    : MenuWindow()
{
}
