/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/11
 **/

#include "WaveformCacheInterface.h"

static WaveformCacheInterface *currentIface = NULL;

WaveformCacheInterface *WaveformCacheInterface::registerWaveformCache(WaveformCacheInterface *interface)
{
    WaveformCacheInterface *old = currentIface;
    currentIface = interface;
    return old;
}

WaveformCacheInterface *WaveformCacheInterface::getWaveformCache()
{
    return currentIface;
}
