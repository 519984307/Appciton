/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/15
 **/

#include "ArrhythmiaAnalysisMenu.h"
#include "Framework/Language/LanguageManager.h"

class ArrhythmiaAnalysisMenuPrivate
{
public:
    ArrhythmiaAnalysisMenuPrivate(){}

    // load settings
    void loadOptions();
};
ArrhythmiaAnalysisMenu::ArrhythmiaAnalysisMenu()
    : MenuContent(trs("ArrhythmiaAnalysisMenu"), trs("ArrhythmiaAnalysisMenuDesc")),
      d_ptr(new ArrhythmiaAnalysisMenuPrivate)
{
}

ArrhythmiaAnalysisMenu::~ArrhythmiaAnalysisMenu()
{
}

void ArrhythmiaAnalysisMenu::readyShow()
{
    d_ptr->loadOptions();
}

void ArrhythmiaAnalysisMenu::layoutExec()
{
}

void ArrhythmiaAnalysisMenuPrivate::loadOptions()
{
}
