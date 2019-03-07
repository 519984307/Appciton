/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <Fangtongzhou@blmed.cn>, 2019/3/5
 **/

#include "testtimedate.h"
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

void TestTimeDate::testTime()
{
   uint _time1 = timeDate.time();
   uint _time2 = QDateTime::currentDateTime().toTime_t();
   QVERIFY(_time1 = _time2);

   QTest::qSleep(1000);

   _time2 = QDateTime::currentDateTime().toTime_t();

   QVERIFY2(_time1 != _time2, "Test fail!");
}

void TestTimeDate::testDifftime()
{
    time_t time1 = time(NULL);
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    unsigned int interval = qrand() % 10;
    QTest::qSleep(interval*1000);
    time_t time2 = time(NULL);

    QCOMPARE(static_cast<int>(timeDate.difftime(time2, time1)), static_cast<int>(time2 - time1));
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

void TestTimeDate::testGetTimeStr()
{
    QString str;
    QDateTime currenttime = QDateTime::currentDateTime();
    unsigned long t = currenttime.toTime_t();
    timeDate.getTime(t, str, true);

    QTime time = QTime::currentTime();
    QString  timestr = QString("%1:%2:%3").arg(time.hour(), 2, 10, QLatin1Char('0'))
                    .arg(time.minute(), 2, 10, QLatin1Char('0'))
                        .arg(time.second(), 2, 10, QLatin1Char('0'));
    QCOMPARE(timestr, str);

    timeDate.getTime(t, str, false);
    timestr = QString("%1:%2").arg(time.hour(), 2, 10, QLatin1Char('0'))
            .arg(time.minute(), 2, 10, QLatin1Char('0'));
    QCOMPARE(timestr, str);
}

void TestTimeDate::testGetDateStr()
{
    QString str;
    QDateTime currenttime = QDateTime::currentDateTime();
    unsigned long t = currenttime.toTime_t();
    timeDate.getDate(t, str, true);

    QDate date = QDate::currentDate();
    QString  datestr = QString("%1/%2/%3").arg(date.year(), 2, 10, QLatin1Char('0'))
                    .arg(date.month(), 2, 10, QLatin1Char('0'))
                        .arg(date.day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(datestr, str);

    timeDate.getDate(t, str, false);
    datestr = QString("%1/%2/%3").arg(date.year()%100, 2, 10, QLatin1Char('0'))
            .arg(date.month(), 2, 10, QLatin1Char('0'))
                 .arg(date.day(), 2, 10, QLatin1Char('0'));
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
