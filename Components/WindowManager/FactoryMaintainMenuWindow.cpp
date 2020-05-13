/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "FactoryMaintainMenuWindow.h"
#include "MachineConfigModuleContent.h"
#include "FactoryVersionInfo.h"
#include "FactoryDataRecordContent.h"
#include "FactorySystemInfoMenuContent.h"
#include "FactoryImportExportMenuContent.h"
#include "ServiceUpdateEntranceContent.h"
#include "NIBPCalibrationMenuContent.h"
#include "FactoryTestMenuContent.h"
#include "FactoryTempMenuContent.h"
#include "O2CalibrationMenuContent.h"
#include "IBPCalibrationMenuContent.h"
#include "Framework/Language/LanguageManager.h"
#include "SystemManager.h"
#include "FactoryCO2MenuContent.h"

FactoryMaintainMenuWindow *FactoryMaintainMenuWindow::getInstance()
{
    static FactoryMaintainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new FactoryMaintainMenuWindow;
        instance->addMenuContent(new MachineConfigModuleContent);
        instance->addMenuContent(new FactoryVersionInfo);
        instance->addMenuContent(new FactoryDataRecordContent);
        instance->addMenuContent(new FactorySystemInfoMenuContent);
        instance->addMenuContent(new FactoryImportExportMenuContent);
        instance->addMenuContent(new ServiceUpdateEntranceContent);
        if (systemManager.isSupport(CONFIG_NIBP))
        {
            instance->addMenuContent(new NIBPCalibrationMenuContent);
        }
        instance->addMenuContent(new FactoryTestMenuContent);
        if (systemManager.isSupport(CONFIG_TEMP))
        {
            instance->addMenuContent(new FactoryTempMenuContent);
        }
        if (systemManager.isSupport(CONFIG_IBP))
        {
            instance->addMenuContent(new IBPCalibrationMenuContent);
        }
        if (systemManager.isSupport(CONFIG_O2))
        {
            instance->addMenuContent(new O2CalibrationMenuContent);
        }
    }

    return instance;
}

FactoryMaintainMenuWindow::FactoryMaintainMenuWindow()
{
}
