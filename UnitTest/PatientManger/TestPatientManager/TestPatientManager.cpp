/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/13
 **/

#include "TestPatientManager.h"
#include "PatientManager.h"
#include <QVariant>
#include "MockConfigManager.h"
#include "MockECGParam.h"
#include "MockSystemManager.h"
#include "MockNIBPParam.h"
#include "MockDataStorageDirManager.h"
#include "MockAlarmIndicator.h"
#include <QDir>
#include <QFile>
#include "IConfig.h"
#include <unistd.h>
#include <sys/types.h>

Q_DECLARE_METATYPE(PatientType)
Q_DECLARE_METATYPE(PatientPacer)
Q_DECLARE_METATYPE(PatientSex)
Q_DECLARE_METATYPE(PatientBloodType)
Q_DECLARE_METATYPE(UnitType)

#define PATIENT_INFO_FILENAME QString("/PatientInfo.xml")

using ::testing::Mock;
using ::testing::ReturnRef;
using ::testing::Return;
using ::testing::_;

void TestPatientManager::initTestCase()
{
    QString newdir = QString("/run/user/%1/data").arg(getuid());
    QDir d(newdir);
    if (!d.exists())
    {
        d.mkdir(newdir);
    }
    DataStorageDirManagerInterface::setDataStorageDir(newdir);
    patientManager.setType(PATIENT_TYPE_ADULT);
}

void TestPatientManager::cleanup()
{
    MockConfigManager::registerConfigManager(NULL);
    MockECGParam::registerECGParam(NULL);
    MockSystemManager::registerSystemManager(NULL);
    MockNIBPParam::registerNIBPParam(NULL);
    MockDataStorageDirManager::registerDataStorageDirManager(NULL);
    MockAlarmIndicator::registerAlarmIndicator(NULL);
}

void TestPatientManager::cleanupTestCase()
{
}

void TestPatientManager::testType_data()
{
    QTest::addColumn<PatientType>("type");
    QTest::addColumn<PatientType>("result");

    QTest::newRow("neo") << PATIENT_TYPE_NEO << PATIENT_TYPE_NEO;
    QTest::newRow("adult") << PATIENT_TYPE_ADULT << PATIENT_TYPE_ADULT;
    QTest::newRow("ped") << PATIENT_TYPE_PED << PATIENT_TYPE_PED;
    QTest::newRow("null") << PATIENT_TYPE_NULL << PATIENT_TYPE_NULL;
    QTest::newRow("nr") << PATIENT_TYPE_NR << PATIENT_TYPE_NR;
}

void TestPatientManager::testType()
{
    QFETCH(PatientType, type);
    QFETCH(PatientType, result);
    MockConfigManager mockConfigManager;
    MockConfigManager::registerConfigManager(&mockConfigManager);
    Config currentConfig("/usr/local/nPM/etc/AdultConfig.xml");
    EXPECT_CALL(mockConfigManager, getCurConfig()).WillOnce(ReturnRef(currentConfig));
    EXPECT_CALL(mockConfigManager, loadConfig(type));

    MockECGParam mockECGParam;
    MockECGParam::registerECGParam(&mockECGParam);
    EXPECT_CALL(mockECGParam, setPatientType(type));

    MockSystemManager mockSystemManager;
    MockSystemManager::registerSystemManager(&mockSystemManager);
    EXPECT_CALL(mockSystemManager, isSupport(PARAM_NIBP)).WillOnce(Return(true));

    MockNIBPParam mockNIBPParam;
    MockNIBPParam::registerNIBPParam(&mockNIBPParam);
    NIBPProviderIFace nibpProvider;
    EXPECT_CALL(mockNIBPParam, provider()).WillOnce(ReturnRef(nibpProvider));

    patientManager.setType(type);
    QCOMPARE(result, patientManager.getType());

    QVERIFY(Mock::VerifyAndClearExpectations(&mockConfigManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockECGParam));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockSystemManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPParam));
}

void TestPatientManager::testPacermaker_data()
{
    QTest::addColumn<PatientPacer>("pacer");
    QTest::addColumn<PatientPacer>("result");

    QTest::newRow("off") << PATIENT_PACER_OFF << PATIENT_PACER_OFF;
    QTest::newRow("on") << PATIENT_PACER_ON << PATIENT_PACER_ON;
    QTest::newRow("nr") << PATIENT_PACER_NR << PATIENT_PACER_NR;
}

