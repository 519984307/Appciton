/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.13  11:30
 **/

#pragma once
#include "MenuContent.h"

class SelectDefaultConfigMenuContent: public MenuContent
{
    Q_OBJECT
public:
    SelectDefaultConfigMenuContent();
    ~SelectDefaultConfigMenuContent();

    /**
     * @brief readyShow
     */
    virtual void readyShow();
};


