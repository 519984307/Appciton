/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/

#include "TestAlarm.h"
#include "Alarm.h"
#include <QVariant>
#include "MockAlarmStateMachine.h"
#include "MockParamInfo.h"
#include <QTime>
#include "MockTrendDataStorageManager.h"
#include "MockAlarmIndicator.h"
#include "MockTrendCache.h"
#include "IConfig.h"
#include "MockEventStorageManager.h"

Q_DECLARE_METATYPE(AlarmStatus)
Q_DECLARE_METATYPE(AlarmLimitIFace *)
Q_DECLARE_METATYPE(AlarmOneShotIFace *)
Q_DECLARE_METATYPE(ParamID)
Q_DECLARE_METATYPE(SubParamID)

using ::testing::_;
using ::testing::Mock;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArgReferee;

class TestAlarmPrivate
{
public:
    TestAlarmPrivate()
        : isCleanup(true)
    {}
    bool isCleanup;
};

TestAlarm::TestAlarm()
    : d_ptr(new TestAlarmPrivate)
{
}

TestAlarm::~TestAlarm()
{
    delete d_ptr;
}

void TestAlarm::initTestCase()
{
}

void TestAlarm::cleanupTestCase()
{
}

void TestAlarm::cleanup()
{
    if (d_ptr->isCleanup)
    {
        alertor.clear();
    }
}

void TestAlarm::testUpdateMuteKeyStatus_data()
{
    QTest::addColumn<bool>("isPressed");

    QTest::newRow("press") << true;

    QTest::newRow("not press") << false;
}

void TestAlarm::testUpdateMuteKeyStatus()
{
    QFETCH(bool, isPressed);

    MockAlarmStateMachine mockAlarmStateMachine;
    MockAlarmStateMachine::registerAlarmStateMachine(&mockAlarmStateMachine);
    if (isPressed)
    {
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_PRESSED, _, _));
    }
    else
    {
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_RELEASED, _, _));
    }
    alertor.updateMuteKeyStatus(isPressed);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}

void TestAlarm::testUpdateResetKeyStatus_data()
{
    QTest::addColumn<bool>("isPressed");

    QTest::newRow("press") << true;

    QTest::newRow("not press") << false;
}

void TestAlarm::testUpdateResetKeyStatus()
{
    QFETCH(bool, isPressed);
    MockAlarmStateMachine mockAlarmStateMachine;
    MockAlarmStateMachine::registerAlarmStateMachine(&mockAlarmStateMachine);
    if (isPressed)
    {
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_RESET_BTN_PRESSED, _, _));
    }
    else
    {
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_RESET_BTN_RELEASED, _, _));
    }
    alertor.updateResetKeyStatus(isPressed);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}

void TestAlarm::testAddAlarmStatus_data()
{
    QTest::addColumn<AlarmStatus>("alarmStatus");
    QTest::addColumn<int>("alarmStatusListCount");

    QTest::newRow("normal") << ALARM_STATUS_NORMAL << 1;
    QTest::newRow("pause") << ALARM_STATUS_PAUSE << 2;
    QTest::newRow("normal repeatly") << ALARM_STATUS_NORMAL << 2;
    QTest::newRow("reset") << ALARM_STATUS_RESET << 3;
    QTest::newRow("normal repeatly again") << ALARM_STATUS_NORMAL << 3;
}

void TestAlarm::testAddAlarmStatus()
{
    d_ptr->isCleanup = false;
    QFETCH(AlarmStatus, alarmStatus);
    QFETCH(int, alarmStatusListCount);

    alertor.addAlarmStatus(alarmStatus);
    QCOMPARE(alertor.getAlarmStatusCount(), alarmStatusListCount);
}

void TestAlarm::testGetAlarmLimitIFace_data()
{
    QTest::addColumn<AlarmLimitIFace *>("source");
    QTest::addColumn<bool>("result");
    QTest::addColumn<bool>("isAddSource");

    AlarmLimitIFace *s1 = new LimitAlarm;
    QTest::newRow("none param ID") << s1 << false << true;
    LimitAlarm *s2 = new LimitAlarm;
    s2->setParamID(PARAM_ECG);
    s2->setSubParamID(SUB_PARAM_HR_PR);
    QTest::newRow("ecg param ID") << static_cast<AlarmLimitIFace *>(s2) << false << true;

    LimitAlarm *s3 = new LimitAlarm;
    s3->setParamID(PARAM_SPO2);
    s3->setSubParamID(SUB_PARAM_SPO2);
    QTest::newRow("not exist param ID") << static_cast<AlarmLimitIFace *>(s3) << true << false;
}

