/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by tongzhou fang  fangtongzhou@blmed.cn, 2019/3/20
 **/
#include "TestNIBPState.h"

Q_DECLARE_METATYPE(NIBPStateMachine*)
Q_DECLARE_METATYPE(NIBPStateMachineType)

NIBPStateTest::NIBPStateTest()
{
}

void NIBPStateTest::initTestCase()
{
}

void NIBPStateTest::cleanupTestCase()
{
    delete monitor;
    delete service;
}
void NIBPStateTest::testGetID_data()
{
    QTest::addColumn<unsigned char>("ID");
    QTest::addColumn<bool>("result");
    unsigned char a = 0x1;
    unsigned char b = 0x22;
    unsigned char c = 'a';
    QTest::newRow("oneDigit") << a << true;
    QTest::newRow("twoDigit") << b << true;
    QTest::newRow("oneChar") << c << true;
}

void NIBPStateTest::testGetID()
{
    QFETCH(unsigned char, ID);
    QFETCH(bool, result);
    NIBPState test(ID);

    QCOMPARE(test.getID() == ID, result);
}

void NIBPStateTest::testGetStateMachine_data()
{
    monitor = new MockNIBPMonitorStateMachine();
    service = new MockNIBPServiceStateMachine();
    QTest::addColumn<NIBPStateMachine *>("nibpStateMachine");
    QTest::addColumn<NIBPStateMachineType>("result");
    QTest::newRow("monitorStateMachine") << static_cast<NIBPStateMachine*>(monitor) << NIBP_STATE_MACHINE_MONITOR;
    QTest::newRow("serviceStateMachine") << static_cast<NIBPStateMachine*>(service) << NIBP_STATE_MACHINE_SERVICE;
}

void NIBPStateTest::testGetStateMachine()
{
    QFETCH(NIBPStateMachine*, nibpStateMachine);
    QFETCH(NIBPStateMachineType, result);
    unsigned char a = 0x1;
    NIBPState test(a);
    test.setStateMachine(static_cast<NIBPStateMachine*>(nibpStateMachine));
    QCOMPARE(test.getStateMachine()->type(), result);
}

