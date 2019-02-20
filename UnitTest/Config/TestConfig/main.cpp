/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/2/20
 **/

#include "TestConfig.h"
#include <QTest>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestConfig tc;
    return QTest::qExec(&tc, argc, argv);
}
