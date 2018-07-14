/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.13 10:10
 **/
#include "ConfigMaintainMenuGrpWindow.h"
#include "LanguageManager.h"
#include "SelectDefaultConfigMenuContent.h"
#include "UserConfigEditMenuContent.h"
#include "ConfigExportImportMenuContent.h"

ConfigMaintainMenuGrpWindow::ConfigMaintainMenuGrpWindow():
    MenuWindow()
{}

ConfigMaintainMenuGrpWindow *ConfigMaintainMenuGrpWindow::getInstance()
{
    static ConfigMaintainMenuGrpWindow *instance = NULL;

    if (instance == NULL)
    {
        instance = new ConfigMaintainMenuGrpWindow;

        instance->addMenuContent(new SelectDefaultConfigMenuContent);
        instance->addMenuContent(new UserConfigEditMenuContent);
        instance->addMenuContent(new ConfigExportImportMenuContent);
    }

    return instance;
}
