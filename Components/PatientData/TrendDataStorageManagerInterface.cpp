/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/

#include "TrendDataStorageManagerInterface.h"
#include <stddef.h>

static TrendDataStorageManagerInterface *trendDataStorageManager = NULL;

TrendDataStorageManagerInterface *TrendDataStorageManagerInterface::registerTrendDataStorageManager(
        TrendDataStorageManagerInterface *instance)
{
    TrendDataStorageManagerInterface *old = trendDataStorageManager;
    trendDataStorageManager = instance;
    return old;
}

TrendDataStorageManagerInterface *TrendDataStorageManagerInterface::getTrendDataStorageManager()
{
    return trendDataStorageManager;
}
