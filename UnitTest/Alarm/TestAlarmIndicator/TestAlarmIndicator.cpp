/**
 ** This file is part of the unittest project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/19
 **/

#include "TestAlarmIndicator.h"
#include "AlarmIndicator.h"
#include "AlarmInfoBarWidget.h"
#include <QTime>
#include <QVariant>
#include <QDebug>
#include <stdlib.h>
#include "MockAlarmInfoBarWidget.h"
#include "MockSoundManager.h"
#include "MockLightManager.h"
#include "MockAlarmStateMachine.h"
#include "MockAlarm.h"
#include "IConfig.h"

Q_DECLARE_METATYPE(AlarmParamIFace *)
Q_DECLARE_METATYPE(AlarmType)
Q_DECLARE_METATYPE(AlarmInfoNode)
Q_DECLARE_METATYPE(AlarmPriority)
Q_DECLARE_METATYPE(AlarmStatus)

using ::testing::Mock;
using ::testing::_;
using ::testing::AnyNumber;
using ::testing::NiceMock;
using ::testing::Return;

static const char *useCase[ALARM_INFO_POOL_LEN + 1] = {"01", "02", "03", "04", "05",
                                                   "06", "07", "08", "09", "10",
                                                   "11", "12", "13", "14", "15",
                                                   "16", "17", "18", "19", "20",
                                                   "21", "22", "23", "24", "25",
                                                   "26", "27", "28", "29", "30",
                                                   "31", "32", "33", "34", "35",
                                                   "36", "37", "38", "39", "40",
                                                   "41", "42", "43", "44", "45",
                                                   "46", "47", "48", "49", "50",
                                                   "51", "52", "53", "54", "55",
                                                   "56", "57", "58", "59", "60",
                                                   "61"};

class TestAlarmIndicatorPrivate
{
public:
    TestAlarmIndicatorPrivate()
        : limitAlarm(NULL),
          oneShotAlarm(NULL),
          isClearAllAlarm(true)
    {}

    ~TestAlarmIndicatorPrivate()
    {}
    LimitAlarm *limitAlarm;
    OneShotAlarm *oneShotAlarm;
    bool isClearAllAlarm;
};


TestAlarmIndicator::TestAlarmIndicator()
    : d_ptr(new TestAlarmIndicatorPrivate)
{
}

void TestAlarmIndicator::initTestCase()
{
    d_ptr->limitAlarm = new LimitAlarm;
    d_ptr->oneShotAlarm = new OneShotAlarm;
}

void TestAlarmIndicator::cleanupTestCase()
{
    if (d_ptr->limitAlarm)
    {
        delete d_ptr->limitAlarm;
    }
    if (d_ptr->oneShotAlarm)
    {
        delete d_ptr->oneShotAlarm;
    }
}

void TestAlarmIndicator::cleanup()
{
    // clean all alarm info
    if (d_ptr->isClearAllAlarm)
    {
        for (int i = 0; i < alarmIndicator.getAlarmCount(); i++)
        {
            AlarmInfoNode node;
            alarmIndicator.getAlarmInfo(i, node);
            alarmIndicator.delAlarmInfo(node.alarmType, node.alarmMessage);
            if (alarmIndicator.checkAlarmIsExist(node.alarmType, node.alarmMessage) == false)
            {
                // delete info success
                i--;
            }
        }

        QCOMPARE(alarmIndicator.getAlarmCount(), 0);
    }
}

void TestAlarmIndicator::testHandleAlarmInfo_data()
{
    QTest::addColumn<AlarmParamIFace *>("alarmInterface");
    QTest::addColumn<AlarmType>("alarmType");
    QTest::addColumn<bool>("result");
    QTest::addColumn<int>("countResult");
    QTest::addColumn<bool>("delResult");
    QTest::addColumn<int>("testCase");

    QTest::newRow("limitPhyAlarm/normalcase") << static_cast<AlarmParamIFace *>(d_ptr->limitAlarm)
                                              << ALARM_TYPE_PHY
                                              << true
                                              << 1
                                              << false
                                              << 0;
    QTest::newRow("oneShotPhyAlarm/normalcase") << static_cast<AlarmParamIFace *>(d_ptr->oneShotAlarm)
                                                << ALARM_TYPE_PHY
                                                << true
                                                << 1
                                                << false
                                                << 0;
    QTest::newRow("oneShotTechAlarm/normalcase") << static_cast<AlarmParamIFace *>(d_ptr->oneShotAlarm)
                                                 << ALARM_TYPE_TECH
                                                 << true
                                                 << 1
                                                 << false
                                                 << 0;

    QTest::newRow("limitPhyAlarm/overQueue") << static_cast<AlarmParamIFace *>(d_ptr->limitAlarm)
                                             << ALARM_TYPE_PHY
                                             << false
                                             << ALARM_INFO_POOL_LEN
                                             << false
                                             << 1;
    QTest::newRow("oneShotPhyAlarm/overQueue") << static_cast<AlarmParamIFace *>(d_ptr->oneShotAlarm)
                                               << ALARM_TYPE_PHY
                                               << false
                                               << ALARM_INFO_POOL_LEN
                                               << false
                                               << 1;
    QTest::newRow("oneShotTechAlarm/overQueue") << static_cast<AlarmParamIFace *>(d_ptr->oneShotAlarm)
                                                << ALARM_TYPE_TECH
                                                << false
                                                << ALARM_INFO_POOL_LEN
                                                << false
                                                << 1;

    QTest::newRow("limitPhyAlarm/nullMessage") << static_cast<AlarmParamIFace *>(d_ptr->limitAlarm)
                                               << ALARM_TYPE_PHY
                                               << false
                                               << 0
                                               << false
                                               << 2;
    QTest::newRow("oneShotPhyAlarm/nullMessage") << static_cast<AlarmParamIFace *>(d_ptr->oneShotAlarm)
                                                 << ALARM_TYPE_PHY
                                                 << false
                                                 << 0
                                                 << false
                                                 << 2;
    QTest::newRow("oneShotTechAlarm/nullMessage") << static_cast<AlarmParamIFace *>(d_ptr->oneShotAlarm)
                                                  << ALARM_TYPE_TECH
                                                  << false
                                                  << 0
                                                  << false
                                                  << 2;
}

