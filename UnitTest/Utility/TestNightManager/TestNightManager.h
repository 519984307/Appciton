/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/6
 **/

#pragma once
#include <QTest>
#include <QObject>

class TestNightManager : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testSetNightMode_data();
    void testSetNightMode();
    void testNightMode();
    void cleanupTestCase();
};
