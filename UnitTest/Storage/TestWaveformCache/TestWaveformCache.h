/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/11
 **/

#ifndef TESTWAVEFORMCACHETEST_H
#define TESTWAVEFORMCACHETEST_H
#include <QString>
#include <QtTest>
#include "WaveformCache.h"

class TestWaveformCache : public QObject
{
    Q_OBJECT

public:
    TestWaveformCache();

private slots:
    void init();
    void cleanup();
    void testRegisterSource_data();
    void testRegisterSource();
};
#endif // TESTWAVEFORMCACHETEST_H
