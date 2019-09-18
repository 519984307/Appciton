/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/15
 **/

#pragma once
#include <QtTest/QtTest>

class PatientInfoWidget;
class TestPatientInfoWidget : public QObject
{
    Q_OBJECT
public:
    TestPatientInfoWidget()
        : patientInfoWidget(NULL)
    {}
    ~TestPatientInfoWidget(){}

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testClick();
    void testLoadPatientInfo_data();
    void testLoadPatientInfo();

private:
    PatientInfoWidget *patientInfoWidget;
};
