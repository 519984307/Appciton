/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/20
 **/

#ifndef TESTDATASTORAGEDIRMANAGER_H
#define TESTDATASTORAGEDIRMANAGER_H
#include <QString>
#include <QtTest>

class TestDataStorageDirManager : public QObject
{
    Q_OBJECT

public:
    TestDataStorageDirManager();

private slots:
    void init();
    void cleanup();
    void testCreateDir();
};
#endif // TESTDATASTORAGEDIRMANAGER_H
