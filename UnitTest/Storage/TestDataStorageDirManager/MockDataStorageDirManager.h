/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/14
 **/

#pragma once
#include "gmock/gmock.h"
#include "DataStorageDirManagerInterface.h"

class MockDataStorageDirManager : public DataStorageDirManagerInterface
{
public:
    MOCK_CONST_METHOD0(getCurFolder, QString());
    MOCK_METHOD1(createDir, void(bool));
    MOCK_METHOD0(cleanCurData, void(void));
};