void TestPatientManager::testPacermaker()
{
    QFETCH(PatientPacer, pacer);
    QFETCH(PatientPacer, result);
    patientManager.setPacermaker(pacer);
    QCOMPARE(result, patientManager.getPacermaker());
}

void TestPatientManager::testSex_data()
{
    QTest::addColumn<PatientSex>("sex");
    QTest::addColumn<PatientSex>("result");

    QTest::newRow("null") << PATIENT_SEX_NULL << PATIENT_SEX_NULL;
    QTest::newRow("male") << PATIENT_SEX_MALE << PATIENT_SEX_MALE;
    QTest::newRow("female") << PATIENT_SEX_FEMALE << PATIENT_SEX_FEMALE;
    QTest::newRow("Nr") << PATIENT_SEX_NR << PATIENT_SEX_NR;
}

void TestPatientManager::testSex()
{
    QFETCH(PatientSex, sex);
    QFETCH(PatientSex, result);

    patientManager.setSex(sex);
    QCOMPARE(result, patientManager.getSex());
}

void TestPatientManager::testBornDate_data()
{
    QTest::addColumn<QDate>("date");
    QTest::addColumn<QDate>("result");

    QTest::newRow("normal") << QDate(2019, 1, 1) << QDate(2019, 1, 1);
    QTest::newRow("null") << QDate() << QDate();
    QTest::newRow("invail") << QDate(2019, 4, 31) << QDate();
}

void TestPatientManager::testBornDate()
{
    QFETCH(QDate, date);
    QFETCH(QDate, result);
    patientManager.setBornDate(date);
    QCOMPARE(result, patientManager.getBornDate());
}

void TestPatientManager::testBlood_data()
{
    QTest::addColumn<PatientBloodType>("blood");
    QTest::addColumn<int>("result");

    QTest::newRow("null") << PATIENT_BLOOD_TYPE_NULL << static_cast<int>(PATIENT_BLOOD_TYPE_NULL);
    QTest::newRow("A") << PATIENT_BLOOD_TYPE_A << static_cast<int>(PATIENT_BLOOD_TYPE_A);
    QTest::newRow("B") << PATIENT_BLOOD_TYPE_B << static_cast<int>(PATIENT_BLOOD_TYPE_B);
    QTest::newRow("AB") << PATIENT_BLOOD_TYPE_AB << static_cast<int>(PATIENT_BLOOD_TYPE_AB);
    QTest::newRow("O") << PATIENT_BLOOD_TYPE_O << static_cast<int>(PATIENT_BLOOD_TYPE_O);
    QTest::newRow("Nr") << PATIENT_BLOOD_TYPE_NR << static_cast<int>(PATIENT_BLOOD_TYPE_NR);
}

void TestPatientManager::testBlood()
{
    QFETCH(PatientBloodType, blood);
    QFETCH(int, result);
    patientManager.setBlood(blood);
    QCOMPARE(result, patientManager.getBlood());
}

void TestPatientManager::testWeight_data()
{
    QTest::addColumn<float>("weight");
    QTest::addColumn<float>("result");

    QTest::newRow("normal") << static_cast<float>(50.5) << static_cast<float>(50.5);
    QTest::newRow("zero") << static_cast<float>(0.0) << static_cast<float>(0.0);
    QTest::newRow("negative") << static_cast<float>(-50.0) << static_cast<float>(-50.0);
}

void TestPatientManager::testWeight()
{
    QFETCH(float, weight);
    QFETCH(float, result);

    patientManager.setWeight(weight);
    QCOMPARE(result, patientManager.getWeight());
}


void TestPatientManager::testHeight_data()
{
    QTest::addColumn<float>("height");
    QTest::addColumn<float>("result");

    QTest::newRow("normal") << static_cast<float>(180.5) << static_cast<float>(180.5);
    QTest::newRow("zero") << static_cast<float>(0.0) << static_cast<float>(0.0);
    QTest::newRow("negative") << static_cast<float>(-180.5) << static_cast<float>(-180.5);
}

void TestPatientManager::testHeight()
{
    QFETCH(float, height);
    QFETCH(float, result);

    patientManager.setHeight(height);
    QCOMPARE(result, patientManager.getHeight());
}

