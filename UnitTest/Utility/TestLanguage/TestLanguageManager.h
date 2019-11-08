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
#include "Framework/Language/LanguageManager.h"
class TestLanguageManager : public QObject, public LanguageManager
{
    Q_OBJECT
public:
    TestLanguageManager(){}

private Q_SLOTS:
    void init();
    void testRegisterInvalidLanguageId();
    void testRegisterInvalidTranslator();
    void testRegisterTranslator();
    void testGetCurLanguage();
    void testSetCurLanguage();
    void cleanup();
};
