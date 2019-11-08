/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/8
 **/

#include "TestTranslator.h"
#include "Framework/Language/Translator.h"

void TestTranslator::testInvalidFile()
{
    /* test empty file path */
    Translator invalidTranslator("");
    /* Invalid translation fill path will create a invalid translator */
    QCOMPARE(invalidTranslator.isValid(), false);

    /* translate non-exist string id will return return the string id  */
    QString nonExistStringID = "NonExistStringID";
    QCOMPARE(invalidTranslator.translate(nonExistStringID), nonExistStringID);

    /* translate empty string will return empty string */
    QString emptyString;
    QCOMPARE(invalidTranslator.translate(emptyString).isEmpty(), true);

    /* test invaid file path */
    Translator invalidTranslator1("/tmp/noExistFile");
    QCOMPARE(invalidTranslator1.isValid(), false);

    /* test broken file path */
    Translator invalidTranslator2(":/English_broken.xml");
    QCOMPARE(invalidTranslator2.isValid(), false);
}

void TestTranslator::testValidFile()
{
    Translator englishTranslator(":/English.xml");
    QCOMPARE(englishTranslator.isValid(), true);

    Translator chineseTranslator(":/ChineseSimplified.xml");
    QCOMPARE(chineseTranslator.isValid(), true);
}

void TestTranslator::testTranslate_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("strId");
    QTest::addColumn<QString>("translation");
    QTest::newRow("EN: Empty string") << QString(":/English.xml") << QString() << QString();
    QTest::newRow("EN: Normal String") << QString(":/English.xml") << QString("AlarmLimit")
                                       << QString("Alarm Limit");
    QTest::newRow("EN: Normal Long String") << QString(":/English.xml")
                                            << QString("MachineConfigIsUpdatedNow")
                                            << QString("The machine configuration has been updated.");
    QTest::newRow("EN: No exist String") << QString(":/English.xml") << QString("NoExistStringId")
                                         << QString("NoExistStringId");
    QTest::newRow("EN: Invalid str ID 1") << QString(":/English.xml") << QString("123Id") << QString("123Id");
    QTest::newRow("EN: Invalid str ID 2") << QString(":/English.xml") << QString("abc/def") << QString("abc/def");

    QTest::newRow("CN: Empty string") << QString(":/ChineseSimplified.xml") << QString() << QString();
    QTest::newRow("CN: Normal String") << QString(":/ChineseSimplified.xml")
                                       << QString("AlarmLimit") << QString::fromUtf8("报警限");
    QTest::newRow("EN: Normal Long String") << QString(":/ChineseSimplified.xml")
                                            << QString("MachineConfigIsUpdatedNow")
                                            << QString::fromUtf8("机器配置现在已经更新。");
    QTest::newRow("CN: No exist String") << QString(":/ChineseSimplified.xml")
                                         << QString("NoExistStringId") << QString("NoExistStringId");
    QTest::newRow("CN: Invalid str ID 1") << QString(":/ChineseSimplified.xml") << QString("123Id") << QString("123Id");
    QTest::newRow("CN: Invalid str ID 2") << QString(":/ChineseSimplified.xml") << QString("abc/def")
                                          << QString("abc/def");
}

void TestTranslator::testTranslate()
{
    QFETCH(QString, file);
    QFETCH(QString, strId);
    QFETCH(QString, translation);

    Translator translator(file);

    QCOMPARE(translator.translate(strId), translation);
}
