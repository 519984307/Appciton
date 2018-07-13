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

ConfigManagerMenuWindow *ConfigManagerMenuWindow::getInstance()
{
    static ConfigManagerMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new ConfigManagerMenuWindow;
    }

    return instance;
}

ConfigManagerMenuWindow::ConfigManagerMenuWindow()
    : MenuWindow()
{
}
