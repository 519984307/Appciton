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

void TestNIBPMonitorStandbyState::initTestCase()
{
}

void TestNIBPMonitorStandbyState::cleanupTestCase()
{
}

void TestNIBPMonitorStandbyState::testEnter_data()
{
    QTest::addColumn<NIBPAutoInterval>("interval");
    QTest::addColumn<NIBPMode>("nibpMode");
    QTest::newRow("2_5min_manual") << NIBP_AUTO_INTERVAL_2_5 << NIBP_MODE_MANUAL;
    QTest::newRow("2_5min_stat") << NIBP_AUTO_INTERVAL_2_5 << NIBP_MODE_STAT;
    QTest::newRow("2_5min_NR") << NIBP_AUTO_INTERVAL_2_5 << NIBP_MODE_NR;
    QTest::newRow("2_5min_auto")  << NIBP_AUTO_INTERVAL_2_5 << NIBP_MODE_AUTO;
    QTest::newRow("5min_auto") << NIBP_AUTO_INTERVAL_5 << NIBP_MODE_AUTO;
    QTest::newRow("10min_auto") << NIBP_AUTO_INTERVAL_10 << NIBP_MODE_AUTO;
    QTest::newRow("15min_auto") << NIBP_AUTO_INTERVAL_15 << NIBP_MODE_AUTO;
    QTest::newRow("20min_auto") << NIBP_AUTO_INTERVAL_20 << NIBP_MODE_AUTO;
    QTest::newRow("30min_auto") << NIBP_AUTO_INTERVAL_30 << NIBP_MODE_AUTO;
    QTest::newRow("45min_auto") << NIBP_AUTO_INTERVAL_45 << NIBP_MODE_AUTO;
    QTest::newRow("60min_auto") << NIBP_AUTO_INTERVAL_60 << NIBP_MODE_AUTO;
    QTest::newRow("90min_auto") << NIBP_AUTO_INTERVAL_90 << NIBP_MODE_AUTO;
    QTest::newRow("120min_auto") << NIBP_AUTO_INTERVAL_120 << NIBP_MODE_AUTO;
    QTest::newRow("NR_auto") << NIBP_AUTO_INTERVAL_NR << NIBP_MODE_AUTO;
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

    QTest::newRow("MODULE_RESET") << NIBP_EVENT_MODULE_RESET << (unsigned char)0x00;
    QTest::newRow("MODULE_ERROR") << NIBP_EVENT_MODULE_ERROR << (unsigned char)0x00;
    QTest::newRow("CONNECTION_ERROR") << NIBP_EVENT_CONNECTION_ERROR << (unsigned char)0x00;
    QTest::newRow("CONNECTION_NORMAL") << NIBP_EVENT_CONNECTION_NORMAL << (unsigned char)0x00;
    QTest::newRow("TIMEOUT") << NIBP_EVENT_TIMEOUT << (unsigned char)0x00;
    QTest::newRow("CURRENT_PRESSURE") << NIBP_EVENT_CURRENT_PRESSURE << (unsigned char)0x00;
    QTest::newRow("TRIGGER_BUTTON_MeasurMode") << NIBP_EVENT_TRIGGER_BUTTON << (unsigned char)0x00;
    QTest::newRow("TRIGGER_PATIENT_TYPE") << NIBP_EVENT_TRIGGER_PATIENT_TYPE << (unsigned char)0x00;
    QTest::newRow("TRIGGER_MODEL_0x01") << NIBP_EVENT_TRIGGER_MODEL << (unsigned char)0x01;
    QTest::newRow("TRIGGER_MODEL_0x00") << NIBP_EVENT_TRIGGER_MODEL << (unsigned char)0x00;
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


