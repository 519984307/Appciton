/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/4/1
 **/

#pragma once
#include "gmock/gmock.h"
#include "TrendCacheInterface.h"

class MockTrendCache : public TrendCacheInterface
{
public:
    MOCK_METHOD2(getTrendData, bool(unsigned t, TrendCacheData *data));
    MOCK_METHOD2(collectTrendData, void(unsigned t, bool overwrite));
    MOCK_METHOD1(collectTrendAlarmStatus, void(unsigned t));
    MOCK_METHOD0(clearTrendCache, void(void));
    MOCK_METHOD1(stopDataCollect, void(quint32));
};
