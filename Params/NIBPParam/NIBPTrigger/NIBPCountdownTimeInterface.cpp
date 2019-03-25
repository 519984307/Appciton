/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/3/25
 **/
#include "NIBPCountdownTimeInterface.h"
#include "stddef.h"

static NIBPCountdownTimeInterface* nibpCountdownTime = NULL;

NIBPCountdownTimeInterface *NIBPCountdownTimeInterface::registerNIBPCountTime(NIBPCountdownTimeInterface *instance)
{
    NIBPCountdownTimeInterface* old = nibpCountdownTime;
    nibpCountdownTime = instance;
    return old;
}

NIBPCountdownTimeInterface *NIBPCountdownTimeInterface::getNIBPCountdownTime()
{
    return nibpCountdownTime;
}
