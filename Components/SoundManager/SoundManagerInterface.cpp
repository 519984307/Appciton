/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/6
 **/

#include "SoundManagerInterface.h"
#include "stddef.h"

static SoundManagerInterface *soundManager = NULL;

SoundManagerInterface *SoundManagerInterface::registerSoundManager(SoundManagerInterface *instance)
{
    SoundManagerInterface *old = soundManager;
    soundManager = instance;
    return old;
}

SoundManagerInterface *SoundManagerInterface::getSoundManager()
{
    return soundManager;
}
