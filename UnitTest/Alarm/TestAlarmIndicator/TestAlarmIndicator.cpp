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

Q_DECLARE_METATYPE(AlarmParamIFace *)
Q_DECLARE_METATYPE(AlarmType)

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

    QTest::newRow("phyNoLatch") << ALARM_TYPE_PHY << 1<< true;
    QTest::newRow("phyLatch") << ALARM_TYPE_PHY << 1 << true;
    QTest::newRow("techNoLatch") << ALARM_TYPE_TECH << 0 << true;
    QTest::newRow("techLatch") << ALARM_TYPE_TECH << 0 << true;
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

void TestAlarmIndicator::testPhyAlarmPauseStatusHandle()
{
}

void TestAlarmIndicator::testHasNonPausePhyAlarm()
{
}

void TestAlarmIndicator::testHasLatchPhyAlarm()
{
}

void TestAlarmIndicator::testDelAllPhyAlarm()
{
}

void TestAlarmIndicator::testClearAlarmPause()
{
}

void TestAlarmIndicator::testUpdataAlarmPriority()
{
}

void TestAlarmIndicator::testUpdateAlarmInfo()
{
}

void TestAlarmIndicator::testPublishAlarm()
{
}

void TestAlarmIndicator::testSetAlarmStatus()
{
}

void TestAlarmIndicator::testPhyAlarmResetStatusHandle()
{
}

void TestAlarmIndicator::testTechAlarmResetStatusHandle()
{
}

void TestAlarmIndicator::testUpdateAlarmStateWidget()
{
}

void TestAlarmIndicator::testDelLatchPhyAlarm()
{
}