void TestAlarmIndicator::testHandleAlarmInfo()
{
    QFETCH(AlarmParamIFace*, alarmInterface);
    QFETCH(AlarmType, alarmType);
    QFETCH(bool, result);
    QFETCH(int, countResult);
    QFETCH(bool, delResult);
    QFETCH(int, testCase);

    QTime t = QTime::currentTime();
    bool ret = false;
    if (testCase == 0)   // 测试正常情况
    {
        // 正常情况
        ret = alarmIndicator.addAlarmInfo(t.elapsed(), alarmType,
                                    alarmInterface->getAlarmPriority(0),
                                    alarmInterface->toString(0),
                                    alarmInterface, 1);
        // test add alarm info
        QCOMPARE(ret, result);

        // test get alarm info
        AlarmInfoNode tmpNode;
        QCOMPARE(alarmIndicator.getAlarmInfo(alarmType, alarmInterface->toString(0), tmpNode),
                 result);

        // test get alarm info count
        QCOMPARE(alarmIndicator.getAlarmCount(alarmType), countResult);

        // test check alarm exist
        QCOMPARE(alarmIndicator.checkAlarmIsExist(alarmType, alarmInterface->toString(0)), result);

        // test delete alarm info
        alarmIndicator.delAlarmInfo(alarmType, alarmInterface->toString(0));
        QCOMPARE(alarmIndicator.checkAlarmIsExist(alarmType, alarmInterface->toString(0)), delResult);
    }
    else if (testCase == 1)  // 测试添加超出队列数量情况
    {
        int i = 0;
        for (i = 0; i<= ALARM_INFO_POOL_LEN; i++)
        {
            ret = alarmIndicator.addAlarmInfo(t.elapsed(), alarmType,
                                        alarmInterface->getAlarmPriority(0),
                                        useCase[i],
                                        alarmInterface, 1);
        }

        // test add alarm info
        QCOMPARE(ret, result);

        // test get alarm info
        AlarmInfoNode tmpNode;
        QCOMPARE(alarmIndicator.getAlarmInfo(alarmType, useCase[i - 1], tmpNode), result);

        // test get alarm info count
        QCOMPARE(alarmIndicator.getAlarmCount(alarmType), countResult);

        // test check alarm exist
        QCOMPARE(alarmIndicator.checkAlarmIsExist(alarmType, useCase[i - 1]), result);

        // test delete alarm info
        alarmIndicator.delAlarmInfo(alarmType, useCase[i - 1]);
        QCOMPARE(alarmIndicator.checkAlarmIsExist(alarmType, useCase[i - 1]), delResult);
    }
    else    // 测试添加的报警字符常量为空
    {
        ret = alarmIndicator.addAlarmInfo(t.elapsed(), alarmType,
                                    alarmInterface->getAlarmPriority(0),
                                    NULL,
                                    alarmInterface, 1);

        QCOMPARE(ret, result);

        AlarmInfoNode tmpNode;
        QCOMPARE(alarmIndicator.getAlarmInfo(alarmType, NULL, tmpNode), result);

        // test get alarm info count
        QCOMPARE(alarmIndicator.getAlarmCount(alarmType), countResult);

        // test check alarm exist
        QCOMPARE(alarmIndicator.checkAlarmIsExist(alarmType, NULL), result);

        // test delete alarm info
        alarmIndicator.delAlarmInfo(alarmType, NULL);
        QCOMPARE(alarmIndicator.checkAlarmIsExist(alarmType, NULL), delResult);
    }

    // test delete all phy alarm
    if (alarmType == ALARM_TYPE_PHY)
    {
        alarmIndicator.delAllPhyAlarm();
        QCOMPARE(alarmIndicator.getAlarmCount(ALARM_TYPE_PHY), 0);
    }
}

void TestAlarmIndicator::testUpdateAlarmInfo_data()
{
    QTest::addColumn<AlarmInfoNode>("prepareNode");
    QTest::addColumn<AlarmInfoNode>("node");
    QTest::addColumn<bool>("result");


    AlarmInfoNode phyLimitNode;
    phyLimitNode.alarmID = 0;
    phyLimitNode.alarmPriority = ALARM_PRIO_PROMPT;
    phyLimitNode.alarmSource = d_ptr->limitAlarm;
    phyLimitNode.alarmType = ALARM_TYPE_PHY;
    phyLimitNode.alarmMessage = d_ptr->limitAlarm->toString(0);

    AlarmInfoNode phyOneshotNode;
    phyOneshotNode.alarmID = 1;
    phyOneshotNode.alarmPriority = ALARM_PRIO_LOW;
    phyOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    phyOneshotNode.alarmType = ALARM_TYPE_PHY;
    phyOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode techOneshotNode;
    techOneshotNode.alarmID = 3;
    techOneshotNode.alarmPriority = ALARM_PRIO_HIGH;
    techOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    techOneshotNode.alarmType = ALARM_TYPE_TECH;
    techOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    QTest::newRow("has exist phy limit node") << phyLimitNode << phyLimitNode << true;
    QTest::newRow("has exist phy oneshot node") << phyOneshotNode << phyOneshotNode << true;
    QTest::newRow("has exist tech oneshot node") << techOneshotNode << techOneshotNode << true;

    AlarmInfoNode updatePNode;
    updatePNode.alarmID = 4;
    updatePNode.alarmPriority = ALARM_PRIO_HIGH;
    updatePNode.alarmSource = d_ptr->limitAlarm;
    updatePNode.alarmType = ALARM_TYPE_PHY;
    updatePNode.alarmMessage = d_ptr->limitAlarm->toString(0);

    AlarmInfoNode updateNode;
    updateNode.alarmID = 5;
    updateNode.alarmPriority = ALARM_PRIO_LOW;
    updateNode.alarmSource = d_ptr->limitAlarm;
    updateNode.alarmType = ALARM_TYPE_PHY;
    updateNode.alarmMessage = d_ptr->limitAlarm->toString(0);

    QTest::newRow("update node") << updatePNode << updateNode << false;

    AlarmInfoNode originalNode;
    QTest::newRow("original node") << updatePNode << originalNode << true;

    AlarmInfoNode noExistNode;
    noExistNode.alarmID = 6;
    noExistNode.alarmPriority = ALARM_PRIO_MED;
    noExistNode.alarmSource = d_ptr->limitAlarm;
    noExistNode.alarmType = ALARM_TYPE_LIFE;
    noExistNode.alarmMessage = "Test";

    QTest::newRow("no exist node") << updatePNode << noExistNode << true;
}

