/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/6
 **/

#include "TestNightManager.h"
#include "NightModeManager.h"


void TestNightManager::initTestCase()
{
}

void TestNightManager::testSetNightMode_data()
{
    QTest::addColumn<bool>("nightMode");
}

void TestNightManager::testSetNightMode()
{
    QFETCH(bool, nightMode);
    nightModeManager.setNightMode(nightMode);
}

void TestNightManager::testNightMode()
{
}

void TestNightManager::cleanupTestCase()
{
}
