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

FactoryMaintainMenuWindow *FactoryMaintainMenuWindow::getInstance()
{
    static FactoryMaintainMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new FactoryMaintainMenuWindow;
    }

    return instance;
}

FactoryMaintainMenuWindow::FactoryMaintainMenuWindow()
{
}
