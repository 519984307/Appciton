/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/3/25
 **/

#include "TestNIBPMonitorStandbyState.h"
#include "MockNIBPCountdownTime.h"
#include "MockNIBPParam.h"
#include "NIBPMonitorStandbyState.h"

using ::testing::Eq;
using ::testing::Mock;
using ::testing::_;
using ::testing::Return;
using ::testing::AnyNumber;

Q_DECLARE_METATYPE(NIBPAutoInterval)
Q_DECLARE_METATYPE(NIBPMode)
Q_DECLARE_METATYPE(NIBPEvent)

class TestNIBPMonitorStandbyStatePrivate
{
public:
    TestNIBPMonitorStandbyStatePrivate() {}
    ~TestNIBPMonitorStandbyStatePrivate(){}
    NIBPMonitorStandbyState monitorStandbyState;
};

TestNIBPMonitorStandbyState::TestNIBPMonitorStandbyState()
    : d_ptr(new TestNIBPMonitorStandbyStatePrivate)
{
}

TestNIBPMonitorStandbyState::~TestNIBPMonitorStandbyState()
{
    delete d_ptr;
}

void TestNIBPMonitorStandbyState::testEnter_data()
{
    QTest::addColumn<NIBPAutoInterval>("interval");
    QTest::addColumn<NIBPMode>("nibpMode");
    QTest::newRow("2.5min manual") << NIBP_AUTO_INTERVAL_2_5 << NIBP_MODE_MANUAL;
    QTest::newRow("2.5min stat") << NIBP_AUTO_INTERVAL_2_5 << NIBP_MODE_STAT;
    QTest::newRow("2.5min NR") << NIBP_AUTO_INTERVAL_2_5 << NIBP_MODE_NR;
    QTest::newRow("2.5min auto")  << NIBP_AUTO_INTERVAL_2_5 << NIBP_MODE_AUTO;
    QTest::newRow("5min auto") << NIBP_AUTO_INTERVAL_5 << NIBP_MODE_AUTO;
    QTest::newRow("10min auto") << NIBP_AUTO_INTERVAL_10 << NIBP_MODE_AUTO;
    QTest::newRow("15min auto") << NIBP_AUTO_INTERVAL_15 << NIBP_MODE_AUTO;
    QTest::newRow("20min auto") << NIBP_AUTO_INTERVAL_20 << NIBP_MODE_AUTO;
    QTest::newRow("30min auto") << NIBP_AUTO_INTERVAL_30 << NIBP_MODE_AUTO;
    QTest::newRow("45min auto") << NIBP_AUTO_INTERVAL_45 << NIBP_MODE_AUTO;
    QTest::newRow("60min auto") << NIBP_AUTO_INTERVAL_60 << NIBP_MODE_AUTO;
    QTest::newRow("90min auto") << NIBP_AUTO_INTERVAL_90 << NIBP_MODE_AUTO;
    QTest::newRow("120min auto") << NIBP_AUTO_INTERVAL_120 << NIBP_MODE_AUTO;
    QTest::newRow("NR auto") << NIBP_AUTO_INTERVAL_NR << NIBP_MODE_AUTO;
}

void TestNIBPMonitorStandbyState::testEnter()
{
    QFETCH(NIBPAutoInterval, interval);
    QFETCH(NIBPMode, nibpMode);
    MockNIBPParam mockNIBPParam;
    NIBPParamInterface::registerNIBPParam(&mockNIBPParam);
    EXPECT_CALL(mockNIBPParam, getMeasurMode).Times(AnyNumber()).WillRepeatedly(testing::Return(nibpMode));
    if (nibpMode == NIBP_MODE_AUTO)
    {
        EXPECT_CALL(mockNIBPParam, setModelText(_));
        EXPECT_CALL(mockNIBPParam, setAutoMeasure(Eq(false)));
        EXPECT_CALL(mockNIBPParam, getAutoInterval).Times(AnyNumber()).WillOnce(testing::Return(interval));
    }
    if (nibpMode == NIBP_MODE_MANUAL)
    {
        EXPECT_CALL(mockNIBPParam, setModelText(_));
    }
    d_ptr->monitorStandbyState.enter();
    QCOMPARE(static_cast<NIBPMonitorStateID>(d_ptr->monitorStandbyState.getID()), NIBP_MONITOR_STANDBY_STATE);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPParam));
}

void TestNIBPMonitorStandbyState::testHandleNIBPEvent_data()
{
    QTest::addColumn<NIBPEvent>("nibpEvent");
    QTest::addColumn<unsigned char>("data");

    QTest::newRow("Module reset") << NIBP_EVENT_MODULE_RESET << (unsigned char)0x00;
    QTest::newRow("Module error") << NIBP_EVENT_MODULE_ERROR << (unsigned char)0x00;
    QTest::newRow("Connect error") << NIBP_EVENT_CONNECTION_ERROR << (unsigned char)0x00;
    QTest::newRow("Connection normal") << NIBP_EVENT_CONNECTION_NORMAL << (unsigned char)0x00;
    QTest::newRow("Timeout") << NIBP_EVENT_TIMEOUT << (unsigned char)0x00;
    QTest::newRow("Current presure") << NIBP_EVENT_CURRENT_PRESSURE << (unsigned char)0x00;
    QTest::newRow("Trigger button measurMode") << NIBP_EVENT_TRIGGER_BUTTON << (unsigned char)0x00;
    QTest::newRow("Trigger patient type") << NIBP_EVENT_TRIGGER_PATIENT_TYPE << (unsigned char)0x00;
    QTest::newRow("Trigger model 0x01") << NIBP_EVENT_TRIGGER_MODEL << (unsigned char)0x01;
    QTest::newRow("Trigger model 0x00") << NIBP_EVENT_TRIGGER_MODEL << (unsigned char)0x00;
    QTest::newRow("NR") << NIBP_EVENT_NR << (unsigned char)0x00;
}

void TestNIBPMonitorStandbyState::testHandleNIBPEvent()
{
    QFETCH(NIBPEvent, nibpEvent);
    QFETCH(unsigned char, data);
    MockNIBPCountdownTime mockNIBPCountdownTime;
    NIBPCountdownTimeInterface::registerNIBPCountTime(&mockNIBPCountdownTime);
    unsigned char str[10] = {data, 0x02};
    MockNIBPParam mockNIBPParam;
    NIBPParamInterface::registerNIBPParam(&mockNIBPParam);

    if (nibpEvent == NIBP_EVENT_TRIGGER_BUTTON)
    {
        EXPECT_CALL(mockNIBPParam, getMeasurMode).Times(AnyNumber()).WillOnce(Return(NIBP_MODE_AUTO));
        EXPECT_CALL(mockNIBPParam, setAutoMeasure(Eq(true)));
    }
    if (nibpEvent == NIBP_EVENT_TRIGGER_MODEL)
    {
        if (str[0] == 0x01)
        {
            EXPECT_CALL(mockNIBPParam, setSTATMeasure(Eq(true)));
            EXPECT_CALL(mockNIBPCountdownTime, STATMeasureStart());
        }
        if (str[0] != 0x01)
        {
            EXPECT_CALL(mockNIBPParam, getSuperMeasurMode).Times(AnyNumber()).WillOnce(testing::Return(NIBP_MODE_AUTO));
            EXPECT_CALL(mockNIBPParam, switchToAuto).Times((AnyNumber()));
            EXPECT_CALL(mockNIBPParam, switchToManual).Times(AnyNumber());
        }
    }
    d_ptr->monitorStandbyState.handleNIBPEvent(nibpEvent, str , 10);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPCountdownTime));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPParam));
}


