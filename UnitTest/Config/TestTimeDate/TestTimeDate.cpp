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
     TimeFormat timeFormat = TIME_FORMAT_24;
     systemConfig.setNumValue("DateTime|TimeFormat", static_cast<int>(timeFormat));
}

void TestTimeDate::testTime_data()
{
    QTest::addColumn<int>("Time");
    QTest::addColumn<bool>("result");
    QTest::newRow("oneSecond") << 1000 << true;
    QTest::newRow("twoSecond") << 2000 << true;
    QTest::newRow("threeSecond") << 3000 << true;
}

void TestTimeDate::testTime()
{
   QFETCH(int, Time);
   QFETCH(bool, result);
   uint time1 = timeDate.time();
   uint time2 = QDateTime::currentDateTime().toTime_t();
   QCOMPARE(time1 == time2, result);

   QTest::qSleep(Time);

   time2 = QDateTime::currentDateTime().toTime_t();

   QCOMPARE((static_cast<bool>(time2 - time1 - Time/1000)), !result);
}

void TestTimeDate::testDifftime_data()
{
    QTest::addColumn<int>("Difftime");
    QTest::addColumn<bool>("result");
    QTest::newRow("oneSecond") << 1 << true;
    QTest::newRow("twoSecond") << 2 << true;
    QTest::newRow("threeSecond") << 3 << true;
}

void TestTimeDate::testDifftime()
{
    QFETCH(int, Difftime);
    QFETCH(bool, result);
    time_t time1 = time(NULL);
    QTest::qSleep(Difftime*1000);
    time_t time2 = time(NULL);

    QCOMPARE(!(static_cast<bool>(timeDate.difftime(time2, time1) - Difftime)), result);
}

void TestTimeDate::testGetTime_data()
{
    QTest::addColumn<bool>("ifShowSecond");
    QTest::addColumn<bool>("result");
    QTest::newRow("showSecond") << true << true;
    QTest::newRow("NotShowSecond") << false << true;
}

