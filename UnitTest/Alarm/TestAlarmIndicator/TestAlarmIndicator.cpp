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
#include "IConfig.h"

Q_DECLARE_METATYPE(AlarmParamIFace *)
Q_DECLARE_METATYPE(AlarmType)
Q_DECLARE_METATYPE(AlarmInfoNode)
Q_DECLARE_METATYPE(AlarmPriority)
Q_DECLARE_METATYPE(AlarmStatus)

using ::testing::Mock;
using ::testing::_;
using ::testing::AnyNumber;

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
          oneShotAlarm(NULL)
    {}

    ~TestAlarmIndicatorPrivate()
    {}
    LimitAlarm *limitAlarm;
    OneShotAlarm *oneShotAlarm;
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

void TestAlarmIndicator::testAddAlarmInfo_data()
{
    QTest::addColumn<AlarmParamIFace *>("alarmInterface");
    QTest::addColumn<AlarmType>("alarmType");

    QTest::newRow("limitPhyAlarm") << static_cast<AlarmParamIFace *>(d_ptr->limitAlarm) << ALARM_TYPE_PHY;
    QTest::newRow("limitTechAlarm") << static_cast<AlarmParamIFace *>(d_ptr->limitAlarm) << ALARM_TYPE_TECH;

    QTest::newRow("oneShotPhyAlarm") << static_cast<AlarmParamIFace *>(d_ptr->oneShotAlarm) << ALARM_TYPE_PHY;
    QTest::newRow("oneShotTechAlarm") << static_cast<AlarmParamIFace *>(d_ptr->oneShotAlarm) << ALARM_TYPE_TECH;
}

void TestAlarmIndicator::testAddAlarmInfo()
{
    QFETCH(AlarmParamIFace*, alarmInterface);
    QFETCH(AlarmType, alarmType);

    QTime t = QTime::currentTime();
    bool ret = alarmIndicator.addAlarmInfo(t.elapsed(), alarmType,
                                alarmInterface->getAlarmPriority(0),
                                alarmInterface->toString(0),
                                alarmInterface, 1);
    QCOMPARE(ret, true);
}

void TestAlarmIndicator::testUpdateAlarmInfo_data()
{
    QTest::addColumn<AlarmInfoNode>("node");
    QTest::addColumn<bool>("result");

    AlarmInfoNode existNode;
    alarmIndicator.getAlarmInfo(0, existNode);
    QTest::newRow("has exist node") << existNode << true;
    AlarmInfoNode updateNode = existNode;
    updateNode.alarmID = 2;
    updateNode.alarmPriority = ALARM_PRIO_PROMPT;
    updateNode.latch = true;
    QTest::newRow("update node") << updateNode << false;

    AlarmInfoNode originalNode;
    QTest::newRow("original node") << originalNode << true;

    AlarmInfoNode noExistNode = existNode;
    noExistNode.alarmType = ALARM_TYPE_LIFE;
    noExistNode.alarmMessage = "Test";
    QTest::newRow("no exist node") << noExistNode << true;
}

void TestAlarmIndicator::testUpdateAlarmInfo()
{
    QFETCH(AlarmInfoNode, node);
    QFETCH(bool, result);

    AlarmInfoNode oldInfo;
    alarmIndicator.getAlarmInfo(0, oldInfo);
    alarmIndicator.updateAlarmInfo(node);
    AlarmInfoNode newInfo;
    alarmIndicator.getAlarmInfo(0, newInfo);

    QCOMPARE(oldInfo == newInfo, result);
}

void TestAlarmIndicator::testDelAlarmInfo_data()
{
    QTest::addColumn<AlarmType>("alarmType");
    QTest::addColumn<QString>("alarmMessage");

    QTest::newRow("limitPhyAlarm") << ALARM_TYPE_PHY << QString(d_ptr->limitAlarm->toString(0));
    QTest::newRow("limitTechAlarm") << ALARM_TYPE_TECH << QString(d_ptr->limitAlarm->toString(0));
    QTest::newRow("oneShotPhyAlarm") << ALARM_TYPE_PHY << QString(d_ptr->oneShotAlarm->toString(0));
    QTest::newRow("oneShotTechAlarm") << ALARM_TYPE_TECH << QString(d_ptr->oneShotAlarm->toString(0));
}