void TestAlarm::testGetAlarmLimitIFace()
{
    d_ptr->isCleanup = true;
    QFETCH(AlarmLimitIFace *, source);
    QFETCH(bool, result);
    QFETCH(bool, isAddSource);
    if (isAddSource)
    {
        alertor.addLimtSource(source);
    }

    MockParamInfo mockParamInfo;
    MockParamInfo::registerParamInfo(&mockParamInfo);
    EXPECT_CALL(mockParamInfo, getParamID).WillOnce(Return(source->getParamID()));
    AlarmLimitIFace *ret = alertor.getAlarmLimitIFace(source->getSubParamID(source->getParamID()));
    QCOMPARE(ret == NULL, result);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockParamInfo));
}

void TestAlarm::testGetAlarmOneShotIFace_data()
{
    QTest::addColumn<AlarmOneShotIFace *>("source");
    QTest::addColumn<SubParamID>("subParamID");
    QTest::addColumn<bool>("result");
    QTest::addColumn<bool>("isAddSource");

    AlarmOneShotIFace *s1 = new OneShotAlarm;
    QTest::newRow("none param ID") << s1 << s1->getSubParamID(0) << false << true;
    OneShotAlarm *s2 = new OneShotAlarm;
    s2->setParamID(PARAM_ECG);
    QTest::newRow("ecg param ID") << static_cast<AlarmOneShotIFace *>(s2)
                                  << SUB_PARAM_HR_PR
                                  << false
                                  << true;

    OneShotAlarm *s3 = new OneShotAlarm;
    s3->setParamID(PARAM_SPO2);
    QTest::newRow("not exist param ID") << static_cast<AlarmOneShotIFace *>(s3)
                                        << SUB_PARAM_SPO2
                                        << true
                                        << false;
}

void TestAlarm::testGetAlarmOneShotIFace()
{
    d_ptr->isCleanup = true;
    QFETCH(AlarmOneShotIFace *, source);
    QFETCH(SubParamID, subParamID);
    QFETCH(bool, result);
    QFETCH(bool, isAddSource);

    if (isAddSource)
    {
        alertor.addOneShotSource(source);
    }

    MockParamInfo mockParamInfo;
    MockParamInfo::registerParamInfo(&mockParamInfo);
    EXPECT_CALL(mockParamInfo, getParamID).WillOnce(Return(source->getParamID()));
    AlarmOneShotIFace *ret = alertor.getAlarmOneShotIFace(subParamID);
    QCOMPARE(ret == NULL, result);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockParamInfo));
}



void TestAlarm::testGetPhyAlarmMessage_data()
{
    QTest::addColumn<ParamID>("paramId");
    QTest::addColumn<int>("alarmType");
    QTest::addColumn<bool>("isOneshot");
    QTest::addColumn<QString>("message");

    QTest::newRow("none") << PARAM_NONE << 0 << false << "";
    QTest::newRow("ECG/oneshot") << PARAM_ECG << 0 << true << "ECGArrASYSTOLE";
    QTest::newRow("ECG/limit") << PARAM_ECG << 10 << false << "";
    QTest::newRow("dup ECG/oneshot") << PARAM_DUP_ECG << 0 << true << "";
    QTest::newRow("dup ECG/limit") << PARAM_DUP_ECG << 1 << false << "HRHigh";
    QTest::newRow("spo2/oneshot") << PARAM_SPO2 << 7 << true << "SPO2LowPerfusion";
    QTest::newRow("spo2/limit") << PARAM_SPO2 << 2 << false << "";
    QTest::newRow("resp/limit") << PARAM_RESP << 0 << false << "";
    QTest::newRow("resp/oneshot") << PARAM_RESP << 10 << true << "";
    QTest::newRow("nibp/limit") << PARAM_NIBP << 2 << false << "NIBPDIALow";
    QTest::newRow("nibp/oneshot") << PARAM_NIBP << 0 << true << "NIBPNoneError";
    QTest::newRow("co2/limit") << PARAM_CO2 << 3 << false << "CO2FiCO2High";
    QTest::newRow("co2/oneshot") << PARAM_CO2 << 0 << true << "Apnea";
    QTest::newRow("temp/limit") << PARAM_TEMP << 11 << false << "";
    QTest::newRow("temp/oneshot") << PARAM_TEMP << 0 << true << "TEMP1SensorOff";
}

