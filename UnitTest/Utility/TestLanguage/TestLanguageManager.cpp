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
#include "Framework/Language/Translator.h"
#include <QString>


void TestLanguageManager::init()
{
    if (instance == NULL)
    {
        /* create instance */
        LanguageManager::getInstance();
    }
}

void TestLanguageManager::testRegisterInvalidLanguageId()
{
    QCOMPARE(instance->registerTranslator(Language_NR, new Translator(QString())), false);

    /* if the translator is invalid, will return the string id */
    QCOMPARE(instance->translate("nonExistID"), QString("nonExistID"));
}

void TestLanguageManager::testRegisterInvalidTranslator()
{
    QCOMPARE(instance->registerTranslator(English, NULL), false);
    QCOMPARE(instance->registerTranslator(English, new Translator(QString())), true);

    /* if the translator is invalid, will return the string id */
    QCOMPARE(instance->translate("nonExistID"), QString("nonExistID"));
}

void TestLanguageManager::testRegisterTranslator()
{
    instance->registerTranslator(English, new Translator(":/English.xml"));
    /* translate NULL string will return emtpy string */
    QCOMPARE(instance->translate(NULL), QString());
    QCOMPARE(instance->translate("Adult"), QString("Adult"));
    QCOMPARE(instance->translate("AlarmLimit"), QString("Alarm Limit"));
    /* if the translator is invalid, will return the string id */
    QCOMPARE(instance->translate("nonExistID"), QString("nonExistID"));
}

void TestLanguageManager::testGetCurLanguage()
{
    /* when no any translator, the current langauge will be invalid */
    QCOMPARE(instance->getCurLanguage(), Language_NR);

    /* after reginster a valid language, the active language will auto switch to the valid language */
    instance->registerTranslator(English, new Translator(":/English.xml"));
    QCOMPARE(instance->getCurLanguage(), English);

    /* the current valid lanague won't be affect after register a new lanaguage */
    instance->registerTranslator(ChineseSimplified, new Translator(":/ChineseSimplified.xml"));
    QCOMPARE(instance->getCurLanguage(), English);
}

void TestLanguageManager::testSetCurLanguage()
{
    QVERIFY2(instance->setCurLanguage(English) == false, "not translator, can't set to the language");
    instance->registerTranslator(English, new Translator(":/English.xml"));
    QCOMPARE(instance->translate("AlarmLimit"), QString("Alarm Limit"));
    QVERIFY2(instance->setCurLanguage(English), "cat set to the language that has a translator");
    instance->registerTranslator(ChineseSimplified, new Translator(":/ChineseSimplified.xml"));
    QVERIFY2(instance->setCurLanguage(ChineseSimplified), "set to the another language with valid translator");
    QCOMPARE(instance->translate("AlarmLimit"), QString::fromUtf8("报警限"));
}

void TestLanguageManager::cleanup()
{
    if (instance)
    {
        LanguageManager *tmp = instance;
        instance = NULL;
        delete tmp;
    }
}
