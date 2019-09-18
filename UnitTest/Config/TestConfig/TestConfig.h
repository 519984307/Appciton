/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/2/20
 **/

#ifndef TESTCONFIG_H
#define TESTCONFIG_H
#include <QObject>

class TestConfig : public QObject
{
    Q_OBJECT

public:
    TestConfig();
    ~TestConfig();

private slots:
    void test_case1();
};
#endif // TESTCONFIG_H