void TestAlarmIndicator::testDelAlarmInfo()
{
    QFETCH(AlarmType, alarmType);
    QFETCH(QString, alarmMessage);

    alarmIndicator.delAlarmInfo(alarmType, alarmMessage.toLocal8Bit());
    QCOMPARE(alarmIndicator.checkAlarmIsExist(alarmType, alarmMessage.toLocal8Bit()), false);
}

void TestAlarmIndicator::testAddAlarmInfoAbnormal()
{
    // 测试加入报警信息满,并加入测试用例
    QTime t = QTime::currentTime();
    bool ret = true;
    AlarmPriority priority = ALARM_PRIO_PROMPT;
    int j = 0;
    for (int i = 0; i < ALARM_INFO_POOL_LEN + 1; i++)
    {
        // 单数时生理报警，双数是技术报警
        AlarmType alarmType;
        if (i % 2 != 0)
        {
            alarmType = ALARM_TYPE_PHY;
        }
        else
        {
            alarmType = ALARM_TYPE_TECH;
        }
        // 生理技术报警各轮循各报警等级
        if (j > 1)
        {
            j = 0;
            int priorityIndex = static_cast<int>(priority) + 1;
            if (priorityIndex > ALARM_PRIO_HIGH)
            {
                priorityIndex = -1;
            }
            priority = static_cast<AlarmPriority>(priorityIndex);
        }
        else
        {
            j++;
        }
        ret = alarmIndicator.addAlarmInfo(t.elapsed(), alarmType,
                                    priority, useCase[i],
                                    d_ptr->limitAlarm, i);
    }
    QCOMPARE(ret, false);
}

void TestAlarmIndicator::testLatchAlarmInfo_data()
{
    QTest::addColumn<AlarmType>("alarmType");
    QTest::addColumn<int>("index");
    QTest::addColumn<bool>("result");

    QTest::newRow("phyNoLatch") << ALARM_TYPE_PHY << ALARM_INFO_POOL_LEN - 1<< true;
    QTest::newRow("phyLatch") << ALARM_TYPE_PHY << ALARM_INFO_POOL_LEN - 1 << true;
    QTest::newRow("techNoLatch") << ALARM_TYPE_TECH << ALARM_INFO_POOL_LEN - 2 << true;
    QTest::newRow("techLatch") << ALARM_TYPE_TECH << ALARM_INFO_POOL_LEN - 2 << true;
    QTest::newRow("noThePhyAlarm") << ALARM_TYPE_PHY << -1 << false;
    QTest::newRow("noTheTechAlarm") << ALARM_TYPE_TECH << -1 << false;
}

void TestAlarmIndicator::testLatchAlarmInfo()
{
    QFETCH(AlarmType, alarmType);
    QFETCH(int, index);
    QFETCH(bool, result);
    bool ret = false;
    if (index > -1)
    {
        ret = alarmIndicator.latchAlarmInfo(alarmType, useCase[index]);
    }
    else
    {
        ret = alarmIndicator.latchAlarmInfo(alarmType, "Test");
    }
    QCOMPARE(ret, result);
}

void TestAlarmIndicator::testUpdateLatchAlarmInfo_data()
{
    QTest::addColumn<int>("index");
    QTest::addColumn<bool>("flag");
    QTest::addColumn<bool>("result");

    QTest::newRow("set phy. alarm latch") << 1 << true << true;
    QTest::newRow("set phy. alarm no latch") << 1 << false << true;
    QTest::newRow("set tech. alarm latch") << 0 << true << true;
    QTest::newRow("set tech. alarm no latch") << 0 << false << true;
    QTest::newRow("set latch but no the alarm") << -1 << true << false;
}

void TestAlarmIndicator::testUpdateLatchAlarmInfo()
{
    QFETCH(int, index);
    QFETCH(bool, flag);
    QFETCH(bool, result);
    bool ret = false;
    if (index >= 0)
    {
        ret = alarmIndicator.updateLatchAlarmInfo(useCase[index], flag);
    }
    else
    {
        ret = alarmIndicator.updateLatchAlarmInfo("Test", flag);
    }

    QCOMPARE(ret, result);
}

void TestAlarmIndicator::testHasLatchPhyAlarm()
{
    alarmIndicator.latchAlarmInfo(ALARM_TYPE_PHY, useCase[ALARM_INFO_POOL_LEN - 1]);
    QCOMPARE(alarmIndicator.hasLatchPhyAlarm(), true);
}

