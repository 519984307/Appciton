/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/3/7
 **/
#include "TestErrorLog.h"
#include "googletest.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestErrorLogTest tc;
    return QTest::qExec(&tc, argc, argv);
}
