/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by tongzhou fang  fangtongzhou@blmed.cn, 2019/3/20
 **/

#include "TestNIBPMonitorStopState.h"
#include "googletest.h"
int main(int argc, char *argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    QCoreApplication app(argc, argv);
    TestNIBPMonitorStopState tc;
    return QTest::qExec(&tc, argc, argv);
}
