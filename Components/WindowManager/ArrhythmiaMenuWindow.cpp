/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/15
 **/

#include "ArrhythmiaMenuWindow.h"
#include "ArrhythmiaAnalysisMenu.h"
#include "ArrhythmiaThresholdMenu.h"

ArrhythmiaMenuWindow *ArrhythmiaMenuWindow::getInstance()
{
    static ArrhythmiaMenuWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new ArrhythmiaMenuWindow;

        instance->addMenuContent(new ArrhythmiaThresholdMenu);
        instance->addMenuContent(new ArrhythmiaAnalysisMenu);
    }

    return instance;
}

ArrhythmiaMenuWindow::ArrhythmiaMenuWindow()
{
}