void TestAlarmIndicator::testDelLatchPhyAlarm()
{
    alarmIndicator.latchAlarmInfo(ALARM_TYPE_PHY, useCase[ALARM_INFO_POOL_LEN - 1]);
    alarmIndicator.delLatchPhyAlarm();
    QCOMPARE(alarmIndicator.hasLatchPhyAlarm(), false);
}

void TestAlarmIndicator::testPhyAlarmPauseStatusHandle()
{
    alarmIndicator.phyAlarmPauseStatusHandle();
    AlarmInfoNode node;
    bool ret = alarmIndicator.getAlarmInfo(ALARM_TYPE_PHY, useCase[1], node);
    QCOMPARE(ret, true);
    QCOMPARE(node.pauseTime != 0, true);
    alarmIndicator.phyAlarmPauseStatusHandle();
    ret = alarmIndicator.getAlarmInfo(ALARM_TYPE_PHY, useCase[1], node);
    QCOMPARE(ret, true);
    QCOMPARE(node.pauseTime != 0, false);
}

void TestAlarmIndicator::testHasNonPausePhyAlarm()
{
    alarmIndicator.phyAlarmPauseStatusHandle();
    QCOMPARE(alarmIndicator.hasNonPausePhyAlarm(), false);
}

void TestAlarmIndicator::testClearAlarmPause()
{
    alarmIndicator.clearAlarmPause();
    QCOMPARE(alarmIndicator.hasNonPausePhyAlarm(), true);
}

void TestAlarmIndicator::testUpdataAlarmPriority_data()
{
    QTest::addColumn<AlarmType>("alarmType");
    QTest::addColumn<int>("index");
    QTest::addColumn<AlarmPriority>("priority");
    QTest::addColumn<AlarmPriority>("resultPriority");
    QTest::addColumn<bool>("updateReturn");

    QTest::newRow("set phy alarm") << ALARM_TYPE_PHY
                                   << 1
                                   << ALARM_PRIO_HIGH
                                   << ALARM_PRIO_HIGH
                                   << true;
    QTest::newRow("set phy alarm same priority") << ALARM_TYPE_PHY
                                                 << 1
                                                 << ALARM_PRIO_HIGH
                                                 << ALARM_PRIO_HIGH
                                                 << false;
    QTest::newRow("set tech alarm") << ALARM_TYPE_TECH
                                    << 0
                                    << ALARM_PRIO_MED
                                    << ALARM_PRIO_MED
                                    << true;
    QTest::newRow("set tech alarm same priority") << ALARM_TYPE_TECH
                                                  << 0
                                                  << ALARM_PRIO_MED
                                                  << ALARM_PRIO_MED
                                                  << false;
    QTest::newRow("set no exist alarm priority by alarm type") << ALARM_TYPE_LIFE
                                                               << 2
                                                               << ALARM_PRIO_HIGH
                                                               << ALARM_PRIO_LOW
                                                               << false;
    QTest::newRow("set no exist alarm priority by alarm message") << ALARM_TYPE_PHY
                                                                  << -1
                                                                  << ALARM_PRIO_HIGH
                                                                  << ALARM_PRIO_LOW
                                                                  << false;
}

void TestAlarmIndicator::testUpdataAlarmPriority()
{
    QFETCH(AlarmType, alarmType);
    QFETCH(int, index);
    QFETCH(AlarmPriority, priority);
    QFETCH(AlarmPriority, resultPriority);
    QFETCH(bool, updateReturn);

    if (index >= 0)
    {
        bool ret = alarmIndicator.updataAlarmPriority(alarmType, useCase[index], priority);
        AlarmInfoNode node;
        alarmIndicator.getAlarmInfo(alarmType, useCase[index], node);
        QCOMPARE(node.alarmPriority, resultPriority);
        QCOMPARE(ret, updateReturn);
    }
    else
    {
        bool ret = alarmIndicator.updataAlarmPriority(alarmType, "Test", priority);
        QCOMPARE(ret, updateReturn);
    }
}

void TestAlarmIndicator::testSetAlarmStatus_data()
{
    QTest::addColumn<AlarmStatus>("status");

    QTest::newRow("normal status") << ALARM_STATUS_NORMAL;
    QTest::newRow("pause status") << ALARM_STATUS_PAUSE;
    QTest::newRow("reset status") << ALARM_STATUS_RESET;
}

