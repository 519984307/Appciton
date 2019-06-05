/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/13
 **/

#pragma once

#include <QtTest>

class TestPatientManagerPrivate;
class TestPatientManager : public QObject
{
    Q_OBJECT
public:
    TestPatientManager(){}
    ~TestPatientManager(){}

private Q_SLOTS:
    void initTestCase();
    void cleanup();
    void cleanupTestCase();
    void testType_data();
    void testType();
    void testPacermaker_data();
    void testPacermaker();
    void testSex_data();
    void testSex();
    void testBornDate_data();
    void testBornDate();
    void testBlood_data();
    void testBlood();
    void testWeight_data();
    void testWeight();
    void testHeight_data();
    void testHeight();
    void testName_data();
    void testName();
    void testPatientID_data();
    void testPatientID();
    void testBedNum_data();
    void testBedNum();
    void testPatientInfo_data();
    void testPatientInfo();
    void testGetWeightUnit_data();
    void testGetWeightUnit();
    void testGetHeightUnit_data();
    void testGetHeightUnit();
    void testMonitor_data();
    void testMonitor();
    void testNewPatient_data();
    void testNewPatient();
    void testFinishPatientInfo();
};