void TestAlarm::testGetPhyAlarmMessage()
{
    d_ptr->isCleanup = true;
    QFETCH(ParamID, paramId);
    QFETCH(int, alarmType);
    QFETCH(bool, isOneshot);
    QFETCH(QString, message);

    QCOMPARE(alertor.getPhyAlarmMessage(paramId, alarmType, isOneshot), message);
}

void TestAlarm::testGetAlarmMessage_data()
{
    QTest::addColumn<AlarmLimitIFace *>("limitSource");
    QTest::addColumn<AlarmOneShotIFace *>("oneshotSource");
    QTest::addColumn<ParamID>("paramId");
    QTest::addColumn<int>("alarmType");
    QTest::addColumn<bool>("isOneshotAlarm");
    QTest::addColumn<QString>("message");

    LimitAlarm *limitAlarm0 = new LimitAlarm;
    QTest::newRow("limit source/normal id") << static_cast<AlarmLimitIFace *>(limitAlarm0)
                                    << static_cast<AlarmOneShotIFace *>(NULL)
                                    << limitAlarm0->getParamID()
                                    << 0
                                    << false
                                    << limitAlarm0->toString(0);

    LimitAlarm *limitAlarm1 = new LimitAlarm;
    QTest::newRow("limit source/invaild id") << static_cast<AlarmLimitIFace *>(limitAlarm1)
                                             << static_cast<AlarmOneShotIFace *>(NULL)
                                             << limitAlarm1->getParamID()
                                             << 3
                                             << false
                                             << limitAlarm1->toString(3);

    OneShotAlarm *oneshotAlarm0 = new OneShotAlarm;
    QTest::newRow("oneshot source/normal id") << static_cast<AlarmLimitIFace *>(NULL)
                                    << static_cast<AlarmOneShotIFace *>(oneshotAlarm0)
                                    << oneshotAlarm0->getParamID()
                                    << 0
                                    << true
                                    << oneshotAlarm0->toString(0);

    OneShotAlarm *oneshotAlarm1 = new OneShotAlarm;
    QTest::newRow("oneshot source/invaild id") << static_cast<AlarmLimitIFace *>(NULL)
                                             << static_cast<AlarmOneShotIFace *>(oneshotAlarm1)
                                             << oneshotAlarm1->getParamID()
                                             << 6
                                             << true
                                             << oneshotAlarm1->toString(6);
}

void TestAlarm::testGetAlarmMessage()
{
    d_ptr->isCleanup = true;
    QFETCH(AlarmLimitIFace *, limitSource);
    QFETCH(AlarmOneShotIFace *, oneshotSource);
    QFETCH(ParamID, paramId);
    QFETCH(int, alarmType);
    QFETCH(bool, isOneshotAlarm);
    QFETCH(QString, message);

    if (isOneshotAlarm)
    {
        alertor.addOneShotSource(oneshotSource);
    }
    else
    {
        alertor.addLimtSource(limitSource);
    }

    QString result = QString(alertor.getAlarmMessage(paramId, alarmType, isOneshotAlarm));
    QCOMPARE(result, message);
}

