/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "ConfigManagerMenuWindow.h"
#include "SelectDefaultConfigMenuContent.h"
#include "UserConfigEditMenuContent.h"
#include "ConfigExportImportMenuContent.h"
#include "Framework/Language/LanguageManager.h"
#include "ConfigManagerPassWordEditMenuContent.h"
#include "SaveCurrentConfigMenuContent.h"

ConfigManagerMenuWindow *ConfigManagerMenuWindow::getInstance()
{
    static ConfigManagerMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new ConfigManagerMenuWindow;
        instance->addMenuContent(new SelectDefaultConfigMenuContent);
        instance->addMenuContent(new UserConfigEditMenuContent);
        instance->addMenuContent(new ConfigExportImportMenuContent);
        instance->addMenuContent(new ConfigManagerPassWordEditMenuContent);
        instance->addMenuContent(new SaveCurrentConfigMenuContent);
        instance->setWindowTitle(trs("DefaultConfigDesc"));
    }

    return instance;
}

ConfigManagerMenuWindow::ConfigManagerMenuWindow()
    : MenuWindow()
{
}
