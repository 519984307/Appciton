/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <Fangtongzhou@blmed.cn>, 2019/3/5
 **/

#include "TestTimeDate.h"
#include "QThread"
#include <time.h>
#include <stdio.h>
#include <QtGlobal>
#include "IConfig.h"
#include "TimeDefine.h"


TestTimeDate::TestTimeDate()
{
     TimeFormat timeFormat = TimeFormat::TIME_FORMAT_24;
     systemConfig.setNumValue("DateTime|TimeFormat", static_cast<int>(timeFormat));
}

void TestTimeDate::testTime_data()
{
    QTest::addColumn<int>("Time");
    QTest::newRow("time1") << 1000;
    QTest::newRow("time2") << 2000;
    QTest::newRow("time3") << 3000;
}

void TestTimeDate::testTime()
{
   uint time1 = timeDate.time();
   uint time2 = QDateTime::currentDateTime().toTime_t();
   QCOMPARE(time1, time2);

   QFETCH(int, Time);
   QTest::qSleep(Time);

   time2 = QDateTime::currentDateTime().toTime_t();

   QVERIFY2(time1 != time2, "Test fail!");
}

void TestTimeDate::testDifftime_data()
{
    QTest::addColumn<int>("Difftime");

    QTest::newRow("difftime1") << 1;
    QTest::newRow("difftime2") << 2;
    QTest::newRow("difftime3") << 3;
}
void TestTimeDate::testDifftime()
{
    time_t time1 = time(NULL);
    QFETCH(int, Difftime);
    QTest::qSleep(Difftime*1000);
    time_t time2 = time(NULL);

    QCOMPARE(static_cast<int>(timeDate.difftime(time2, time1)), Difftime);
}

void TestTimeDate::testGetTime()
{
    QString str;
    timeDate.getTime(str, true);
    QTime currentTime = QTime::currentTime();
    QString time = QString("%1:%2:%3").arg(currentTime.hour(), 2, 10, QLatin1Char('0')).
                arg(currentTime.minute(), 2, 10, QLatin1Char('0'))
                .arg(currentTime.second(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str, time);

    timeDate.getTime(str, false);
    time = QString("%1:%2").arg(currentTime.hour(), 2, 10, QLatin1Char('0')).
            arg(currentTime.minute(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str, time);
}

void TestTimeDate::testGetDate()
{
    QString str;
    timeDate.getDate(str, true);
    QDate currentDate = QDate::currentDate();
    QString date = QString("%1/%2/%3").arg(currentDate.year(), 4, 10, QLatin1Char('0'))
                    .arg(currentDate.month(), 2, 10, QLatin1Char('0'))
                        .arg(currentDate.day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str, date);

    timeDate.getDate(str, false);
    date = QString("%1/%2/%3").arg(currentDate.year()%100, 2, 10, QLatin1Char('0'))
            .arg(currentDate.month(), 2, 10, QLatin1Char('0'))
                .arg(currentDate.day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str, date);
}

void TestTimeDate::testGetTimeStr_data()
{
    QTest::addColumn<int>("Date");
    QTest::newRow("row1") << 2;
    QTest::newRow("row2") << 246;
    QTest::newRow("row3") << 24682;
    QTest::newRow("row4") << 2468246;
    QTest::newRow("row5") << 246824682;
}

void TestTimeDate::testGetTimeStr()
{
    QFETCH(int, Date);
    QString str;
    timeDate.getTime(Date, str, true);
    QDateTime time = QDateTime::fromTime_t(Date);
    QString  timestr = QString("%1:%2:%3").arg(time.time().hour(), 2, 10, QLatin1Char('0'))
                    .arg(time.time().minute(), 2, 10, QLatin1Char('0'))
                        .arg(time.time().second(), 2, 10, QLatin1Char('0'));
    QCOMPARE(timestr, str);

    timeDate.getTime(Date, str, false);
    timestr = QString("%1:%2").arg(time.time().hour(), 2, 10, QLatin1Char('0'))
            .arg(time.time().minute(), 2, 10, QLatin1Char('0'));
    QCOMPARE(timestr, str);
}

void TestTimeDate::testGetDateStr_data()
{
    QTest::addColumn<int>("Date");
    QTest::newRow("data1") << 1;
    QTest::newRow("data2") << 135;
    QTest::newRow("data3") << 13579;
    QTest::newRow("data4") << 1357913;
    QTest::newRow("data5") << 135791357;
}

void TestTimeDate::testGetDateStr()
{
    QFETCH(int, Date);
    QString str;
    timeDate.getDate(Date, str, true);
    QDateTime date = QDateTime::fromTime_t(Date);
    QString  datestr = QString("%1/%2/%3").arg(date.date().year(), 2, 10, QLatin1Char('0'))
                    .arg(date.date().month(), 2, 10, QLatin1Char('0'))
                        .arg(date.date().day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(datestr, str);

    timeDate.getDate(Date, str, false);
    datestr = QString("%1/%2/%3").arg(date.date().year()%100, 2, 10, QLatin1Char('0'))
            .arg(date.date().month(), 2, 10, QLatin1Char('0'))
                 .arg(date.date().day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(datestr, str);
}

void TestTimeDate::testGetDateYear()
{
    int currentYear = timeDate.getDateYear();
    QDate date = QDate::currentDate();
    QCOMPARE(currentYear, date.year());
}

void TestTimeDate::testGetDateMonth()
{
    int currentMonth = timeDate.getDateMonth();
    QDate date = QDate::currentDate();
    QCOMPARE(currentMonth, date.month());
}

void TestTimeDate::testGetDateDay()
{
    int currentDay = timeDate.getDateDay();
    QDate date = QDate::currentDate();
    QCOMPARE(currentDay, date.day());
}

void TestTimeDate::testGetTimeHour()
{
    int currentHour = timeDate.getTimeHour();
    QTime time = QTime::currentTime();
    QCOMPARE(currentHour, time.hour());
}

void TestTimeDate::testGetTimeMinute()
{
    int currentMinute = timeDate.getTimeMinute();
    QTime time = QTime::currentTime();
    QCOMPARE(currentMinute, time.minute());
}

void TestTimeDate::testGetTimeSecond()
{
    int currentSecond = timeDate.getTimeSenonds();
    QTime time = QTime::currentTime();
    QCOMPARE(currentSecond, time.second());
}

void TestTimeDate::testGetTimeMsec()
{
    int currentMsec = timeDate.getTimeMsec();
    QTime time = QTime::currentTime();
    QCOMPARE(currentMsec, time.msec());
}

//  QTEST_APPLESS_MAIN(TestTimeDate)

//  #include "testtimedate.moc"