void TestAlarm::testMainRunLimitSource_data()
{
    QTest::addColumn<AlarmLimitIFace *>("source");
    QTest::addColumn<AlarmStatus>("alarmStatus");
    QTest::addColumn<int>("isLock");
    QTest::addColumn<bool>("isNeedAddAlarm");
    QTest::addColumn<bool>("isCleanup");

    LimitAlarm *s1 = new LimitAlarm;
    QTest::newRow("normal state/alarm first/nolatch") << static_cast<AlarmLimitIFace *>(s1)
                                                      << ALARM_STATUS_NORMAL << 0
                                                      << false
                                                      << false;
    QTest::newRow("normal state/alarm second/nolatch") << static_cast<AlarmLimitIFace *>(s1)
                                                       << ALARM_STATUS_NORMAL << 0
                                                       << false
                                                       << false;
    QTest::newRow("normal state/alarm thrid/nolatch") << static_cast<AlarmLimitIFace *>(s1)
                                                      << ALARM_STATUS_NORMAL << 0
                                                      << true
                                                      << false;

    QTest::newRow("normal state/alarm fourth/nolatch") << static_cast<AlarmLimitIFace *>(s1)
                                                      << ALARM_STATUS_NORMAL << 0
                                                      << false
                                                      << false;
    QTest::newRow("normal state/alarm fifth/nolatch") << static_cast<AlarmLimitIFace *>(s1)
                                                       << ALARM_STATUS_NORMAL << 0
                                                       << false
                                                       << false;
    QTest::newRow("normal state/alarm sixth/nolatch") << static_cast<AlarmLimitIFace *>(s1)
                                                      << ALARM_STATUS_NORMAL << 0
                                                      << true
                                                      << true;

    LimitAlarm *s2 = new LimitAlarm;
    QTest::newRow("normal state/alarm first/latch") << static_cast<AlarmLimitIFace *>(s2)
                                                      << ALARM_STATUS_NORMAL << 1
                                                      << false
                                                      << false;
    QTest::newRow("normal state/alarm second/latch") << static_cast<AlarmLimitIFace *>(s2)
                                                       << ALARM_STATUS_NORMAL << 1
                                                       << false
                                                       << false;
    QTest::newRow("normal state/alarm thrid/latch") << static_cast<AlarmLimitIFace *>(s2)
                                                      << ALARM_STATUS_NORMAL << 1
                                                      << true
                                                      << false;

    QTest::newRow("normal state/alarm fourth/latch") << static_cast<AlarmLimitIFace *>(s2)
                                                      << ALARM_STATUS_NORMAL << 1
                                                      << false
                                                      << false;
    QTest::newRow("normal state/alarm fifth/latch") << static_cast<AlarmLimitIFace *>(s2)
                                                       << ALARM_STATUS_NORMAL << 1
                                                       << false
                                                       << false;
    QTest::newRow("normal state/alarm sixth/latch") << static_cast<AlarmLimitIFace *>(s2)
                                                      << ALARM_STATUS_NORMAL << 1
                                                      << true
                                                      << true;

    LimitAlarm *s3 = new LimitAlarm;
    QTest::newRow("pause state/nolatch") << static_cast<AlarmLimitIFace *>(s3)
                                         << ALARM_STATUS_RESET << 0
                                         << false
                                         << false;
    QTest::newRow("pause state/nolatch") << static_cast<AlarmLimitIFace *>(s3)
                                         << ALARM_STATUS_RESET << 0
                                         << false
                                         << false;
    QTest::newRow("reset state/nolatch") << static_cast<AlarmLimitIFace *>(s3)
                                         << ALARM_STATUS_RESET << 0
                                         << true
                                         << true;
}

static bool compareAlarmInfoList(QList<Alarm::AlarmInfo> &list1, QList<Alarm::AlarmInfo> &list2)    /* NOLINT */
{
    QList<Alarm::AlarmInfo>::Iterator iter = list1.begin();
    for (; iter != list1.end(); iter++)
    {
        if (!list2.contains(*iter))
        {
            return false;
        }
    }
    return true;
}