void TestAlarmIndicator::testPhyAlarmResetStatusHandle()
{
    alarmIndicator.phyAlarmResetStatusHandle();
    for (int i = 0; i < ALARM_INFO_POOL_LEN; i++)
    {
        if (i % 2 != 0)
        {
            AlarmInfoNode phyNode;
            if (alarmIndicator.getAlarmInfo(ALARM_TYPE_PHY, useCase[i], phyNode))
            {
                if (phyNode.alarmPriority > ALARM_PRIO_LOW)
                {
                    // 只确认中级报警和高级报警
                    QCOMPARE(phyNode.acknowledge, true);
                }
                else
                {
                    QCOMPARE(phyNode.acknowledge, false);
                }
            }
        }
    }
}

void TestAlarmIndicator::testTechAlarmResetStatusHandle_data()
{
    QTest::addColumn<AlarmInfoNode>("node");
    QTest::addColumn<bool>("isRemoveAfterLatch");
    QTest::addColumn<bool>("isAcknowledge");

    AlarmInfoNode node0;
    alarmIndicator.getAlarmInfo(ALARM_TYPE_TECH, useCase[0], node0);
    node0.alarmPriority = ALARM_PRIO_HIGH;
    QTest::newRow("high priority/not remove/not acknowledge") << node0 << false << false;
    AlarmInfoNode node2;
    alarmIndicator.getAlarmInfo(ALARM_TYPE_TECH, useCase[2], node2);
    node2.alarmPriority = ALARM_PRIO_PROMPT;
    QTest::newRow("prompt priority/not remove/not acknowledge") << node2 << false << false;

    AlarmInfoNode node4;
    alarmIndicator.getAlarmInfo(ALARM_TYPE_TECH, useCase[4], node4);
    node4.alarmPriority = ALARM_PRIO_HIGH;
    QTest::newRow("high priority/remove/not acknowledge") << node4 << true << false;
    AlarmInfoNode node6;
    alarmIndicator.getAlarmInfo(ALARM_TYPE_TECH, useCase[6], node6);
    node6.alarmPriority = ALARM_PRIO_PROMPT;
    QTest::newRow("prompt priority/remove/not acknowledge") << node6 << true << false;

    AlarmInfoNode node8;
    alarmIndicator.getAlarmInfo(ALARM_TYPE_TECH, useCase[8], node8);
    node8.alarmPriority = ALARM_PRIO_HIGH;
    QTest::newRow("high priority/remove/acknowledge") << node8 << true << true;
    AlarmInfoNode node10;
    alarmIndicator.getAlarmInfo(ALARM_TYPE_TECH, useCase[10], node10);
    node10.alarmPriority = ALARM_PRIO_PROMPT;
    QTest::newRow("prompt priority/remove/acknowledge") << node10 << true << true;

    AlarmInfoNode node12;
    alarmIndicator.getAlarmInfo(ALARM_TYPE_TECH, useCase[12], node12);
    node12.alarmPriority = ALARM_PRIO_HIGH;
    QTest::newRow("high priority/not remove/acknowledge") << node12 << false << true;
    AlarmInfoNode node14;
    alarmIndicator.getAlarmInfo(ALARM_TYPE_TECH, useCase[14], node14);
    node14.alarmPriority = ALARM_PRIO_PROMPT;
    QTest::newRow("prompt priority/not remove/acknowledge") << node14 << false << true;
}

void TestAlarmIndicator::testTechAlarmResetStatusHandle()
{
    QFETCH(AlarmInfoNode, node);
    QFETCH(bool, isRemoveAfterLatch);
    QFETCH(bool, isAcknowledge);
    node.removeAfterLatch = isRemoveAfterLatch;
    node.acknowledge = isAcknowledge;
    alarmIndicator.updateAlarmInfo(node);
    bool handleResult = alarmIndicator.techAlarmResetStatusHandle();
    AlarmInfoNode tmpNode;
    bool ret = alarmIndicator.getAlarmInfo(node.alarmType, node.alarmMessage, tmpNode);
    if (node.alarmPriority > ALARM_PRIO_PROMPT)
    {
        // 高于提示等级时，确认未被确认的技术报警
        if (isRemoveAfterLatch)
        {
            QCOMPARE(ret, false);
        }
        else
        {
            if (!isAcknowledge)
            {
                QCOMPARE(handleResult, true);
            }
            else
            {
                QCOMPARE(handleResult, false);
            }
            QCOMPARE(tmpNode.acknowledge, true);
        }
    }
    else
    {
        QCOMPARE(tmpNode.acknowledge, isAcknowledge);
        QCOMPARE(handleResult, false);
    }
}

