/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/1
 **/

#include "TestLanguageManager.h"
#include "LanguageManager.h"
#include <QString>
#include "MockLanguageManger.h"
using ::testing::Return;

#define AdultConfigPath "/usr/local/nPM/etc/AdultConfig.xml"

void TestLanguageManager::initTestCase()
{
    curLanguage = languageManager.getCurLanguage();
}

void TestLanguageManager::testTranslate_data()
{
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("result");

    if (languageManager.getCurLanguage())
    {
        // chinese
        QTest::newRow("normal case") << QString("LanguageNameLocal") << QString::fromLocal8Bit("简体中文");
    }
    else
    {
        // english
        QTest::newRow("normal case") << QString("LanguageNameLocal") << QString::fromLocal8Bit("English");
    }
    QTest::newRow("null case") << QString() << QString();
    QTest::newRow("no exist case") << QString("noExist") << QString("noExist");
    QTest::newRow("chinese case") << QString::fromLocal8Bit("简体中文") << QString::fromLocal8Bit("简体中文");
    QTest::newRow("special character") << QString::fromLatin1("???") << QString::fromLatin1("???");
}

void TestLanguageManager::testTranslate()
{
    QFETCH(QString, id);
    QFETCH(QString, result);
    MockConfigManager configManager;
    ConfigManagerInterface::registerConfigManager(&configManager);

    Config adultConfig(QString(AdultConfigPath));
    EXPECT_CALL(configManager, getCurConfig()).WillOnce(Return(adultConfig));
    // chinese
    languageManager.reload(1);
    QCOMPARE(languageManager.translate("LanguageNameLocal"), QString::fromLocal8Bit("简体中文"));
    QCOMPARE(languageManager.translate(""), QString());
    QCOMPARE(languageManager.translate("noExist"), QString::fromLocal8Bit("noExist"));
    QCOMPARE(languageManager.translate(NULL), QString("null"));
    QCOMPARE(languageManager.translate(id), result);

    // english
    languageManager.reload(0);
    QCOMPARE(languageManager.translate("LanguageNameLocal"), QString::fromLocal8Bit("English"));
    QCOMPARE(languageManager.translate(""), QString());
    QCOMPARE(languageManager.translate("noExist"), QString::fromLocal8Bit("noExist"));
    QCOMPARE(languageManager.translate(NULL), QString("null"));
    QCOMPARE(languageManager.translate(id), result);
}

void TestLanguageManager::testReload()
{
}

void TestLanguageManager::testGetCurVoicePromptFolderName()
{
}

void TestLanguageManager::cleanupTestCase()
{
    languageManager.reload(curLanguage);
}