void TestAlarm::testMainRunLimitSource()
{
    QFETCH(AlarmLimitIFace *, source);  // 报警源
    QFETCH(AlarmStatus, alarmStatus);   // 报警状态
    QFETCH(int, isLock);                // 报警的栓锁状态
    QFETCH(bool, isNeedAddAlarm);       // 是否新发生的报警
    QFETCH(bool, isCleanup);            // 是否需要清除报警池中的报警
    d_ptr->isCleanup = isCleanup;

    static bool lastAlarmStatus[ALARM_COUNT] = {false, false, false, false};
    static QList<AlarmLimitIFace *> sourceList;
    if (!sourceList.contains(source))
    {
        alertor.addLimtSource(source);
        sourceList.append(source);
    }
    alertor.addAlarmStatus(alarmStatus);
    unsigned t = QTime::currentTime().elapsed();
    alertor.setLatchLockSta(isLock);
    QList<Alarm::AlarmInfo> alarmList;

    MockAlarmIndicator mockAlarmIndicator;
    MockAlarmIndicator::registerAlarmIndicator(&mockAlarmIndicator);
    MockTrendCache mockTrendCache;
    MockTrendCache::registerTrendCache(&mockTrendCache);
    MockAlarmStateMachine mockAlarmStateMachine;
    MockAlarmStateMachine::registerAlarmStateMachine(&mockAlarmStateMachine);
    MockEventStorageManager mockEventStorageManager;
    MockEventStorageManager::registerEventStorageManager(&mockEventStorageManager);
    EXPECT_CALL(mockAlarmIndicator, publishAlarm(_));
    int delInfoCount = 0;
    int newPhyAlarm = 0;
    int latchCount = 0;
    for (int i = 0; i < source->getAlarmSourceNR(); i++)
    {
        if (alarmStatus == ALARM_STATUS_PAUSE)
        {
            continue;
        }
        else if (alarmStatus == ALARM_STATUS_RESET)
        {
            isLock = false;
        }

        // 记录移除报警的个数
        if (!source->isAlarmEnable(i))
        {
            delInfoCount++;
        }
        else
        {
            if (!isLock && source->getCompare(0, i) == 0)
            {
                // 若报警没被栓锁且没有超限，则删除该报警
                delInfoCount++;
            }
        }

        // 记录栓锁报警个数
        if (!source->isAlarmEnable(i) && isLock && lastAlarmStatus[i])
        {
            latchCount++;
        }
        else if (source->getCompare(0, i) == 0 && isLock && lastAlarmStatus[i]
                 && isNeedAddAlarm && source->isAlarmEnable(i))
        {
            // 若报警没有超限，但是被栓锁了且上一报警状态是报警的、报警源报警使能
            latchCount++;
        }

        if (isNeedAddAlarm && source->getCompare(0, i) != 0)
        {
            // 添加超限报警
            EXPECT_CALL(mockAlarmIndicator, addAlarmInfo(t, ALARM_TYPE_PHY,
                                                         source->getAlarmPriority(i),
                                                         source->toString(i),
                                                         source, i,
                                                         _, _)).WillOnce(Return(true));

            newPhyAlarm++;
        }
        if (source->isAlarmEnable(i) && source->getCompare(0, i) != 0)
        {
            // 发生超限报警时
            if (isLock || lastAlarmStatus[i])
            {
                // 被栓锁或上次报警状态为报警，则检测报警是否存在
                EXPECT_CALL(mockAlarmIndicator, checkAlarmIsExist(ALARM_TYPE_PHY, source->toString(i)))
                        .Times(::testing::AnyNumber())
                        .WillRepeatedly(Return(false));
            }
        }

        // record last alarm status
        if (source->getCompare(0, i) != 0 && isNeedAddAlarm)
        {
            lastAlarmStatus[i] = true;
        }
        else
        {
            lastAlarmStatus[i] = false;
        }

        if (lastAlarmStatus[i])
        {
            // prepare data for test getCurrentPhyAlarmInfo
            Alarm::AlarmInfo info;
            info.paramid = source->getParamID();
            info.isOneshot = false;
            info.alarmType = i;
            info.timestamp = t;
            info.order = i;
            alarmList.append(info);
        }
    }

    if (delInfoCount)
    {
        // 删除报警
        EXPECT_CALL(mockAlarmIndicator, delAlarmInfo(ALARM_TYPE_PHY, NULL)).Times(delInfoCount);
    }

    if (newPhyAlarm)
    {
        // 有新发生的超限报警，则报警状态机处理新发生的生理报警
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_NEW_PHY_ALARM, _, _)).Times(newPhyAlarm);

        // 存储超限报警事件
        EXPECT_CALL(mockEventStorageManager, triggerAlarmEvent(_, source->getWaveformID(0), t)).Times(newPhyAlarm);
    }

    if (latchCount)
    {
        // 处理每个报警的栓锁状态
        EXPECT_CALL(mockAlarmIndicator, latchAlarmInfo(ALARM_TYPE_PHY, NULL))
                .Times(latchCount)
                .WillRepeatedly(Return(true));
    }

    TrendCacheData cacheData;
    /*
     *  set a valid value for SUB_PARAM_NONE, because Limit Alarm sub param id is none
     * and need to have a valid value
     */
    cacheData.values[SUB_PARAM_NONE] = 0;
    EXPECT_CALL(mockTrendCache, getTrendData(t, _)).Times(source->getAlarmSourceNR()).
            WillRepeatedly(DoAll(SetArgReferee<1>(cacheData), Return(true)));

    alertor.mainRun(t);

    // test getCurrentPhyAlarmInfo
    QList<Alarm::AlarmInfo> alarmInfoList = alertor.getCurrentPhyAlarmInfo();
    QCOMPARE(compareAlarmInfoList(alarmList, alarmInfoList), true);

    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockTrendCache));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockEventStorageManager));
}

