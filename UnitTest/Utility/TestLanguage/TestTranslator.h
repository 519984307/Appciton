/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/7
 **/

#pragma once

#include <QTest>

class Translator;
class TestTranslator : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testInvalidFile();
    void testValidFile();
    void testTranslate_data();
    void testTranslate();
};
