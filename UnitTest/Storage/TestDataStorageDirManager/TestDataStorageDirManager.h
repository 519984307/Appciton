/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/20
 **/

#pragma once
#include <QString>
#include <QtTest>
#include "MockTimeManager.h"
#include "AlarmParamIFace.h"

class TestDataStorageDirManager : public QObject
{
    Q_OBJECT

public:
    TestDataStorageDirManager();

private slots:
    void init();
    void cleanup();
    void testGetCurFolder();
    void testGetRescueEvent();
    void testGetRescueDataDir();
    void testIsCurRescueFolderFull_data();
    void testIsCurRescueFolderFull();
    void testDeleteData_data();
    void testDeleteData();
    void testCleanCurData();
};