void TestAlarmIndicator::testUpdateAlarmInfo()
{
    QFETCH(AlarmInfoNode, prepareNode);
    QFETCH(AlarmInfoNode, node);
    QFETCH(bool, result);

    // test already has exist node case
    alarmIndicator.addAlarmInfo(QTime::currentTime().elapsed(),
                                prepareNode.alarmType,
                                prepareNode.alarmPriority,
                                prepareNode.alarmMessage,
                                prepareNode.alarmSource,
                                prepareNode.alarmID);
    alarmIndicator.updateAlarmInfo(node);
    AlarmInfoNode newInfo;
    alarmIndicator.getAlarmInfo(0, newInfo);

    QCOMPARE(prepareNode == newInfo, result);
}

void TestAlarmIndicator::testLatchAlarmInfo_data()
{
    QTest::addColumn<AlarmInfoNode>("prepareNode");
    QTest::addColumn<AlarmInfoNode>("latchNode");
    QTest::addColumn<bool>("result");
    QTest::addColumn<bool>("delResult");

    AlarmInfoNode phyLimitNode;
    phyLimitNode.alarmID = 0;
    phyLimitNode.alarmPriority = ALARM_PRIO_PROMPT;
    phyLimitNode.alarmSource = d_ptr->limitAlarm;
    phyLimitNode.alarmType = ALARM_TYPE_PHY;
    phyLimitNode.alarmMessage = d_ptr->limitAlarm->toString(0);

    AlarmInfoNode phyLimitLatchNode = phyLimitNode;
    phyLimitLatchNode.latch = true;

    AlarmInfoNode phyOneshotNode;
    phyOneshotNode.alarmID = 1;
    phyOneshotNode.alarmPriority = ALARM_PRIO_LOW;
    phyOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    phyOneshotNode.alarmType = ALARM_TYPE_PHY;
    phyOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode phyOneshotLatchNode = phyOneshotNode;
    phyOneshotLatchNode.latch = true;

    AlarmInfoNode techOneshotNode;
    techOneshotNode.alarmID = 3;
    techOneshotNode.alarmPriority = ALARM_PRIO_HIGH;
    techOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    techOneshotNode.alarmType = ALARM_TYPE_TECH;
    techOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode techOneshotLatchNode = techOneshotNode;
    techOneshotLatchNode.latch = true;

    QTest::newRow("latch phy limit which hasn't latch") << phyLimitNode << phyLimitLatchNode<< true << false;
    QTest::newRow("latch phy limit which has latch") << phyLimitNode << phyLimitNode<< true << false;

    QTest::newRow("latch phy oneshot which hasn't latch") << phyOneshotNode << phyOneshotLatchNode<< true << false;
    QTest::newRow("latch phy oneshot which has latch") << phyOneshotLatchNode << phyOneshotNode<< true << false;

    QTest::newRow("latch tech oneshot which hasn't latch") << techOneshotNode << techOneshotLatchNode<< true << false;
    QTest::newRow("latch tech oneshot which has latch") << techOneshotLatchNode << techOneshotNode<< true << false;


    AlarmInfoNode phyLimitNonExistNode = phyLimitLatchNode;
    phyLimitNonExistNode.alarmMessage = "Test";

    AlarmInfoNode phyOneshotNonExistNode = phyOneshotLatchNode;
    phyOneshotNonExistNode.alarmMessage = "Test";

    AlarmInfoNode techOneshotNonExistNode = techOneshotLatchNode;
    techOneshotNonExistNode.alarmMessage = "Test";

    QTest::newRow("latch phy limit which hasn't latch") << phyLimitNode << phyLimitNonExistNode<< false << false;
    QTest::newRow("latch phy limit which has latch") << phyOneshotNode << phyOneshotNonExistNode<< false << false;
    QTest::newRow("latch phy oneshot which hasn't latch") << techOneshotNode << techOneshotNonExistNode
                                                          << false << false;
}