void TestPatientManager::testName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("result");

    QTest::newRow("normal") << QString("ZhangSan") << QString("ZhangSan");
    QTest::newRow("null") << QString("") << QString("");
    QTest::newRow("special characters") << QString("@@") << QString("@@");
    QTest::newRow("escape characters") << QString("\r") << QString("\r");
}

void TestPatientManager::testName()
{
    QFETCH(QString, name);
    QFETCH(QString, result);
    patientManager.setName(name);
    QCOMPARE(result, QString(patientManager.getName()));
    QCOMPARE(MAX_PATIENT_NAME_LEN - 1, patientManager.getNameLength());
}

void TestPatientManager::testPatientID_data()
{
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("result");

    QTest::newRow("normal") << QString("ZhangSan") << QString("ZhangSan");
    QTest::newRow("null") << QString("") << QString("");
    QTest::newRow("special characters") << QString("@@") << QString("@@");
    QTest::newRow("escape characters") << QString("\r") << QString("\r");
}

void TestPatientManager::testPatientID()
{
    QFETCH(QString, id);
    QFETCH(QString, result);
    patientManager.setPatID(id);
    QCOMPARE(id, QString(patientManager.getPatID()));
    QCOMPARE(MAX_PATIENT_ID_LEN - 1, patientManager.getIDLength());
}

void TestPatientManager::testBedNum_data()
{
    QTest::addColumn<QString>("bed");
    QTest::addColumn<QString>("result");

    QTest::newRow("normal") << QString("ZhangSan") << QString("ZhangSan");
    QTest::newRow("null") << QString("") << QString("");
    QTest::newRow("special characters") << QString("@@") << QString("@@");
    QTest::newRow("escape characters") << QString("\r") << QString("\r");
}

void TestPatientManager::testBedNum()
{
    QFETCH(QString, bed);
    QFETCH(QString, result);
    patientManager.setBedNum(bed);
    QCOMPARE(result, patientManager.getBedNum());
}

void TestPatientManager::testPatientInfo_data()
{
    QTest::addColumn<PatientType>("type");
    QTest::addColumn<PatientPacer>("pacer");
    QTest::addColumn<PatientSex>("sex");
    QTest::addColumn<QDate>("bornDate");
    QTest::addColumn<PatientBloodType>("blood");
    QTest::addColumn<float>("weight");
    QTest::addColumn<float>("height");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("id");

    QTest::newRow("adult/pacer on/male/1990.01.01/A/70/188/zhangsan/P01")
            << PATIENT_TYPE_ADULT
            << PATIENT_PACER_ON
            << PATIENT_SEX_MALE
            << QDate(1990, 1, 1)
            << PATIENT_BLOOD_TYPE_A
            << static_cast<float>(70.0)
            << static_cast<float>(188.0)
            << "zhangsan"
            << "P01";
    QTest::newRow("NR/NR/NR/null/NR/0.0/0.0/null/null")
            << PATIENT_TYPE_NR
            << PATIENT_PACER_NR
            << PATIENT_SEX_NR
            << QDate()
            << PATIENT_BLOOD_TYPE_NR
            << static_cast<float>(0.0)
            << static_cast<float>(0.0)
            << ""
            << "";
    QTest::newRow("adult/pacer off/female/1990.01.01/A/70/188/zhangsan/P01")
            << PATIENT_TYPE_ADULT
            << PATIENT_PACER_OFF
            << PATIENT_SEX_FEMALE
            << QDate(1990, 1, 1)
            << PATIENT_BLOOD_TYPE_A
            << static_cast<float>(70.0)
            << static_cast<float>(188.0)
            << "zhangsan"
            << "P01";
}

