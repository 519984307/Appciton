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

Q_DECLARE_METATYPE(NIBPStateMachineType)

NIBPStateTest::NIBPStateTest()
{
}

void NIBPStateTest::testGetID_data()
{
    QTest::addColumn<unsigned char>("ID");
    QTest::addColumn<unsigned char>("result");
    QTest::newRow("oneDigit") << static_cast<unsigned char>(0x1) << static_cast<unsigned char>(0x1);
    QTest::newRow("twoDigit") << static_cast<unsigned char>(0x22) << static_cast<unsigned char>(0x22);
    QTest::newRow("oneChar") << static_cast<unsigned char>('a') << static_cast<unsigned char>('a');
}

void NIBPStateTest::testGetID()
{
    QFETCH(unsigned char, ID);
    QFETCH(unsigned char, result);
    NIBPState test(ID);
    QCOMPARE(test.getID(), result);
}

void NIBPStateTest::testGetStateMachine_data()
{
    QTest::addColumn<bool>("isMonitor");
    QTest::addColumn<NIBPStateMachineType>("result");
    QTest::newRow("monitorStateMachine") << true << NIBP_STATE_MACHINE_MONITOR;
    QTest::newRow("serviceStateMachine") << false << NIBP_STATE_MACHINE_SERVICE;
}

void NIBPStateTest::testGetStateMachine()
{
    QFETCH(bool, isMonitor);
    QFETCH(NIBPStateMachineType, result);
    unsigned char testID = 0x1;
    NIBPState test(testID);
    NIBPStateMachine* nibpStateMachine = NULL;
    if (isMonitor)
    {
        nibpStateMachine = new MockNIBPMonitorStateMachine();
    }
    else
    {
        nibpStateMachine = new MockNIBPServiceStateMachine();
    }
    test.setStateMachine(nibpStateMachine);
    QCOMPARE(test.getStateMachine()->type(), result);
}

