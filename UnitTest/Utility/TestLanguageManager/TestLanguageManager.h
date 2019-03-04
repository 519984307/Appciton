/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/1
 **/

#pragma once

#include <QTest>
class TestLanguageManager : public QObject
{
    Q_OBJECT
public:
    TestLanguageManager() : curLanguage(0){}

private Q_SLOTS:
    void initTestCase();
    void testReload();
    void testTranslate_data();
    void testTranslate();
    void testGetCurVoicePromptFolderName();
    void cleanupTestCase();

private:
    int curLanguage;
};