void TestAlarmIndicator::testLatchAlarmInfo()
{
    QFETCH(AlarmInfoNode, prepareNode);
    QFETCH(AlarmInfoNode, latchNode);
    QFETCH(bool, result);
    QFETCH(bool, delResult);
    alarmIndicator.addAlarmInfo(QTime::currentTime().elapsed(),
                                prepareNode.alarmType,
                                prepareNode.alarmPriority,
                                prepareNode.alarmMessage,
                                prepareNode.alarmSource,
                                prepareNode.alarmID);

    // test latch alarm info
    bool ret = alarmIndicator.latchAlarmInfo(latchNode.alarmType, latchNode.alarmMessage);
    QCOMPARE(ret, result);

    // test has phy latch alarm info and delete phy latch alarm;
    if (latchNode.alarmType == ALARM_TYPE_PHY)
    {
        QCOMPARE(alarmIndicator.hasLatchPhyAlarm(), result);
        alarmIndicator.delLatchPhyAlarm();
        QCOMPARE(alarmIndicator.hasLatchPhyAlarm(), delResult);
    }
}

void TestAlarmIndicator::testUpdateLatchAlarmInfo_data()
{
    QTest::addColumn<AlarmInfoNode>("prepareNode");
    QTest::addColumn<AlarmInfoNode>("latchNode");
    QTest::addColumn<bool>("result");

    AlarmInfoNode phyLimitNode;
    phyLimitNode.alarmID = 0;
    phyLimitNode.alarmPriority = ALARM_PRIO_PROMPT;
    phyLimitNode.alarmSource = d_ptr->limitAlarm;
    phyLimitNode.alarmType = ALARM_TYPE_PHY;
    phyLimitNode.alarmMessage = d_ptr->limitAlarm->toString(0);

    AlarmInfoNode phyLimitLatchNode = phyLimitNode;
    phyLimitLatchNode.latch = true;

    AlarmInfoNode phyOneshotNode;
    phyOneshotNode.alarmID = 1;
    phyOneshotNode.alarmPriority = ALARM_PRIO_LOW;
    phyOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    phyOneshotNode.alarmType = ALARM_TYPE_PHY;
    phyOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode phyOneshotLatchNode = phyOneshotNode;
    phyOneshotLatchNode.latch = true;

    AlarmInfoNode techOneshotNode;
    techOneshotNode.alarmID = 3;
    techOneshotNode.alarmPriority = ALARM_PRIO_HIGH;
    techOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    techOneshotNode.alarmType = ALARM_TYPE_TECH;
    techOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode techOneshotLatchNode = techOneshotNode;
    techOneshotLatchNode.latch = true;

    QTest::newRow("latch phy limit alarm which hasn't latched") << phyLimitNode << phyLimitLatchNode << true;
    QTest::newRow("latch phy limit alarm which has latched") << phyLimitLatchNode << phyLimitLatchNode << true;
    QTest::newRow("unlatch phy limit alarm which hasn't latched") << phyLimitNode << phyLimitNode << true;
    QTest::newRow("unlatch phy limit alarm which has latched") << phyLimitLatchNode << phyLimitNode << true;

    QTest::newRow("latch phy oneshot alarm which hasn't latched") << phyOneshotNode << phyOneshotLatchNode << true;
    QTest::newRow("latch phy oneshot alarm which has latched") << phyOneshotLatchNode << phyOneshotLatchNode << true;
    QTest::newRow("unlatch phy oneshot alarm which hasn't latched") << phyOneshotNode << phyOneshotNode << true;
    QTest::newRow("unlatch phy oneshot alarm which has latched") << phyOneshotLatchNode << phyOneshotNode << true;

    QTest::newRow("latch tech oneshot alarm which hasn't latched") << techOneshotNode << techOneshotLatchNode << true;
    QTest::newRow("latch tech oneshot alarm which has latched") << techOneshotLatchNode << techOneshotLatchNode << true;
    QTest::newRow("unlatch tech oneshot alarm which hasn't latched") << techOneshotNode << techOneshotNode << true;
    QTest::newRow("unlatch tech oneshot alarm which has latched") << techOneshotLatchNode << techOneshotNode << true;

    AlarmInfoNode phyLimitNonExistNode = phyLimitLatchNode;
    phyLimitNonExistNode.alarmMessage = "Test";

    AlarmInfoNode phyOneshotNonExistNode = phyOneshotLatchNode;
    phyOneshotNonExistNode.alarmMessage = "Test";

    AlarmInfoNode techOneshotNonExistNode = techOneshotLatchNode;
    techOneshotNonExistNode.alarmMessage = "Test";

    QTest::newRow("set latch but no the phy limit alarm") << phyLimitNode << phyLimitNonExistNode << false;
    QTest::newRow("set latch but no the phy oneshot alarm") << phyOneshotNode << phyOneshotNonExistNode << false;
    QTest::newRow("set latch but no the phy limit alarm") << techOneshotNode << techOneshotNonExistNode << false;
}

void TestAlarmIndicator::testUpdateLatchAlarmInfo()
{
    QFETCH(AlarmInfoNode, prepareNode);
    QFETCH(AlarmInfoNode, latchNode);
    QFETCH(bool, result);
    alarmIndicator.addAlarmInfo(QTime::currentTime().elapsed(),
                                prepareNode.alarmType,
                                prepareNode.alarmPriority,
                                prepareNode.alarmMessage,
                                prepareNode.alarmSource,
                                prepareNode.alarmID);

    bool ret = alarmIndicator.updateLatchAlarmInfo(latchNode.alarmMessage, latchNode.latch);
    QCOMPARE(ret, result);
}