void TestAlarm::testMainRunOneshotSource_data()
{
    QTest::addColumn<AlarmOneShotIFace *>("source");
    QTest::addColumn<AlarmStatus>("alarmStatus");
    QTest::addColumn<bool>("isLock");
    QTest::addColumn<bool>("isChangeTechAlarm");
    QTest::addColumn<bool>("isChangePhyAlarm");
    QTest::addColumn<bool>("isChangeRemove");
    QTest::addColumn<bool>("isChangeEnable");
    QTest::addColumn<bool>("isCleanup");
    QTest::addColumn<bool>("isNeedAddAlarm");

    OneShotAlarm *s1 = new OneShotAlarm;
    QTest::newRow("normal state/alarm first/nolatch") << static_cast<AlarmOneShotIFace *>(s1)
                                                      << ALARM_STATUS_NORMAL
                                                      << false
                                                      << false
                                                      << false
                                                      << false
                                                      << false
                                                      << false
                                                      << false;
    QTest::newRow("normal state/alarm second/nolatch") << static_cast<AlarmOneShotIFace *>(s1)
                                                       << ALARM_STATUS_NORMAL
                                                       << false
                                                       << false
                                                       << false
                                                       << false
                                                       << false
                                                       << false
                                                       << false;
    QTest::newRow("normal state/alarm third/nolatch") << static_cast<AlarmOneShotIFace *>(s1)
                                                      << ALARM_STATUS_NORMAL
                                                      << false
                                                      << false
                                                      << false
                                                      << false
                                                      << false
                                                      << false
                                                      << true;

    QTest::newRow("normal state/alarm/latch") << static_cast<AlarmOneShotIFace *>(s1)
                                        << ALARM_STATUS_NORMAL
                                        << true
                                        << false
                                        << false
                                        << false
                                        << false
                                        << false
                                        << false;
    QTest::newRow("open latch, test tech oneshot cancel alarm which is alarm last time")
                                                        << static_cast<AlarmOneShotIFace *>(s1)
                                                        << ALARM_STATUS_NORMAL
                                                        << true
                                                        << true
                                                        << false
                                                        << false
                                                        << false
                                                        << false
                                                        << false;
    QTest::newRow("open latch, test tech oneshot alarm which doesn't alarm last time")
                                                        << static_cast<AlarmOneShotIFace *>(s1)
                                                        << ALARM_STATUS_NORMAL
                                                        << true
                                                        << true
                                                        << false
                                                        << false
                                                        << false
                                                        << false
                                                        << false;
    QTest::newRow("open latch, test phy oneshot alarm which doesn't alarm last time")
                                                        << static_cast<AlarmOneShotIFace *>(s1)
                                                        << ALARM_STATUS_NORMAL
                                                        << true
                                                        << false
                                                        << true
                                                        << false
                                                        << false
                                                        << false
                                                        << false;
    QTest::newRow("open latch, test phy oneshot cancel alarm which is alarm last time")
                                                        << static_cast<AlarmOneShotIFace *>(s1)
                                                        << ALARM_STATUS_NORMAL
                                                        << true
                                                        << false
                                                        << true
                                                        << false
                                                        << false
                                                        << false
                                                        << false;

    QTest::newRow("alarm phy alarm")
                                    << static_cast<AlarmOneShotIFace *>(s1)
                                    << ALARM_STATUS_NORMAL
                                    << true
                                    << false
                                    << true
                                    << true
                                    << false
                                    << false
                                    << false;

    QTest::newRow("test remove alarm which is alarm last time")
                                                        << static_cast<AlarmOneShotIFace *>(s1)
                                                        << ALARM_STATUS_NORMAL
                                                        << false
                                                        << false
                                                        << false
                                                        << true
                                                        << false
                                                        << false
                                                        << true;
    QTest::newRow("test enable alarm which is alarm last time")
                                                        << static_cast<AlarmOneShotIFace *>(s1)
                                                        << ALARM_STATUS_NORMAL
                                                        << false
                                                        << false
                                                        << false
                                                        << false
                                                        << true
                                                        << false
                                                        << false;

    QTest::newRow("test in pause state")<< static_cast<AlarmOneShotIFace *>(s1)
                                        << ALARM_STATUS_PAUSE
                                        << false
                                        << false
                                        << false
                                        << false
                                        << false
                                        << false
                                        << false;

    QTest::newRow("test in reset state")<< static_cast<AlarmOneShotIFace *>(s1)
                                        << ALARM_STATUS_RESET
                                        << false
                                        << false
                                        << false
                                        << false
                                        << false
                                        << true
                                        << true;
}

