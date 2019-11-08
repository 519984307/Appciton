/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/15
 **/

#include "TestPatientInfoWidget.h"
#include "PatientInfoWidget.h"
#include "MockFontManager.h"
#include "MockWindowManager.h"
#include "PatientInfoWindowInterface.h"
#include <QFont>
#include <QLayout>
#include "Framework/Language/LanguageManager.h"
#include <QVariant>

using ::testing::Mock;
using ::testing::Return;
using ::testing::_;

Q_DECLARE_METATYPE(PatientType)

void TestPatientInfoWidget::initTestCase()
{
    MockFontManager mockFontManager;
    MockFontManager::registerFontManager(&mockFontManager);
    QFont font;
    font.setFamily("Droid Sans Fallback");
    font.setBold(false);
    font.setPixelSize(20);
    EXPECT_CALL(mockFontManager, getFontSize(4)).WillOnce(Return(20));
    EXPECT_CALL(mockFontManager, textFont(_, _)).WillRepeatedly(Return(font));
    patientInfoWidget = new PatientInfoWidget;
    QVERIFY(Mock::VerifyAndClearExpectations(&mockFontManager));
    MockFontManager::registerFontManager(NULL);
}

void TestPatientInfoWidget::cleanupTestCase()
{
    delete patientInfoWidget;
}

void TestPatientInfoWidget::testClick()
{
    MockWindowManager mockWindowManager;
    MockWindowManager::registerWindowManager(&mockWindowManager);
    PatientInfoWindowInterface patientInfoWindow;
    PatientInfoWindowInterface::registerPatientInfoWindow(&patientInfoWindow);
    EXPECT_CALL(mockWindowManager, showWindow(&patientInfoWindow,
                                              WindowManagerInterface::ShowBehaviorCloseIfVisiable |
                                              WindowManagerInterface::ShowBehaviorCloseOthers));

    QTest::mouseClick(patientInfoWidget, Qt::LeftButton);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockWindowManager));
    MockWindowManager::registerWindowManager(NULL);
    PatientInfoWindowInterface::registerPatientInfoWindow(NULL);
}

void TestPatientInfoWidget::testLoadPatientInfo_data()
{
    QTest::addColumn<PatientType>("type");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("bed");

    QTest::newRow("adult/zhangSan/P01") << PATIENT_TYPE_ADULT << "zhangSan" << "P01";
    QTest::newRow("neo/zhangsan/P01") << PATIENT_TYPE_NEO << "zhangSan" << "P01";
    QTest::newRow("ped/zhangsan/P01") << PATIENT_TYPE_PED << "zhangSan" << "P01";
    QTest::newRow("adult/null/P01") << PATIENT_TYPE_ADULT << "" << "P01";
    QTest::newRow("adult/zhangSan/null") << PATIENT_TYPE_ADULT << "zhangSan" << "";
    QTest::newRow("adult/null/null") << PATIENT_TYPE_ADULT << "" << "";
    QTest::newRow("adult/Daenerys Targaryen/P01") << PATIENT_TYPE_ADULT
                                                  << "Daenerys Targaryen"
                                                  << "P01";
}

void TestPatientInfoWidget::testLoadPatientInfo()
{
    QFETCH(PatientType, type);
    QFETCH(QString, name);
    QFETCH(QString, bed);
    PatientInfo info;
    info.type = type;
    strncpy(info.name, name.toLocal8Bit(), name.length());

    patientInfoWidget->loadPatientInfo(info, bed);
}