void TestAlarmIndicator::testPhyAlarmPauseStatusHandle_data()
{
    QTest::addColumn<AlarmInfoNode>("pauseNode");
    QTest::addColumn<bool>("result");
    QTest::addColumn<bool>("pauseResult");
    QTest::addColumn<bool>("pauseRepeatlyResult");
    QTest::addColumn<bool>("clearResult");

    AlarmInfoNode phyLimitNode;
    phyLimitNode.alarmID = 0;
    phyLimitNode.alarmPriority = ALARM_PRIO_PROMPT;
    phyLimitNode.alarmSource = d_ptr->limitAlarm;
    phyLimitNode.alarmType = ALARM_TYPE_PHY;
    phyLimitNode.alarmMessage = d_ptr->limitAlarm->toString(0);

    AlarmInfoNode phyLimitPauseNode = phyLimitNode;
    phyLimitPauseNode.pauseTime = 120;

    AlarmInfoNode phyOneshotNode;
    phyOneshotNode.alarmID = 1;
    phyOneshotNode.alarmPriority = ALARM_PRIO_LOW;
    phyOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    phyOneshotNode.alarmType = ALARM_TYPE_PHY;
    phyOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode phyOneshotPauseNode = phyOneshotNode;
    phyOneshotPauseNode.pauseTime = 120;

    QTest::newRow("pause phy limit alarm which hasn't paused") << phyLimitNode
                                                               << true
                                                               << false
                                                               << true
                                                               << true;

    QTest::newRow("pause phy limit alarm which has paused")    << phyLimitPauseNode
                                                               << false
                                                               << true
                                                               << false
                                                               << true;

    QTest::newRow("pause phy oneshot alarm which hasn't paused") << phyOneshotNode
                                                                 << true
                                                                 << false
                                                                 << true
                                                                 << true;

    QTest::newRow("pause phy oneshot alarm which has paused")  << phyOneshotPauseNode
                                                               << false
                                                               << true
                                                               << false
                                                               << true;
}

void TestAlarmIndicator::testPhyAlarmPauseStatusHandle()
{
    QFETCH(AlarmInfoNode, pauseNode);
    QFETCH(bool, result);
    QFETCH(bool, pauseResult);
    QFETCH(bool, pauseRepeatlyResult);
    QFETCH(bool, clearResult);

    alarmIndicator.addAlarmInfo(QTime::currentTime().elapsed(),
                                pauseNode.alarmType,
                                pauseNode.alarmPriority,
                                pauseNode.alarmMessage,
                                pauseNode.alarmSource,
                                pauseNode.alarmID);

    alarmIndicator.updateAlarmInfo(pauseNode);

    // test has non pause phy alarm
    QCOMPARE(alarmIndicator.hasNonPausePhyAlarm(), result);

    // test pause phy alarm
    alarmIndicator.phyAlarmPauseStatusHandle();
    QCOMPARE(alarmIndicator.hasNonPausePhyAlarm(), pauseResult);

    // test pause phy alarm repeatly
    alarmIndicator.phyAlarmPauseStatusHandle();
    QCOMPARE(alarmIndicator.hasNonPausePhyAlarm(), pauseRepeatlyResult);

    // test clear alarm pause
    alarmIndicator.clearAlarmPause();
    QCOMPARE(alarmIndicator.hasNonPausePhyAlarm(), clearResult);
}

void TestAlarmIndicator::testUpdataAlarmPriority_data()
{
    QTest::addColumn<AlarmInfoNode>("prepareNode");
    QTest::addColumn<AlarmInfoNode>("priorityNode");
    QTest::addColumn<bool>("updateReturn");

    AlarmInfoNode phyLimitNode;
    phyLimitNode.alarmID = 0;
    phyLimitNode.alarmPriority = ALARM_PRIO_PROMPT;
    phyLimitNode.alarmSource = d_ptr->limitAlarm;
    phyLimitNode.alarmType = ALARM_TYPE_PHY;
    phyLimitNode.alarmMessage = d_ptr->limitAlarm->toString(0);

    AlarmInfoNode phyLimitPriorityNode = phyLimitNode;
    phyLimitPriorityNode.alarmPriority = ALARM_PRIO_LOW;

    AlarmInfoNode phyOneshotNode;
    phyOneshotNode.alarmID = 1;
    phyOneshotNode.alarmPriority = ALARM_PRIO_LOW;
    phyOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    phyOneshotNode.alarmType = ALARM_TYPE_PHY;
    phyOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode phyOneshotPriorityNode = phyOneshotNode;
    phyOneshotPriorityNode.alarmPriority = ALARM_PRIO_MED;

    AlarmInfoNode techOneshotNode;
    techOneshotNode.alarmID = 3;
    techOneshotNode.alarmPriority = ALARM_PRIO_MED;
    techOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    techOneshotNode.alarmType = ALARM_TYPE_TECH;
    techOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode techOneshotPriorityNode = techOneshotNode;
    techOneshotPriorityNode.alarmPriority = ALARM_PRIO_HIGH;

    QTest::newRow("set phy limit alarm") << phyLimitNode
                                         << phyLimitPriorityNode
                                         << true;
    QTest::newRow("set phy limit alarm same priority") << phyLimitPriorityNode
                                                       << phyLimitPriorityNode
                                                       << false;

    QTest::newRow("set phy oneshot alarm") << phyOneshotNode
                                           << phyOneshotPriorityNode
                                           << true;
    QTest::newRow("set phy limit oneshot same priority") << phyOneshotPriorityNode
                                                         << phyOneshotPriorityNode
                                                         << false;

    QTest::newRow("set tech limit alarm") << techOneshotNode
                                          << techOneshotPriorityNode
                                          << true;
    QTest::newRow("set tech limit alarm same priority") << techOneshotPriorityNode
                                                        << techOneshotPriorityNode
                                                        << false;

    AlarmInfoNode phyLimitNonExistNode = phyLimitPriorityNode;
    phyLimitNonExistNode.alarmMessage = "Test";

    AlarmInfoNode phyOneshotNonExistNode = phyOneshotPriorityNode;
    phyOneshotNonExistNode.alarmMessage = "Test";

    AlarmInfoNode techOneshotNonExistNode = techOneshotPriorityNode;
    techOneshotNonExistNode.alarmMessage = "Test";

    QTest::newRow("set no exist phy limit alarm priority") << phyLimitNode
                                                           << phyLimitNonExistNode
                                                           << false;
    QTest::newRow("set no exist phy oneshot alarm priority")   << phyOneshotNode
                                                               << phyOneshotNonExistNode
                                                               << false;
    QTest::newRow("set no exist tech oneshot alarm priority")  << techOneshotNode
                                                               << techOneshotNonExistNode
                                                               << false;
}

