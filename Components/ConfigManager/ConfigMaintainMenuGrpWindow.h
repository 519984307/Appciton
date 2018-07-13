/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.13 10:10
 **/

#pragma once
#include "MenuWindow.h"

class ConfigMaintainMenuGrpWindow: public MenuWindow
{
    Q_OBJECT
public:
    ConfigMaintainMenuGrpWindow();
    ~ConfigMaintainMenuGrpWindow(){}

    /**
     * @brief getInstance
     * @return
     */
    static ConfigMaintainMenuGrpWindow* getInstance();
};