void TestAlarm::testMainRunOneshotSource()
{
    QFETCH(AlarmOneShotIFace *, source);    // 报警源
    QFETCH(AlarmStatus, alarmStatus);       // 报警状态
    QFETCH(bool, isLock);                   // 栓锁状态
    QFETCH(bool, isChangeTechAlarm);        // 控制技术报警是否发生报警
    QFETCH(bool, isChangePhyAlarm);         // 控制生理报警是否发生报警
    QFETCH(bool, isChangeRemove);           // 控制报警是否被移除
    QFETCH(bool, isChangeEnable);           // 控制报警是否使能
    QFETCH(bool, isCleanup);                // 是否需要清除报警池中的报警
    QFETCH(bool, isNeedAddAlarm);           // 是否需要添加报警（3次确认报警后该标志为true）
    d_ptr->isCleanup = isCleanup;

    static bool lastOneShotAlarmStatus[ALARM_COUNT] = {false, false, false, false};

    if (isChangeTechAlarm)
    {
        firstIdAlarm = !firstIdAlarm;
    }
    if (isChangePhyAlarm)
    {
        fourthIdAlarm = !fourthIdAlarm;
    }
    if (isChangeRemove)
    {
        thirdIdRemove = !thirdIdRemove;
    }
    if (isChangeEnable)
    {
        firstIdEnable = !firstIdEnable;
    }
    static QList<AlarmOneShotIFace *> sourceList;
    if (!sourceList.contains(source))
    {
        alertor.addOneShotSource(source);
        sourceList.append(source);
    }
    alertor.addAlarmStatus(alarmStatus);
    unsigned t = QTime::currentTime().elapsed();
    alertor.setLatchLockSta(isLock);

    MockAlarmIndicator mockAlarmIndicator;
    MockAlarmIndicator::registerAlarmIndicator(&mockAlarmIndicator);
    MockTrendCache mockTrendCache;
    MockTrendCache::registerTrendCache(&mockTrendCache);
    TrendCacheData cacheData;
    /*
     *  set a valid value for SUB_PARAM_NONE, because Limit Alarm sub param id is none
     * and need to have a valid value
     */
    cacheData.values[SUB_PARAM_NONE] = 0;
    EXPECT_CALL(mockTrendCache, getTrendData(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(cacheData), Return(true)));
    MockAlarmStateMachine mockAlarmStateMachine;
    MockAlarmStateMachine::registerAlarmStateMachine(&mockAlarmStateMachine);
    MockEventStorageManager mockEventStorageManager;
    MockEventStorageManager::registerEventStorageManager(&mockEventStorageManager);
    EXPECT_CALL(mockAlarmIndicator, publishAlarm(_));

    int machineCount = 0;
    for (int i = 0; i < source->getAlarmSourceNR(); i++)
    {
        if ((alarmStatus == ALARM_STATUS_PAUSE || alarmStatus == ALARM_STATUS_RESET)
                && source->getAlarmType(i) != ALARM_TYPE_TECH)
        {
            // 生理报警在报警暂停复位后状态时，清空所有的上一次报警状态
            lastOneShotAlarmStatus[i] = false;
            continue;
        }
        if (source->isNeedRemove(i) && lastOneShotAlarmStatus[i])
        {
            // 若上次报警发生中，这次要被移除，则让AlarmIndicator移除报警
            EXPECT_CALL(mockAlarmIndicator, delAlarmInfo(source->getAlarmType(i), source->toString(i)));
            lastOneShotAlarmStatus[i] = false;
            continue;
        }
        if (!source->isAlarmEnable(i))
        {
            if (lastOneShotAlarmStatus[i])
            {
                // 上次报警发生，此时报警去使能了
                if (source->getAlarmType(i) != ALARM_TYPE_TECH && isLock)
                {
                    // 栓锁了的生理报警，则栓锁报警
                    EXPECT_CALL(mockAlarmIndicator, latchAlarmInfo(source->getAlarmType(i), source->toString(i)))
                            . WillOnce(Return(true));
                }
                else
                {
                    // 删除非栓锁的报警和技术报警
                    EXPECT_CALL(mockAlarmIndicator, delAlarmInfo(source->getAlarmType(i), source->toString(i)));
                }
            }
            lastOneShotAlarmStatus[i] = false;
            continue;
        }

        if (!source->isNeedRemove(i) && source->isAlarmEnable(i) && source->isAlarmed(i)
                && !lastOneShotAlarmStatus[i] && isNeedAddAlarm)
        {
            // 若此报警不需移除，且允许报警，且发生了报警，且上次没有发生报警，同时满足上述条件后，才可添加报警
            machineCount++;
            EXPECT_CALL(mockAlarmIndicator, addAlarmInfo(t, source->getAlarmType(i),
                                                         source->getAlarmPriority(i),
                                                         source->toString(i),
                                                         source, i,
                                                         _, _)).WillOnce(Return(true));
        }

        if (!source->isAlarmed(i))
        {
            if (lastOneShotAlarmStatus[i])
            {
                // 上次发生了报警，现在报警撤销了
                if (isLock && source->getAlarmType(i) != ALARM_TYPE_TECH)
                {
                    if (isNeedAddAlarm)
                    {
                        // 若为栓锁的生理报警，则栓锁报警
                        EXPECT_CALL(mockAlarmIndicator, latchAlarmInfo(source->getAlarmType(i), source->toString(i)))
                                . WillOnce(Return(false));
                        lastOneShotAlarmStatus[i] = false;
                    }
                }
                else
                {
                    // 删除非栓锁的生理报警和技术报警
                    EXPECT_CALL(mockAlarmIndicator, delAlarmInfo(source->getAlarmType(i), source->toString(i)));
                    lastOneShotAlarmStatus[i] = false;
                }
            }
            continue;
        }
        else
        {
            if (lastOneShotAlarmStatus[i])
            {
                // 上次发生了报警，现在仍然在报警状态下
                if (isLock && source->getAlarmType(i) != ALARM_TYPE_TECH && isNeedAddAlarm)
                {
                    // 检测生理栓锁报警是否存在和更新报警栓锁状态
                    EXPECT_CALL(mockAlarmIndicator, checkAlarmIsExist(source->getAlarmType(i), source->toString(i)))
                            .WillOnce(Return(true));
                    EXPECT_CALL(mockAlarmIndicator, updateLatchAlarmInfo(source->toString(i), false));
                }
                continue;
            }
        }

        if (source->isAlarmed(i) && source->getAlarmType(i) != ALARM_TYPE_TECH && isNeedAddAlarm)
        {
            // 存储生理报警事件
            EXPECT_CALL(mockEventStorageManager, triggerAlarmEvent(_, source->getWaveformID(i), t));
        }

        if (!source->isNeedRemove(i) && isNeedAddAlarm)
        {
            // 非移除报警时，才记录上一时刻的报警状态
            lastOneShotAlarmStatus[i] = source->isAlarmed(i);
        }
    }

    EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_NEW_PHY_ALARM, _, _)).Times(machineCount);

    alertor.mainRun(t);

    // test getOneShotAlarmStatus
    bool ret = alertor.getOneShotAlarmStatus(source, 0);
    QCOMPARE(ret, lastOneShotAlarmStatus[0]);

    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmIndicator));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockTrendCache));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockEventStorageManager));
}