void TestAlarmIndicator::testUpdataAlarmPriority()
{
    QFETCH(AlarmInfoNode, prepareNode);
    QFETCH(AlarmInfoNode, priorityNode);
    QFETCH(bool, updateReturn);

    alarmIndicator.addAlarmInfo(QTime::currentTime().elapsed(),
                                prepareNode.alarmType,
                                prepareNode.alarmPriority,
                                prepareNode.alarmMessage,
                                prepareNode.alarmSource,
                                prepareNode.alarmID);

    bool ret = alarmIndicator.updataAlarmPriority(priorityNode.alarmType,
                                                  priorityNode.alarmMessage,
                                                  priorityNode.alarmPriority);
    QCOMPARE(ret, updateReturn);
}

void TestAlarmIndicator::testResetStatusHandle_data()
{
    QTest::addColumn<AlarmInfoNode>("prepareNode");
    QTest::addColumn<bool>("result");

    AlarmInfoNode phyLimitNode;
    phyLimitNode.alarmPriority = ALARM_PRIO_MED;
    phyLimitNode.alarmSource = d_ptr->limitAlarm;
    phyLimitNode.alarmType = ALARM_TYPE_PHY;
    phyLimitNode.alarmMessage = d_ptr->limitAlarm->toString(0);

    AlarmInfoNode phyLimitResetNode = phyLimitNode;
    phyLimitResetNode.acknowledge = true;

    AlarmInfoNode phyPromptLimitResetNode = phyLimitNode;
    phyPromptLimitResetNode.alarmPriority = ALARM_PRIO_PROMPT;

    AlarmInfoNode phyOneshotNode;
    phyOneshotNode.alarmPriority = ALARM_PRIO_HIGH;
    phyOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    phyOneshotNode.alarmType = ALARM_TYPE_PHY;
    phyOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode phyOneshotResetNode = phyOneshotNode;
    phyOneshotResetNode.acknowledge = true;

    AlarmInfoNode phyLowOneshotResetNode = phyOneshotNode;
    phyLowOneshotResetNode.alarmPriority = ALARM_PRIO_LOW;

    AlarmInfoNode techOneshotNode;
    techOneshotNode.alarmID = 3;
    techOneshotNode.alarmPriority = ALARM_PRIO_HIGH;
    techOneshotNode.alarmSource = d_ptr->oneShotAlarm;
    techOneshotNode.alarmType = ALARM_TYPE_TECH;
    techOneshotNode.alarmMessage = d_ptr->oneShotAlarm->toString(0);

    AlarmInfoNode techOneshotResetNode = techOneshotNode;
    techOneshotResetNode.acknowledge = true;

    AlarmInfoNode techLowOneshotResetNode = techOneshotNode;
    techLowOneshotResetNode.alarmPriority = ALARM_PRIO_LOW;

    AlarmInfoNode techPromptOneshotResetNode = techOneshotNode;
    techPromptOneshotResetNode.alarmPriority = ALARM_PRIO_PROMPT;

    QTest::newRow("reset hasn't acknowledge phy med limit alarm") << phyLimitNode
                                                                  << true;
    QTest::newRow("reset has acknowledge phy limit alarm") << phyLimitResetNode
                                                           << true;
    QTest::newRow("reset hasn't acknowledge phy  prompt limit alarm") << phyPromptLimitResetNode
                                                                      << false;

    QTest::newRow("reset hasn't acknowledge phy high oneshot alarm") << phyOneshotNode
                                                                     << true;
    QTest::newRow("reset hasn't acknowledge phy low oneshot alarm") << phyLowOneshotResetNode
                                                                    << true;
    QTest::newRow("reset has acknowledge phy oneshot alarm") << phyOneshotResetNode
                                                             << true;

    QTest::newRow("reset hasn't acknowledge tech high limit alarm") << techOneshotNode
                                                                    << true;
    QTest::newRow("reset has acknowledge tech limit alarm") << techOneshotResetNode
                                                            << true;
    QTest::newRow("reset hasn't acknowledge tech low limit alarm") << techLowOneshotResetNode
                                                                   << true;
    QTest::newRow("reset hasn't acknowledge tech prompt limit alarm") << techPromptOneshotResetNode
                                                                      << false;
}

void TestAlarmIndicator::testResetStatusHandle()
{
    QFETCH(AlarmInfoNode, prepareNode);
    QFETCH(bool, result);

    alarmIndicator.addAlarmInfo(QTime::currentTime().elapsed(),
                                prepareNode.alarmType,
                                prepareNode.alarmPriority,
                                prepareNode.alarmMessage,
                                prepareNode.alarmSource,
                                prepareNode.alarmID);

    bool ret = false;
    if (prepareNode.alarmType == ALARM_TYPE_PHY)
    {
        ret = alarmIndicator.phyAlarmResetStatusHandle();
    }
    else if (prepareNode.alarmType == ALARM_TYPE_TECH)
    {
        ret = alarmIndicator.techAlarmResetStatusHandle();
    }
    QCOMPARE(ret, result);
}


void TestAlarmIndicator::testPublishAlarmHasNoAlarm_data()
{
    QTest::addColumn<AlarmStatus>("status");

    QTest::newRow("normal status") << ALARM_STATUS_NORMAL;
    QTest::newRow("pause status") << ALARM_STATUS_PAUSE;
    QTest::newRow("reset status") << ALARM_STATUS_RESET;
}

