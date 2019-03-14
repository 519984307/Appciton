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

void TestTimeDate::testGetTime_data()
{
    QTest::addColumn<bool>("ifShowSecond");
    QTest::addColumn<bool>("showResult");
    QTest::addColumn<bool>("NotShowEesult");
    QTest::newRow("showSecond") << true << true << false;
    QTest::newRow("NotShowSecond") << false << false << true;
}

void TestTimeDate::testGetTime()
{
    QFETCH(bool, ifShowSecond);
    QFETCH(bool, showResult);
    QFETCH(bool, NotShowEesult);
    QString str;
    timeDate.getTime(str, ifShowSecond);
    QTime currentTime = QTime::currentTime();
    QString time = QString("%1:%2:%3").arg(currentTime.hour(), 2, 10, QLatin1Char('0')).
                arg(currentTime.minute(), 2, 10, QLatin1Char('0'))
                .arg(currentTime.second(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str == time, showResult);

    timeDate.getTime(str, ifShowSecond);
    time = QString("%1:%2").arg(currentTime.hour(), 2, 10, QLatin1Char('0')).
            arg(currentTime.minute(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str == time, NotShowEesult);
}

void TestTimeDate::testGetDate_data()
{
    QTest::addColumn<bool>("ifShow4Bits");
    QTest::addColumn<bool>("showResult");
    QTest::addColumn<bool>("NotShowResult");
    QTest::newRow("showYear4Bits") << true << true << false;
    QTest::newRow("NotShowYear4Bits") << false << false << true;
}
void TestTimeDate::testGetDate()
{
    QFETCH(bool, ifShow4Bits);
    QFETCH(bool, showResult);
    QFETCH(bool, NotShowResult);
    QString str;
    timeDate.getDate(str, ifShow4Bits);
    QDate currentDate = QDate::currentDate();
    QString date = QString("%1/%2/%3").arg(currentDate.year(), 4, 10, QLatin1Char('0'))
                    .arg(currentDate.month(), 2, 10, QLatin1Char('0'))
                        .arg(currentDate.day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str == date, showResult);

    timeDate.getDate(str, ifShow4Bits);
    date = QString("%1/%2/%3").arg(currentDate.year()%100, 2, 10, QLatin1Char('0'))
            .arg(currentDate.month(), 2, 10, QLatin1Char('0'))
                .arg(currentDate.day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str == date, NotShowResult);
}

void TestTimeDate::testGetTimeStr_data()
{
    QTest::addColumn<int>("Date");
    QTest::addColumn<bool>("showSecond");
    QTest::addColumn<bool>("notShowSecond");

    QTest::newRow("row1") << 2 << true << false;
    QTest::newRow("row2") << 246 << true << false;
    QTest::newRow("row3") << 24682 << true << false;
    QTest::newRow("row4") << 2468246 << true << false;
    QTest::newRow("row5") << 246824682 << true << false;
}

void TestTimeDate::testGetTimeStr()
{
    QFETCH(int, Date);
    QFETCH(bool, showSecond);
    QFETCH(bool, notShowSecond);
    QString str;
    timeDate.getTime(Date, str, showSecond);
    QDateTime time = QDateTime::fromTime_t(Date);
    QString  timestr = QString("%1:%2:%3").arg(time.time().hour(), 2, 10, QLatin1Char('0'))
                    .arg(time.time().minute(), 2, 10, QLatin1Char('0'))
                        .arg(time.time().second(), 2, 10, QLatin1Char('0'));
    QCOMPARE(timestr, str);

    timeDate.getTime(Date, str, notShowSecond);
    timestr = QString("%1:%2").arg(time.time().hour(), 2, 10, QLatin1Char('0'))
            .arg(time.time().minute(), 2, 10, QLatin1Char('0'));
    QCOMPARE(timestr, str);
}

void TestTimeDate::testGetDateStr_data()
{
    QTest::addColumn<int>("Date");
    QTest::addColumn<bool>("showYear4Bits");
    QTest::addColumn<bool>("notShowYear4Bits");
    QTest::newRow("data1") << 1 << true << false;
    QTest::newRow("data2") << 135 << true << false;
    QTest::newRow("data3") << 13579 << true << false;
    QTest::newRow("data4") << 1357913 << true << false;
    QTest::newRow("data5") << 135791357 << true << false;
}

void TestTimeDate::testGetDateStr()
{
    QFETCH(int, Date);
    QFETCH(bool, showYear4Bits);
    QFETCH(bool, notShowYear4Bits);
    QString str;
    timeDate.getDate(Date, str, showYear4Bits);
    QDateTime date = QDateTime::fromTime_t(Date);
    QString  datestr = QString("%1/%2/%3").arg(date.date().year(), 2, 10, QLatin1Char('0'))
                    .arg(date.date().month(), 2, 10, QLatin1Char('0'))
                        .arg(date.date().day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(datestr, str);

    timeDate.getDate(Date, str, notShowYear4Bits);
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
