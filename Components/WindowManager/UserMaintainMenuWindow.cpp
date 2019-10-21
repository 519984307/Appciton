/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "UserMaintainMenuWindow.h"
#include "UserMaintainGeneralMenuContent.h"
#include "ModuleMaintainMenuContent.h"
#include "AlarmMaintainMenuContent.h"
#include "WifiMaintainMenuContent.h"
#include "WiredNetworkMaintainMenuContent.h"
#include "OthersMaintainMenuContent.h"
#include "UnitSetupMenuContent.h"
#include "Supervisor12LMenuContent.h"
#include "SupervisorPrintMenuContent.h"
#include "ErrorLogEntranceContent.h"
#include "DemoMenuContent.h"
#include "LanguageManager.h"
#include "SystemManager.h"

UserMaintainMenuWindow *UserMaintainMenuWindow::getInstance()
{
    static UserMaintainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new UserMaintainMenuWindow;

        instance->addMenuContent(new UserMaintainGeneralMenuContent);
        instance->addMenuContent(new ModuleMaintainMenuContent);
        instance->addMenuContent(new AlarmMaintainMenuContent);
        if (systemManager.isSupport(CONFIG_WIFI))
        {
            instance->addMenuContent(new WifiMaintainMenuContent);
        }
#ifndef HIDE_WIRED_NETWORK_FUNCTION
        instance->addMenuContent(new WiredNetworkMaintainMenuContent);
#endif
        instance->addMenuContent(new OthersMaintainMenuContent);
        instance->addMenuContent(new UnitSetupMenuContent);
        instance->addMenuContent(new ErrorLogEntranceContent);
#ifndef HIDE_ECG_12_LEAD_FUNCTION
        instance->addMenuContent(new Supervisor12LMenuContent);
#endif
//        instance->addMenuContent(new DemoMenuContent);
        instance->setWindowTitle(trs("UserMaintainGeneralMenuDesc"));
    }

    return instance;
}

UserMaintainMenuWindow::UserMaintainMenuWindow()
{
}