void TestTimeDate::testGetTime()
{
    QFETCH(bool, ifShowSecond);
    QFETCH(bool, result);
    QString str;
    QString time;
    timeDate.getTime(str, ifShowSecond);
    QTime currentTime = QTime::currentTime();
    if (ifShowSecond)
    {
    time = QString("%1:%2:%3").arg(currentTime.hour(), 2, 10, QLatin1Char('0')).
                arg(currentTime.minute(), 2, 10, QLatin1Char('0'))
                .arg(currentTime.second(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str == time, result);
    }
    else
    {
    timeDate.getTime(str, ifShowSecond);
    time = QString("%1:%2").arg(currentTime.hour(), 2, 10, QLatin1Char('0')).
            arg(currentTime.minute(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str == time, result);
    }
}

void TestTimeDate::testGetDate_data()
{
    QTest::addColumn<bool>("ifShow4Bits");
    QTest::addColumn<bool>("result");
    QTest::newRow("showYear4Bits") << true << true;
    QTest::newRow("NotShowYear4Bits") << false << true;
}
void TestTimeDate::testGetDate()
{
    QFETCH(bool, ifShow4Bits);
    QFETCH(bool, result);
    QString str;
    QString date;
    timeDate.getDate(str, ifShow4Bits);
    QDate currentDate = QDate::currentDate();
    if (ifShow4Bits)
    {
    date = QString("%1/%2/%3").arg(currentDate.year(), 4, 10, QLatin1Char('0'))
                    .arg(currentDate.month(), 2, 10, QLatin1Char('0'))
                        .arg(currentDate.day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str == date, result);
    }
    else
    {
    date = QString("%1/%2/%3").arg(currentDate.year()%100, 2, 10, QLatin1Char('0'))
            .arg(currentDate.month(), 2, 10, QLatin1Char('0'))
                .arg(currentDate.day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(str == date, result);
    }
}

void TestTimeDate::testGetTimeStr_data()
{
    QTest::addColumn<int>("Date");
    QTest::addColumn<bool>("ifShowSecond");
    QTest::addColumn<bool>("result");
    QTest::newRow("random2") << 2 << true << true;
    QTest::newRow("random246") << 246 << true << true;
    QTest::newRow("random24682") << 24682 << false << true;
    QTest::newRow("random2468246") << 2468246 << true << true;
    QTest::newRow("random246824682") << 246824682 << false << true;
}

void TestTimeDate::testGetTimeStr()
{
    QFETCH(int, Date);
    QFETCH(bool, ifShowSecond);
    QFETCH(bool, result);
    QString str;
    QString timestr;
    timeDate.getTime(Date, str, ifShowSecond);
    QDateTime time = QDateTime::fromTime_t(Date);
    if (ifShowSecond)
    {
    timestr = QString("%1:%2:%3").arg(time.time().hour(), 2, 10, QLatin1Char('0'))
                    .arg(time.time().minute(), 2, 10, QLatin1Char('0'))
                        .arg(time.time().second(), 2, 10, QLatin1Char('0'));
    QCOMPARE(timestr == str, result);
    }
    else
    {
    timestr = QString("%1:%2").arg(time.time().hour(), 2, 10, QLatin1Char('0'))
            .arg(time.time().minute(), 2, 10, QLatin1Char('0'));
    QCOMPARE(timestr == str, result);
    }
}

void TestTimeDate::testGetDateStr_data()
{
    QTest::addColumn<int>("Date");
    QTest::addColumn<bool>("ifShowYear4Bits");
    QTest::addColumn<bool>("result");
    QTest::newRow("random1") << 1 << true << true;
    QTest::newRow("random135") << 135 << false << true;
    QTest::newRow("random13579") << 13579 << true << true;
    QTest::newRow("random1357913") << 1357913 << false << true;
    QTest::newRow("random135791357") << 135791357 << true << true;
}

void TestTimeDate::testGetDateStr()
{
    QFETCH(int, Date);
    QFETCH(bool, ifShowYear4Bits);
    QFETCH(bool, result);
    QString str;
    QString  datestr;
    timeDate.getDate(Date, str, ifShowYear4Bits);
    QDateTime date = QDateTime::fromTime_t(Date);
    if (ifShowYear4Bits)
    {
    datestr = QString("%1/%2/%3").arg(date.date().year(), 2, 10, QLatin1Char('0'))
                    .arg(date.date().month(), 2, 10, QLatin1Char('0'))
                        .arg(date.date().day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(datestr == str, result);
    }
    else
    {
    datestr = QString("%1/%2/%3").arg(date.date().year()%100, 2, 10, QLatin1Char('0'))
            .arg(date.date().month(), 2, 10, QLatin1Char('0'))
                 .arg(date.date().day(), 2, 10, QLatin1Char('0'));
    QCOMPARE(datestr == str, result);
    }
}

void TestTimeDate::testGetDateYear_data()
{
    QTest::addColumn<int>("testNum");
    QTest::addColumn<bool>("result");
    QTest::newRow("testNum100") << 100 << true;
    QTest::newRow("testNum200") << 200 << true;
    QTest::newRow("testNum500") << 500 << true;
    QTest::newRow("testNum1000") << 10000 << true;
}

void TestTimeDate::testGetDateYear()
{
    QFETCH(int, testNum);
    QFETCH(bool, result);
    for (int i = 0; i < testNum; i++)
    {
    int currentYear = timeDate.getDateYear();
    QDate date = QDate::currentDate();
    QCOMPARE(currentYear == date.year(), result);
    }
}

void TestTimeDate::testGetDateMonth_data()
{
    QTest::addColumn<int>("testNum");
    QTest::addColumn<bool>("result");
    QTest::newRow("testNum100") << 100 << true;
    QTest::newRow("testNum200") << 200 << true;
    QTest::newRow("testNum500") << 500 << true;
    QTest::newRow("testNum1000") << 10000 << true;
}

void TestTimeDate::testGetDateMonth()
{
    QFETCH(int, testNum);
    QFETCH(bool, result);
    for (int i = 0; i < testNum; i++)
    {
    int currentMonth = timeDate.getDateMonth();
    QDate date = QDate::currentDate();
    QCOMPARE(currentMonth == date.month(), result);
    }
}

void TestTimeDate::testGetDateDay_data()
{
    QTest::addColumn<int>("testNum");
    QTest::addColumn<bool>("result");
    QTest::newRow("testNum100") << 100 << true;
    QTest::newRow("testNum200") << 200 << true;
    QTest::newRow("testNum500") << 500 << true;
    QTest::newRow("testNum1000") << 10000 << true;
}

void TestTimeDate::testGetDateDay()
{
    QFETCH(int, testNum);
    QFETCH(bool, result);
    for (int i = 0; i < testNum; i++)
    {
    int currentDay = timeDate.getDateDay();
    QDate date = QDate::currentDate();
    QCOMPARE(currentDay ==  date.day(), result);
    }
}

void TestTimeDate::testGetTimeHour_data()
{
    QTest::addColumn<int>("testNum");
    QTest::addColumn<bool>("result");
    QTest::newRow("testNum100") << 100 << true;
    QTest::newRow("testNum200") << 200 << true;
    QTest::newRow("testNum500") << 500 << true;
    QTest::newRow("testNum1000") << 10000 << true;
}

void TestTimeDate::testGetTimeHour()
{
    QFETCH(int, testNum);
    QFETCH(bool, result);
    for (int i = 0; i < testNum; i++)
    {
    int currentHour = timeDate.getTimeHour();
    QTime time = QTime::currentTime();
    QCOMPARE(currentHour ==  time.hour(), result);
    }
}

void TestTimeDate::testGetTimeMinute_data()
{
    QTest::addColumn<int>("testNum");
    QTest::addColumn<bool>("result");
    QTest::newRow("testNum100") << 100 << true;
    QTest::newRow("testNum200") << 200 << true;
    QTest::newRow("testNum500") << 500 << true;
    QTest::newRow("testNum1000") << 10000 << true;
}

void TestTimeDate::testGetTimeMinute()
{
    QFETCH(int, testNum);
    QFETCH(bool, result);
    for (int i = 0; i < testNum; i++)
    {
    int currentMinute = timeDate.getTimeMinute();
    QTime time = QTime::currentTime();
    QCOMPARE(currentMinute == time.minute(), result);
    }
}

void TestTimeDate::testGetTimeSecond_data()
{
    QTest::addColumn<int>("testNum");
    QTest::addColumn<bool>("result");
    QTest::newRow("testNum100") << 100 << true;
    QTest::newRow("testNum200") << 200 << true;
    QTest::newRow("testNum500") << 500 << true;
    QTest::newRow("testNum1000") << 10000 << true;
}

void TestTimeDate::testGetTimeSecond()
{
    QFETCH(int, testNum);
    QFETCH(bool, result);
    for (int i = 0; i < testNum; i++)
    {
    int currentSecond = timeDate.getTimeSenonds();
    QTime time = QTime::currentTime();
    QCOMPARE(time.second() - currentSecond <= 1, result);
    }
}

void TestTimeDate::testGetTimeMsec_data()
{
    QTest::addColumn<int>("testNum");
    QTest::addColumn<bool>("result");
    QTest::newRow("testNum100") << 100 << true;
}

void TestTimeDate::testGetTimeMsec()
{
    QFETCH(int, testNum);
    QFETCH(bool, result);
    for (int i = 0; i < testNum; i++)
    {
        int currentMsec = timeDate.getTimeMsec();
        QTime time = QTime::currentTime();
        QCOMPARE(qAbs(currentMsec - time.msec()) <= 1, result);
    }
}

//  QTEST_APPLESS_MAIN(TestTimeDate)

//  #include "testtimedate.moc"