void TestPatientManager::testPatientInfo()
{
    QFETCH(PatientType, type);
    QFETCH(PatientPacer, pacer);
    QFETCH(PatientSex, sex);
    QFETCH(QDate, bornDate);
    QFETCH(PatientBloodType, blood);
    QFETCH(float, weight);
    QFETCH(float, height);
    QFETCH(QString, name);
    QFETCH(QString, id);

    patientManager.setType(type);
    patientManager.setPacermaker(pacer);
    patientManager.setSex(sex);
    patientManager.setBornDate(bornDate);
    patientManager.setBlood(blood);
    patientManager.setWeight(weight);
    patientManager.setHeight(height);
    patientManager.setName(name);
    patientManager.setPatID(id);

    // test getPatientInfo()
    const PatientInfo &info = patientManager.getPatientInfo();
    QCOMPARE(info.type, type);
    QCOMPARE(info.pacer, pacer);
    QCOMPARE(info.sex, sex);
    QCOMPARE(info.bornDate, bornDate);
    QCOMPARE(info.blood, blood);
    QCOMPARE(info.weight, weight);
    QCOMPARE(info.height, height);
    QCOMPARE(QString(info.name), name);
    QCOMPARE(QString(info.id), id);

    // test updatePaitentInfo() and GetHistroyPatientInfo()
    QString testPath = DataStorageDirManagerInterface::dataStorageDir() + QString("TestData");
    QString testFile = testPath + PATIENT_INFO_FILENAME;
    QDir dir(DataStorageDirManagerInterface::dataStorageDir());
    if (!dir.exists(testPath) || !QFile::exists(testFile))
    {
        dir.mkpath(testPath);
        QFile f(":" + PATIENT_INFO_FILENAME);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QByteArray data = f.readAll();
            QFile outFile(testPath + PATIENT_INFO_FILENAME);
            if (outFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
                outFile.write(data);
                outFile.close();
            }
            f.close();
        }
    }
    MockDataStorageDirManager mockDataStorageDirManager;
    MockDataStorageDirManager::registerDataStorageDirManager(&mockDataStorageDirManager);
    EXPECT_CALL(mockDataStorageDirManager, getCurFolder()).WillOnce(Return(testPath));

    patientManager.updatePatientInfo();
    PatientInfo historyInfo = patientManager.getHistoryPatientInfo(testPath + "/PatientInfo");

    if (dir.exists())
    {
        dir.remove(testPath + PATIENT_INFO_FILENAME);
        dir.rmpath(testPath);
    }

    QCOMPARE(historyInfo.type, type);
    QCOMPARE(historyInfo.pacer, pacer);
    QCOMPARE(historyInfo.sex, sex);
    QCOMPARE(historyInfo.bornDate, bornDate);
    QCOMPARE(historyInfo.blood, blood);
    QCOMPARE(historyInfo.weight, weight);
    QCOMPARE(historyInfo.height, height);
    QCOMPARE(QString(info.name), name);
    QCOMPARE(QString(info.id), id);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockDataStorageDirManager));
}

void TestPatientManager::testGetWeightUnit_data()
{
    QTest::addColumn<UnitType>("weight");
    QTest::addColumn<UnitType>("result");

    QTest::newRow("kg") << UNIT_KG << UNIT_KG;
    QTest::newRow("lb") << UNIT_LB << UNIT_LB;
    QTest::newRow("not weight unit") << UNIT_CM << UNIT_KG;
    QTest::newRow("NR") << UNIT_NR << UNIT_KG;
    QTest::newRow("None") << UNIT_NONE << UNIT_KG;
}

void TestPatientManager::testGetWeightUnit()
{
    QFETCH(UnitType, weight);
    QFETCH(UnitType, result);
    systemConfig.setNumValue("Unit|WeightUnit", static_cast<int>(weight));
    systemConfig.saveToDisk();
    QCOMPARE(result, patientManager.getWeightUnit());
}

void TestPatientManager::testGetHeightUnit_data()
{
    QTest::addColumn<UnitType>("height");
    QTest::addColumn<UnitType>("result");

    QTest::newRow("cm") << UNIT_CM<< UNIT_CM;
    QTest::newRow("inch") << UNIT_INCH << UNIT_INCH;
    QTest::newRow("not weight unit") << UNIT_KG << UNIT_CM;
    QTest::newRow("NR") << UNIT_NR << UNIT_CM;
    QTest::newRow("None") << UNIT_NONE << UNIT_CM;
}

void TestPatientManager::testGetHeightUnit()
{
    QFETCH(UnitType, height);
    QFETCH(UnitType, result);
    systemConfig.setNumValue("Unit|HeightUnit", static_cast<int>(height));
    systemConfig.saveToDisk();
    QCOMPARE(result, patientManager.getHeightUnit());
}

