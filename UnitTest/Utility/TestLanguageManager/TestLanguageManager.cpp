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

#define AdultConfigPath "/usr/local/nPM/etc/AdultConfig.xml"

void TestLanguageManager::initTestCase()
{
    curLanguage = languageManager.getCurLanguage();
}

void TestLanguageManager::testReload_data()
{
    QTest::addColumn<int>("index");
    QTest::addColumn<int>("curLanguage");

    QTest::newRow("chinese") << 1 << 1;
    QTest::newRow("English") << 0 << 0;
    QTest::newRow("NR") << 2 << 0;
    QTest::newRow("Abnormal") << 500 << 0;
}

void TestLanguageManager::testReload()
{
    QFETCH(int, index);
    QFETCH(int, curLanguage);

    languageManager.reload(index);
    QCOMPARE(static_cast<int>(languageManager.getCurLanguage()), curLanguage);
}

void TestLanguageManager::testTranslateChinese_data()
{
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("result");

    QTest::newRow("normal case") << QString("LanguageNameLocal") << QString::fromLocal8Bit("简体中文");
    QTest::newRow("null case") << QString() << QString();
    QTest::newRow("no exist case") << QString("noExist") << QString("noExist");
    QTest::newRow("chinese case") << QString::fromLocal8Bit("简体中文") << QString::fromLocal8Bit("简体中文");
    QTest::newRow("special character") << QString::fromLatin1("???") << QString::fromLatin1("???");
}

void TestLanguageManager::testTranslateChinese()
{
    QFETCH(QString, id);
    QFETCH(QString, result);

    // chinese
    languageManager.reload(1);
    QCOMPARE(languageManager.translate("LanguageNameLocal"), QString::fromLocal8Bit("简体中文"));
    QCOMPARE(languageManager.translate(""), QString());
    QCOMPARE(languageManager.translate("noExist"), QString::fromLocal8Bit("noExist"));
    QCOMPARE(languageManager.translate(NULL), QString("null"));
    QCOMPARE(languageManager.translate(id), result);
}

void TestLanguageManager::testTranslateEnglish_data()
{
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("result");

    QTest::newRow("normal case") << QString("LanguageNameLocal")<< QString("English");
    QTest::newRow("null case") << QString() << QString();
    QTest::newRow("no exist case") << QString("noExist") << QString("noExist");
    QTest::newRow("chinese case")<< QString::fromLocal8Bit("简体中文") << QString::fromLocal8Bit("简体中文");
    QTest::newRow("special character") << QString::fromLatin1("???") << QString::fromLatin1("???");
}

void TestLanguageManager::testTranslateEnglish()
{
    QFETCH(QString, id);
    QFETCH(QString, result);

    languageManager.reload(0);
    QCOMPARE(languageManager.translate(id), result);

    QCOMPARE(languageManager.translate("LanguageNameLocal"), QString("English"));
    QCOMPARE(languageManager.translate(""), QString());
    QCOMPARE(languageManager.translate("noExist"), QString("noExist"));
    QCOMPARE(languageManager.translate(NULL), QString("null"));
}

void TestLanguageManager::testGetCurVoicePromptFolderName_data()
{
    QTest::addColumn<int>("language");
    QTest::addColumn<QString>("result");

    QTest::newRow("english") << 0 << QString("English");
    QTest::newRow("chinese") << 1 << QString("ChinaMainland");
    QTest::newRow("NR case") << 2 << QString("English");
    QTest::newRow("abnormal case") << 500 << QString("English");
}

void TestLanguageManager::testGetCurVoicePromptFolderName()
{
    QFETCH(int, language);
    QFETCH(QString, result);

    languageManager.reload(language);
    QCOMPARE(languageManager.getCurVoicePromptFolderName(), result);
}

void TestLanguageManager::cleanupTestCase()
{
    languageManager.reload(curLanguage);
}
