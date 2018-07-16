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

UserMaintainMenuWindow *UserMaintainMenuWindow::getInstance()
{
    static UserMaintainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new UserMaintainMenuWindow;
    }

    return instance;
}

UserMaintainMenuWindow::UserMaintainMenuWindow()
{
}
