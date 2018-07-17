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

UserMaintainMenuWindow *UserMaintainMenuWindow::getInstance()
{
    static UserMaintainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new UserMaintainMenuWindow;

        instance->addMenuContent(new UserMaintainGeneralMenuContent);
        instance->addMenuContent(new ModuleMaintainMenuContent);
        instance->addMenuContent(new AlarmMaintainMenuContent);
        instance->addMenuContent(new WifiMaintainMenuContent);
    }

    return instance;
}

UserMaintainMenuWindow::UserMaintainMenuWindow()
{
}
