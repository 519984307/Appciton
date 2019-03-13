/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/6
 **/

#include "TestStorageFile.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestStorageFile tc;
    return QTest::qExec(&tc, argc, argv);
}
