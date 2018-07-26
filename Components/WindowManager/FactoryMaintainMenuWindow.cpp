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
#include "SoftWareVersionContent.h"
#include "MonitorInfoContent.h"
#include "MachineConfigModuleContent.h"
#include "FactoryDataRecordContent.h"
#include "SelectStarterLogoContent.h"
#include "FactorySystemInfoMenuContent.h"
#include "FactoryImportExportMenuContent.h"
#include "ServiceUpdateEntranceContent.h"
#include "NIBPCalibrationMenuContent.h"
#include "FactoryTestMenuContent.h"
#include "FactoryTempMenuContent.h"

FactoryMaintainMenuWindow *FactoryMaintainMenuWindow::getInstance()
{
    static FactoryMaintainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new FactoryMaintainMenuWindow;
        instance->addMenuContent(new SoftWareVersionContent);
        instance->addMenuContent(new MonitorInfoContent);
        instance->addMenuContent(new MachineConfigModuleContent);
        instance->addMenuContent(new FactoryDataRecordContent);
        instance->addMenuContent(new SelectStarterLogoContent);
        instance->addMenuContent(new FactorySystemInfoMenuContent);
        instance->addMenuContent(new FactoryImportExportMenuContent);
        instance->addMenuContent(new ServiceUpdateEntranceContent);
        instance->addMenuContent(new NIBPCalibrationMenuContent);
        instance->addMenuContent(new FactoryTestMenuContent);
        instance->addMenuContent(new FactoryTempMenuContent);
    }

    return instance;
}

FactoryMaintainMenuWindow::FactoryMaintainMenuWindow()
{
}
