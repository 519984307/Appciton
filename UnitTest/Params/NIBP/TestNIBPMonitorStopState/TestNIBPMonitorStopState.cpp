/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by tongzhou fang  fangtongzhou@blmed.cn, 2019/3/20
 **/

#include "TestNIBPMonitorStopState.h"
#include "MockNIBPCountdownTime.h"
#include "MockNIBPParam.h"
#include "NIBPMonitorStopState.h"
#include "NIBPDefine.h"
#include "gtest/gtest.h"
using ::testing::Eq;
using ::testing::Mock;
using ::testing::_;
using ::testing::Return;
using ::testing::AnyNumber;

Q_DECLARE_METATYPE(NIBPMode)
Q_DECLARE_METATYPE(NIBPEvent)
class TestNIBPMonitorStopStatePrivate
{
public:
    TestNIBPMonitorStopStatePrivate() {}
    ~TestNIBPMonitorStopStatePrivate(){}
    NIBPMonitorStopState monitorStopState;
};

TestNIBPMonitorStopState::TestNIBPMonitorStopState()
    : d_ptr(new TestNIBPMonitorStopStatePrivate)
{
}

TestNIBPMonitorStopState::~TestNIBPMonitorStopState()
{
    delete d_ptr;
}

void TestNIBPMonitorStopState::testEnter_data()
{
    QTest::addColumn<NIBPMode>("nibpMode");
    QTest::newRow("STAT") << NIBP_MODE_STAT;
    QTest::newRow("Manual") << NIBP_MODE_MANUAL;
    QTest::newRow("Auto") << NIBP_MODE_AUTO;
}

void TestNIBPMonitorStopState::testEnter()
{
    QFETCH(NIBPMode, nibpMode);
    MockNIBPParam mockNIBPParam;
    NIBPParamInterface::registerNIBPParam(&mockNIBPParam);
    NIBPProviderIFace nibpiface;
    MockNIBPCountdownTime mockNIBPCountdownTime;
    NIBPCountdownTimeInterface::registerNIBPCountTime(&mockNIBPCountdownTime);
    EXPECT_CALL(mockNIBPParam, createSnapshot(Eq(NIBP_ONESHOT_ABORT)));
    EXPECT_CALL(mockNIBPParam, provider).Times(AnyNumber()).WillRepeatedly(testing::ReturnRef(nibpiface));
    EXPECT_CALL(mockNIBPParam, setAdditionalMeasure(Eq(false)));
    EXPECT_CALL(mockNIBPParam, getMeasurMode).Times(AnyNumber()).WillRepeatedly(testing::Return(nibpMode));

    if (nibpMode == NIBP_MODE_STAT)
    {
    EXPECT_CALL(mockNIBPCountdownTime, STATMeasureStop).Times(AnyNumber());
    EXPECT_CALL(mockNIBPParam, setSTATMeasure(Eq(false)));
    EXPECT_CALL(mockNIBPParam, setSTATClose(Eq(true)));
    EXPECT_CALL(mockNIBPParam, setModelText(_));
    }
    else
    {
    EXPECT_CALL(mockNIBPParam, isAutoMeasure).Times(AnyNumber()).WillOnce(Return(true));
    EXPECT_CALL(mockNIBPParam, setModelText(_));
    }
    EXPECT_CALL(mockNIBPParam, setText(_));
    EXPECT_CALL(mockNIBPParam, clearResult).Times(AnyNumber());

    d_ptr->monitorStopState.enter();
    QCOMPARE(static_cast<NIBPMonitorStateID>(d_ptr->monitorStopState.getID()), NIBP_MONITOR_STOPE_STATE);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPParam));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPCountdownTime));
}

void TestNIBPMonitorStopState::testHandleNIBPEvent_data()
{
    QTest::addColumn<NIBPEvent>("nibpEvent");
    QTest::newRow("Monitor STOP") << NIBP_EVENT_MONITOR_STOP;
    QTest::newRow("Monitor Start Measure") << NIBP_EVENT_MONITOR_START_MEASURE;
    QTest::newRow("Monitor Measure Done") << NIBP_EVENT_MONITOR_MEASURE_DONE;
    QTest::newRow("Monitor Get Result") << NIBP_EVENT_MONITOR_GET_RESULT;
}

void TestNIBPMonitorStopState::testHandleNIBPEvent()
{
    QFETCH(NIBPEvent, nibpEvent);
    unsigned char test = (unsigned char)0x01;
    MockNIBPParam mockNIBPParam;
    NIBPParamInterface::registerNIBPParam(&mockNIBPParam);
    if (nibpEvent == NIBP_EVENT_MONITOR_STOP)
    {
    EXPECT_CALL(mockNIBPParam, setText(_)).Times(AnyNumber());
    EXPECT_CALL(mockNIBPParam, clearResult).Times(AnyNumber());
    }
    d_ptr->monitorStopState.handleNIBPEvent(nibpEvent, &test, 1);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPParam));
}