void TestAlarmIndicator::testPublishAlarmHasNoAlarm()
{
    // 当前没有报警时，PublishAlarm处理
    QFETCH(AlarmStatus, status);

    NiceMock<MockAlarmInfoBarWidget> mockPhyInfoBarWidget;
    AlarmInfoBarWidget::registerAlarmInfoBar(ALARM_TYPE_PHY, &mockPhyInfoBarWidget);
    NiceMock<MockAlarmInfoBarWidget> mockTechInfoBarWidget;
    AlarmInfoBarWidget::registerAlarmInfoBar(ALARM_TYPE_TECH, &mockTechInfoBarWidget);
    NiceMock<MockSoundManager> mockSoundManager;
    SoundManagerInterface::registerSoundManager(&mockSoundManager);
    NiceMock<MockLightManager> mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);

    alarmIndicator.setAlarmPhyWidgets(&mockPhyInfoBarWidget, NULL, NULL);
    alarmIndicator.setAlarmTechWidgets(&mockTechInfoBarWidget);
    EXPECT_CALL(mockPhyInfoBarWidget, clear());
    EXPECT_CALL(mockTechInfoBarWidget, clear());
    EXPECT_CALL(mockSoundManager, updateAlarm(false, ALARM_PRIO_LOW));
    EXPECT_CALL(mockLightManager, updateAlarm(false, ALARM_PRIO_LOW));
    alarmIndicator.publishAlarm(status);

    QVERIFY(Mock::VerifyAndClearExpectations(&mockPhyInfoBarWidget));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockTechInfoBarWidget));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockSoundManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockLightManager));
}

void TestAlarmIndicator::testPublishAlarmHasPhyAlarm_data()
{
    QTest::addColumn<AlarmStatus>("status");
    QTest::addColumn<AlarmPriority>("priority");
    QTest::addColumn<AlarmPriority>("highestPriority");

    QTest::newRow("normal status/prompt") << ALARM_STATUS_NORMAL << ALARM_PRIO_PROMPT << ALARM_PRIO_PROMPT;
    QTest::newRow("normal status/high") << ALARM_STATUS_NORMAL << ALARM_PRIO_HIGH << ALARM_PRIO_HIGH;
    QTest::newRow("pause status/high") << ALARM_STATUS_PAUSE << ALARM_PRIO_HIGH << ALARM_PRIO_HIGH;
    QTest::newRow("pause status/prompt") << ALARM_STATUS_PAUSE << ALARM_PRIO_PROMPT << ALARM_PRIO_HIGH;
    /* after alarm reset, not light alarm */
    QTest::newRow("reset status/low") << ALARM_STATUS_RESET << ALARM_PRIO_LOW << ALARM_PRIO_HIGH;
    d_ptr->isClearAllAlarm = false;
}

void TestAlarmIndicator::testPublishAlarmHasPhyAlarm()
{
    // 当前有生理报警时，PublishAlarm处理
    QFETCH(AlarmStatus, status);
    QFETCH(AlarmPriority, priority);
    QFETCH(AlarmPriority, highestPriority);

    AlarmPriority soundPriority;
    if (status != ALARM_STATUS_NORMAL)
    {
        // 非报警正常时，都关闭声音
        soundPriority = ALARM_PRIO_PROMPT;
    }
    else
    {
        soundPriority = highestPriority;
    }
    AlarmPriority lightPriority = highestPriority;

    systemConfig.setNumValue("Alarms|AlarmAudio", 1);
    static int i = 0;
    alarmIndicator.addAlarmInfo(QTime::currentTime().elapsed(),
                                ALARM_TYPE_PHY,
                                priority, useCase[i++], d_ptr->limitAlarm, 0);
    NiceMock<MockAlarm> mockAlarm;
    AlarmInterface::registerAlarm(&mockAlarm);
    MockAlarmInfoBarWidget mockPhyInfoBarWidget;
    AlarmInfoBarWidget::registerAlarmInfoBar(ALARM_TYPE_PHY, &mockPhyInfoBarWidget);
    MockAlarmInfoBarWidget mockTechInfoBarWidget;
    AlarmInfoBarWidget::registerAlarmInfoBar(ALARM_TYPE_TECH, &mockTechInfoBarWidget);
    MockSoundManager mockSoundManager;
    SoundManagerInterface::registerSoundManager(&mockSoundManager);
    MockLightManager mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);
    MockAlarmStateMachine mockAlarmStateMachine;
    AlarmStateMachineInterface::registerAlarmStateMachine(&mockAlarmStateMachine);

    alarmIndicator.setAlarmPhyWidgets(&mockPhyInfoBarWidget, NULL, NULL);
    alarmIndicator.setAlarmTechWidgets(&mockTechInfoBarWidget);


    ON_CALL(mockAlarm, getAlarmLightOnAlarmReset).WillByDefault(Return(true));

    if (status == ALARM_STATUS_PAUSE)
    {
        // 报警暂停处理
        alarmIndicator.phyAlarmPauseStatusHandle();
    }
    else if (status == ALARM_STATUS_RESET)
    {
        // 报警复位处理时，会清除每个报警的时间，确认每个报警，去除每个报警的栓锁
        alarmIndicator.clearAlarmPause();
        alarmIndicator.phyAlarmResetStatusHandle();
        alarmIndicator.delLatchPhyAlarm();
    }

    bool soundHasAlarm = false;
    if (highestPriority > ALARM_PRIO_PROMPT && status == ALARM_STATUS_NORMAL)
    {
        // 当前报警最高等级大于提示等级，且报警状态为正常时，播放报警音
        soundHasAlarm = true;
    }

    bool lightHasAlarm = false;
    if (highestPriority != ALARM_PRIO_PROMPT)
    {
        /* after alarm reset and alarm pause, no light alarm */
        lightHasAlarm = true;
    }

    EXPECT_CALL(mockPhyInfoBarWidget, display(_));      // 无论什么报警状态，都显示报警

    if (status != ALARM_STATUS_PAUSE)
    {
        // 非暂停时，处理无生理暂停事件
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_NO_PAUSED_PHY_ALARM, _, _));
    }

    if (status != ALARM_STATUS_RESET)
    {
        // 非报警复位时，报警状态机处理无报警确认事件
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_NO_ACKNOWLEDG_ALARM, _, _));
    }

    EXPECT_CALL(mockTechInfoBarWidget, clear());
    EXPECT_CALL(mockSoundManager, updateAlarm(soundHasAlarm, soundPriority));
    EXPECT_CALL(mockLightManager, updateAlarm(lightHasAlarm, lightPriority));

    alarmIndicator.publishAlarm(status);

    QVERIFY(Mock::VerifyAndClearExpectations(&mockPhyInfoBarWidget));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockTechInfoBarWidget));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockSoundManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockLightManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}