void TestPatientManager::testMonitor_data()
{
    QTest::addColumn<bool>("status");
    QTest::addColumn<bool>("result");

    QTest::newRow("true") << true << true;
    QTest::newRow("false") << false << false;
}

void TestPatientManager::testMonitor()
{
    QFETCH(bool, status);
    QFETCH(bool, result);

    patientManager.setMonitor(status);
    QCOMPARE(result, patientManager.isMonitoring());

    // test dischargePatient()
    MockDataStorageDirManager mockDataStorageDirManager;
    MockDataStorageDirManager::registerDataStorageDirManager(&mockDataStorageDirManager);
    if (!status)
    {
        EXPECT_CALL(mockDataStorageDirManager, cleanCurData()).Times(1);
    }
    else
    {
        EXPECT_CALL(mockDataStorageDirManager, createDir(true)).Times(1);
    }

    patientManager.dischargePatient();

    if (status)
    {
        // new patient
        PatientInfo info = patientManager.getPatientInfo();
        QCOMPARE(info.bornDate, QDate());
        QCOMPARE(info.blood, PATIENT_BLOOD_TYPE_NULL);
        QCOMPARE(info.height, static_cast<float>(0.0));
        QCOMPARE(QString(info.name), QString());
        QCOMPARE(QString(info.id), QString());
        QCOMPARE(info.sex, PATIENT_SEX_NULL);
        QCOMPARE(info.type, patientManager.getType());
        QCOMPARE(info.weight, static_cast<float>(0.0));
        QCOMPARE(info.pacer, PATIENT_PACER_OFF);
    }

    QVERIFY(Mock::VerifyAndClearExpectations(&mockDataStorageDirManager));
}

void TestPatientManager::testNewPatient_data()
{
    QTest::addColumn<PatientType>("type");

    QTest::newRow("adult") << PATIENT_TYPE_ADULT;
    QTest::newRow("Neo") << PATIENT_TYPE_NEO;
    QTest::newRow("ped") << PATIENT_TYPE_PED;
    QTest::newRow("null") << PATIENT_TYPE_NULL;
    QTest::newRow("nr") << PATIENT_TYPE_NR;
}

void TestPatientManager::testNewPatient()
{
    QFETCH(PatientType, type);
    patientManager.setType(type);

    MockDataStorageDirManager mockDataStorageDirManager;
    MockDataStorageDirManager::registerDataStorageDirManager(&mockDataStorageDirManager);
    EXPECT_CALL(mockDataStorageDirManager, createDir(true)).Times(1);

    MockAlarmIndicator mockAlarmIndicator;
    MockAlarmIndicator::registerAlarmIndicator(&mockAlarmIndicator);
    EXPECT_CALL(mockAlarmIndicator, delAllPhyAlarm()).Times(1);

    MockSystemManager mockSystemManager;
    MockSystemManager::registerSystemManager(&mockSystemManager);
    EXPECT_CALL(mockSystemManager, isSupport(PARAM_NIBP)).WillOnce(Return(true));

    MockNIBPParam mockNIBPParam;
    MockNIBPParam::registerNIBPParam(&mockNIBPParam);
    EXPECT_CALL(mockNIBPParam, clearResult()).Times(1);
    EXPECT_CALL(mockNIBPParam, clearTrendListData()).Times(1);

    patientManager.newPatient();
    PatientInfo info = patientManager.getPatientInfo();
    QCOMPARE(info.bornDate, QDate());
    QCOMPARE(info.blood, PATIENT_BLOOD_TYPE_NULL);
    QCOMPARE(info.height, static_cast<float>(0.0));
    QCOMPARE(QString(info.name), QString());
    QCOMPARE(QString(info.id), QString());
    QCOMPARE(info.sex, PATIENT_SEX_NULL);
    QCOMPARE(info.type, type);
    QCOMPARE(info.weight, static_cast<float>(0.0));
    QCOMPARE(info.pacer, PATIENT_PACER_OFF);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockDataStorageDirManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockSystemManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPParam));
    QCOMPARE(patientManager.isNewPatient(), true);
}

void TestPatientManager::testFinishPatientInfo()
{
    patientManager.finishPatientInfo();
    QCOMPARE(patientManager.isMonitoring(), true);
    QCOMPARE(patientManager.isNewPatient(), false);
}
