/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/15
 **/

#pragma once
#include "Framework/UI/MenuWindow.h"

class ArrhythmiaMenuWindow : public MenuWindow
{
    Q_OBJECT
public:
    static ArrhythmiaMenuWindow *getInstance();

private:
    ArrhythmiaMenuWindow();
};