void TestAlarmIndicator::testDelAllPhyAlarm()
{
    alarmIndicator.delAllPhyAlarm();
    QCOMPARE(alarmIndicator.getAlarmCount(ALARM_TYPE_PHY), 0);
    // 删除全部报警
    int alarmCount = alarmIndicator.getAlarmCount();
    for (int i = 0; i < alarmCount; i++)
    {
        AlarmInfoNode node;
        alarmIndicator.getAlarmInfo(0, node);
        alarmIndicator.delAlarmInfo(node.alarmType, node.alarmMessage);
    }
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
    QFETCH(AlarmStatus, status);

    MockAlarmInfoBarWidget mockPhyInfoBarWidget;
    AlarmInfoBarWidget::registerAlarmInfoBar(ALARM_TYPE_PHY, &mockPhyInfoBarWidget);
    MockAlarmInfoBarWidget mockTechInfoBarWidget;
    AlarmInfoBarWidget::registerAlarmInfoBar(ALARM_TYPE_TECH, &mockTechInfoBarWidget);
    MockSoundManager mockSoundManager;
    SoundManagerInterface::registerSoundManager(&mockSoundManager);
    MockLightManager mockLightManager;
    LightManagerInterface::registerLightManager(&mockLightManager);

    alarmIndicator.setAlarmPhyWidgets(&mockPhyInfoBarWidget, NULL);
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
    QTest::newRow("pause status/high") << ALARM_STATUS_PAUSE << ALARM_PRIO_HIGH << ALARM_PRIO_LOW;
    QTest::newRow("pause status/prompt") << ALARM_STATUS_PAUSE << ALARM_PRIO_PROMPT << ALARM_PRIO_LOW;
    QTest::newRow("reset status/low") << ALARM_STATUS_RESET << ALARM_PRIO_LOW << ALARM_PRIO_LOW;
}

void TestAlarmIndicator::testPublishAlarmHasPhyAlarm()
{
    QFETCH(AlarmStatus, status);
    QFETCH(AlarmPriority, priority);
    QFETCH(AlarmPriority, highestPriority);
    systemConfig.setNumValue("Alarms|AlarmAudioOff", 0);
    static int i = 0;
    alarmIndicator.addAlarmInfo(QTime::currentTime().elapsed(),
                                ALARM_TYPE_PHY,
                                priority, useCase[i++], d_ptr->limitAlarm, 0);
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

    alarmIndicator.setAlarmPhyWidgets(&mockPhyInfoBarWidget, NULL);
    alarmIndicator.setAlarmTechWidgets(&mockTechInfoBarWidget);

    if (status == ALARM_STATUS_PAUSE)
    {
        // 设置暂停
        alarmIndicator.phyAlarmPauseStatusHandle();
        alarmIndicator.delAllPhyAlarm();
    }
    else if (status == ALARM_STATUS_RESET)
    {
        alarmIndicator.phyAlarmResetStatusHandle();
        alarmIndicator.delLatchPhyAlarm();
    }

    bool soundHasAlarm = false;
    if (priority > ALARM_PRIO_PROMPT && status == ALARM_STATUS_NORMAL)
    {
        soundHasAlarm = true;
    }

    bool lightHasAlarm = false;
    if (priority != ALARM_PRIO_PROMPT && status != ALARM_STATUS_PAUSE)
    {
        lightHasAlarm = true;
    }

    if (status != ALARM_STATUS_PAUSE)
    {
        EXPECT_CALL(mockPhyInfoBarWidget, display(_));
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_NO_PAUSED_PHY_ALARM, _, _));
        EXPECT_CALL(mockAlarmStateMachine, handAlarmEvent(ALARM_STATE_EVENT_NO_ACKNOWLEDG_ALARM, _, _));
    }
    else
    {
        EXPECT_CALL(mockPhyInfoBarWidget, clear());
    }
    EXPECT_CALL(mockTechInfoBarWidget, clear());
    EXPECT_CALL(mockSoundManager, updateAlarm(soundHasAlarm, highestPriority));
    EXPECT_CALL(mockLightManager, updateAlarm(lightHasAlarm, highestPriority));

    alarmIndicator.publishAlarm(status);

    QVERIFY(Mock::VerifyAndClearExpectations(&mockPhyInfoBarWidget));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockTechInfoBarWidget));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockSoundManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockLightManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockAlarmStateMachine));
}