void TestAlarmIndicator::testPublishAlarmHasTechAlarm_data()
{
    QTest::addColumn<AlarmStatus>("status");
    QTest::addColumn<AlarmPriority>("priority");
    QTest::addColumn<AlarmPriority>("highestPriority");

    QTest::newRow("normal status/prompt") << ALARM_STATUS_NORMAL << ALARM_PRIO_PROMPT << ALARM_PRIO_PROMPT;
    QTest::newRow("normal status/high") << ALARM_STATUS_NORMAL << ALARM_PRIO_HIGH << ALARM_PRIO_HIGH;
    QTest::newRow("pause status/high") << ALARM_STATUS_PAUSE << ALARM_PRIO_HIGH << ALARM_PRIO_HIGH;
    QTest::newRow("pause status/prompt") << ALARM_STATUS_PAUSE << ALARM_PRIO_PROMPT << ALARM_PRIO_HIGH;
    QTest::newRow("reset status/low") << ALARM_STATUS_RESET << ALARM_PRIO_LOW << ALARM_PRIO_HIGH;
}

void TestAlarmIndicator::testPublishAlarmHasTechAlarm()
{
    // 当前有技术报警时，PublishAlarm处理
    QFETCH(AlarmStatus, status);
    QFETCH(AlarmPriority, priority);
    QFETCH(AlarmPriority, highestPriority);
    alarmIndicator.delAllPhyAlarm();
    d_ptr->isClearAllAlarm = false;
    static int j = 0;
    // 增加技术报警
    alarmIndicator.addAlarmInfo(QTime::currentTime().elapsed(),
                                ALARM_TYPE_TECH,
                                priority, useCase[j++], d_ptr->oneShotAlarm, 0, false);

    NiceMock<MockAlarm> mockAlarm;
    MockAlarmInfoBarWidget mockPhyInfoBarWidget;
    AlarmInfoBarWidget::registerAlarmInfoBar(ALARM_TYPE_PHY, &mockPhyInfoBarWidget);
    MockAlarmInfoBarWidget mockTechInfoBarWidget;
    AlarmInfoBarWidget::registerAlarmInfoBar(ALARM_TYPE_TECH, &mockTechInfoBarWidget);
    MockSoundManager mockSoundManager;
    SoundManagerInterface::registerSoundManager(&mockSoundManager);
    MockLightManager mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);
    MockAlarmStateMachine mockAlarmStateMachine;
    AlarmStateMachineInterface::registerAlarmStateMachine(&mockAlarmStateMachine);

    // 加载生理和技术报警显示栏
    alarmIndicator.setAlarmPhyWidgets(&mockPhyInfoBarWidget, NULL, NULL);
    alarmIndicator.setAlarmTechWidgets(&mockTechInfoBarWidget);
    ON_CALL(mockAlarm, getAlarmLightOnAlarmReset).WillByDefault(Return(true));

    if (highestPriority > ALARM_PRIO_PROMPT)
    {
        EXPECT_CALL(mockLightManager, updateAlarm(true, highestPriority));  // 任何报警状态下都显示报警灯

        if (status == ALARM_STATUS_NORMAL)
        {
            // 报警最高等级非提示时，且此时报警状态为正常状态，则更新报警声音和报警灯
            EXPECT_CALL(mockSoundManager, updateAlarm(true, highestPriority));
        }
        else
        {
            // 关闭报警音
            EXPECT_CALL(mockSoundManager, updateAlarm(false, ALARM_PRIO_PROMPT));
        }
    }
    else
    {
        // 关闭报警音,灯光
        EXPECT_CALL(mockSoundManager, updateAlarm(false, ALARM_PRIO_PROMPT));
        EXPECT_CALL(mockLightManager, updateAlarm(false, ALARM_PRIO_PROMPT));
    }

    EXPECT_CALL(mockPhyInfoBarWidget, clear());
    EXPECT_CALL(mockTechInfoBarWidget, display(_));

    EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_ALL_PHY_ALARM_LATCHED, _, _));
    EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_NO_PAUSED_PHY_ALARM, _, _));
    if (status != ALARM_STATUS_RESET)
    {
        // 非复位状态时，才处理无报警被确认事件
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_NO_ACKNOWLEDG_ALARM, _, _));
    }

    if (status == ALARM_STATUS_RESET)
    {
        // 复位时，对报警进行确认
        alarmIndicator.techAlarmResetStatusHandle();
        alarmIndicator.phyAlarmResetStatusHandle();
    }

    alarmIndicator.publishAlarm(status);

    QVERIFY(Mock::VerifyAndClearExpectations(&mockPhyInfoBarWidget));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockTechInfoBarWidget));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockSoundManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockLightManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}

