/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#include "AlarmLimitMenuContent.h"
#include "LanguageManager.h"

class AlarmLimitMenuContentPrivate
{
public:
    AlarmLimitMenuContentPrivate() {}
};

AlarmLimitMenuContent::AlarmLimitMenuContent()
    : MenuContent(trs("AlarmLimitMenu"), trs("AlarmLimitMenuDesc")),
      d_ptr(new AlarmLimitMenuContentPrivate())
{
}

AlarmLimitMenuContent::~AlarmLimitMenuContent()
{
    delete d_ptr;
}

void AlarmLimitMenuContent::readyShow()
{
}

void AlarmLimitMenuContent::layoutExec()
{
}
