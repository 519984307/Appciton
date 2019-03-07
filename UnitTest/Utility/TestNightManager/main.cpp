/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/


#include <QCoreApplication>
#include "TestNightManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TestNightManager tc;
    return QTest::qExec(&tc, argc, argv);
}
