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
#include "Framework/TimeDate/TimeDefine.h"

static QString getTimeFormatStr(TimeFormat timeformat, bool showSeconds)
{
    QString formatStr = "hh:mm";
    if (showSeconds)
    {
        formatStr += ":ss";
    }

    if (TIME_FORMAT_12 == timeformat)
    {
        formatStr += " AP";
    }

    return formatStr;
}

static QString getDateFormatStr(DateFormat dateformat, bool year4digits)
{
    QString formatStr;
    if (year4digits)
    {
        if (dateformat == DATE_FORMAT_Y_M_D)
        {
            formatStr = "yyyy/MM/dd";
        }
        else if (dateformat == DATE_FORMAT_M_D_Y)
        {
            formatStr = "MM/dd/yyyy";
        }
        else if (dateformat == DATE_FORMAT_D_M_Y)
        {
            formatStr = "dd/MM/yyyy";
        }
    }
    else
    {
        if (dateformat == DATE_FORMAT_Y_M_D)
        {
            formatStr = "yy/MM/dd";
        }
        else if (dateformat == DATE_FORMAT_M_D_Y)
        {
            formatStr = "MM/dd/yy";
        }
        else if (dateformat == DATE_FORMAT_D_M_Y)
        {
            formatStr = "dd/MM/yy";
        }
    }
    return formatStr;
}

TestTimeDate::TestTimeDate()
{
}

void TestTimeDate::init()
{
    if (!instance)
    {
        /* create the instance */
        timeDate->getInstance();
    }
}

void TestTimeDate::cleanup()
{
    if (instance)
    {
        /* delete instance */
        delete instance;
        instance = NULL;
    }
}

void TestTimeDate::testGetDateFormat()
{
    /* default date format will be DATE_FORMAT_Y_M_D */
    QCOMPARE(timeDate->getDateFormat(), DATE_FORMAT_Y_M_D);
}

void TestTimeDate::testSetDateFormat()
{
    timeDate->setDateFormat(DATE_FORMAT_M_D_Y);
    QCOMPARE(timeDate->getDateFormat(), DATE_FORMAT_M_D_Y);

    timeDate->setDateFormat(DATE_FORMAT_D_M_Y);
    QCOMPARE(timeDate->getDateFormat(), DATE_FORMAT_D_M_Y);

    /* set invalid format will not change the exist format */
    timeDate->setDateFormat(DATE_FORMAT_NR);
    QCOMPARE(timeDate->getDateFormat(), DATE_FORMAT_D_M_Y);
}

void TestTimeDate::testGetTimeFormat()
{
    /* default time format will be TIME_FORMAT_12 */
    QCOMPARE(timeDate->getTimeFormat(), TIME_FORMAT_12);
}

void TestTimeDate::testSetTimeformat()
{
    timeDate->setTimeFormat(TIME_FORMAT_24);
    QCOMPARE(timeDate->getTimeFormat(), TIME_FORMAT_24);

    /* set invalid format will not change the exist format */
    timeDate->setTimeFormat(TIME_FORMAT_NR);
    QCOMPARE(timeDate->getTimeFormat(), TIME_FORMAT_24);
}

void TestTimeDate::testTime()
{
   uint time1 = timeDate->time();
   uint time2 = QDateTime::currentDateTime().toTime_t();

   QVERIFY2(time1 == time2, "timestamp should be equal to the system timestamp");
}

void TestTimeDate::testDifftime_data()
{
    QTest::addColumn<int>("diffTime");
    QTest::newRow("oneSecond") << 1;
    QTest::newRow("twoSecond") << 2;
}

void TestTimeDate::testDifftime()
{
    QFETCH(int, diffTime);
    time_t time1 = ::time(NULL);
    QTest::qSleep(diffTime*1000);
    time_t time2 = ::time(NULL);

    QCOMPARE(static_cast<int>(timeDate->difftime(time2, time1)), diffTime);
}

void TestTimeDate::testGetTime_data()
{
    QTest::addColumn<TimeFormat>("format");
    QTest::addColumn<bool>("showSeconds");
    QTest::newRow("12h format, show seconds") << TIME_FORMAT_12 << true;
    QTest::newRow("12h format, not show seconds") << TIME_FORMAT_12 << false;
    QTest::newRow("24h fromat, show seconds") << TIME_FORMAT_24 << true;
    QTest::newRow("24h fromat, not show seconds") << TIME_FORMAT_24 << false;
}

void TestTimeDate::testGetTime()
{
    QFETCH(TimeFormat, format);
    QFETCH(bool, showSeconds);

    timeDate->setTimeFormat(format);

    QString timeStr1 = timeDate->getTime(showSeconds);
    QString formatStr = getTimeFormatStr(format, showSeconds);
    QString timeStr2 = QDateTime::currentDateTime().toString(formatStr);

    QCOMPARE(timeStr1, timeStr2);
}

void TestTimeDate::testGetDate_data()
{
    QTest::addColumn<DateFormat>("format");
    QTest::addColumn<bool>("year4digits");

    QTest::newRow("YYYY/MM/DD") << DATE_FORMAT_Y_M_D << true;
    QTest::newRow("DD/MM/YYYY") << DATE_FORMAT_D_M_Y << true;
    QTest::newRow("DD/MM/YYYY") << DATE_FORMAT_M_D_Y << true;
    QTest::newRow("DD/MM/YY") << DATE_FORMAT_D_M_Y << false;
}
void TestTimeDate::testGetDate()
{
    QFETCH(DateFormat, format);
    QFETCH(bool, year4digits);

    timeDate->setDateFormat(format);
    QString dateStr1 = timeDate->getDate(year4digits);
    QString formatstr = getDateFormatStr(format, year4digits);
    QString dateStr2 = QDateTime::currentDateTime().toString(formatstr);

    QCOMPARE(dateStr1, dateStr2);
}

void TestTimeDate::testGetDateTime_data()
{
    QTest::addColumn<DateFormat>("dateformat");
    QTest::addColumn<TimeFormat>("timeformat");
    QTest::addColumn<bool>("year4digits");
    QTest::addColumn<bool>("showSeconds");

    QTest::newRow("YYYY/MM/DD, 12h, show second") << DATE_FORMAT_Y_M_D
                                                  << TIME_FORMAT_12  << true << true;
    QTest::newRow("DD/MM/YYYY, 24h, not show second") << DATE_FORMAT_D_M_Y
                                                      << TIME_FORMAT_24 << true << false;
    QTest::newRow("DD/MM/YY, 12h, show second") << DATE_FORMAT_M_D_Y
                                                << TIME_FORMAT_12 << false << true;
    QTest::newRow("DD/MM/YY, 24h, not show second") << DATE_FORMAT_D_M_Y
                                                    << TIME_FORMAT_24 << false << false;
}

void TestTimeDate::testGetDateTime()
{
    QFETCH(DateFormat, dateformat);
    QFETCH(TimeFormat, timeformat);
    QFETCH(bool, year4digits);
    QFETCH(bool, showSeconds);

    timeDate->setDateFormat(dateformat);
    timeDate->setTimeFormat(timeformat);

    QString dateStr1 = timeDate->getDateTime(year4digits, showSeconds);
    QString formatstr = getDateFormatStr(dateformat, year4digits);
    formatstr += " " + getTimeFormatStr(timeformat, showSeconds);
    QString dateStr2 = QDateTime::currentDateTime().toString(formatstr);

    QCOMPARE(dateStr1, dateStr2);
}

void TestTimeDate::testGetTimeStr_data()
{
    QTest::addColumn<uint>("timestamp");
    QTest::addColumn<TimeFormat>("format");
    QTest::addColumn<bool>("showSeconds");
    QTest::newRow("12h format, show seconds") << 0u << TIME_FORMAT_12 << true;
    QTest::newRow("12h format, not show seconds") << QDateTime::currentDateTime().toTime_t()
                                                  << TIME_FORMAT_12 << false;
    QTest::newRow("24h fromat, show seconds") << QDateTime::currentDateTime().toTime_t()
                                              << TIME_FORMAT_24 << true;
    QTest::newRow("24h fromat, not show seconds") << UINT_MAX << TIME_FORMAT_24 << false;
    QTest::newRow("24h fromat, show seconds") << UINT_MAX << TIME_FORMAT_24 << true;
}

void TestTimeDate::testGetTimeStr()
{
    QFETCH(uint, timestamp);
    QFETCH(TimeFormat, format);
    QFETCH(bool, showSeconds);

    timeDate->setTimeFormat(format);

    QString timeStr1 = timeDate->getTime(timestamp, showSeconds);
    QString formatStr = getTimeFormatStr(format, showSeconds);
    QString timeStr2 = QDateTime::fromTime_t(timestamp).toString(formatStr);

    QCOMPARE(timeStr1, timeStr2);
}

void TestTimeDate::testGetDateStr_data()
{
    QTest::addColumn<uint>("timestamp");
    QTest::addColumn<DateFormat>("format");
    QTest::addColumn<bool>("year4digits");

    QTest::newRow("YYYY/MM/DD") << 0u << DATE_FORMAT_Y_M_D << true;
    QTest::newRow("DD/MM/YYYY") << QDateTime::currentDateTime().toTime_t() << DATE_FORMAT_D_M_Y << true;
    QTest::newRow("DD/MM/YYYY") << UINT_MAX <<DATE_FORMAT_M_D_Y << true;
    QTest::newRow("DD/MM/YY") << QDateTime::currentDateTime().toTime_t() << DATE_FORMAT_D_M_Y << false;
}

void TestTimeDate::testGetDateStr()
{
    QFETCH(uint, timestamp);
    QFETCH(DateFormat, format);
    QFETCH(bool, year4digits);

    timeDate->setDateFormat(format);
    QString dateStr1 = timeDate->getDate(timestamp, year4digits);
    QString formatStr = getDateFormatStr(format, year4digits);
    QString dateStr2 = QDateTime::fromTime_t(timestamp).toString(formatStr);

    QCOMPARE(dateStr1, dateStr2);
}

void TestTimeDate::testGetDateTimeStr_data()
{
    QTest::addColumn<uint>("timestamp");
    QTest::addColumn<DateFormat>("dateformat");
    QTest::addColumn<TimeFormat>("timeformat");
    QTest::addColumn<bool>("year4digits");
    QTest::addColumn<bool>("showSeconds");

    QTest::newRow("YYYY/MM/DD, 12h, show second") << 0u << DATE_FORMAT_Y_M_D
                                                  << TIME_FORMAT_12  << true << true;
    QTest::newRow("DD/MM/YYYY, 24h, not show second") << QDateTime::currentDateTime().toTime_t()
                                                      << DATE_FORMAT_D_M_Y << TIME_FORMAT_24
                                                      << true << false;
    QTest::newRow("DD/MM/YY, 12h, show second")  << QDateTime::currentDateTime().toTime_t()
                                                 << DATE_FORMAT_M_D_Y << TIME_FORMAT_12
                                                 << false << true;
    QTest::newRow("DD/MM/YY, 24h, not show second") << UINT_MAX << DATE_FORMAT_D_M_Y
                                                    << TIME_FORMAT_24 << false << false;
}

void TestTimeDate::testGetDateTimeStr()
{
    QFETCH(uint, timestamp);
    QFETCH(DateFormat, dateformat);
    QFETCH(TimeFormat, timeformat);
    QFETCH(bool, year4digits);
    QFETCH(bool, showSeconds);

    timeDate->setTimeFormat(timeformat);
    timeDate->setDateFormat(dateformat);

    QString dateStr1 = timeDate->getDateTime(timestamp, year4digits, showSeconds);
    QString formatstr = getDateFormatStr(dateformat, year4digits);
    formatstr += " " + getTimeFormatStr(timeformat, showSeconds);
    QString dateStr2 = QDateTime::fromTime_t(timestamp).toString(formatstr);

    QCOMPARE(dateStr1, dateStr2);
}

void TestTimeDate::testGetDateYear_data()
{
    QTest::addColumn<uint>("timestamp");
    QTest::addColumn<uint>("value");

    QTest::newRow("current time") << 0u
                                  << static_cast<uint>(QDateTime::currentDateTime().date().year());
    QTest::newRow("2019/11/12 16:51:30") << QDateTime(QDate(2019, 11, 12), QTime(16, 51, 30)).toTime_t()
                                         << 2019u;
}

void TestTimeDate::testGetDateYear()
{
    QFETCH(uint, timestamp);
    QFETCH(uint, value);

    QCOMPARE(timeDate->getDateYear(timestamp), value);
}

void TestTimeDate::testGetDateMonth_data()
{
    QTest::addColumn<uint>("timestamp");
    QTest::addColumn<uint>("value");

    QTest::newRow("current time") << 0u
                                  << static_cast<uint>(QDateTime::currentDateTime().date().month());
    QTest::newRow("2019/11/12 16:51:30") << QDateTime(QDate(2019, 11, 12), QTime(16, 51, 30)).toTime_t()
                                         << 11u;
}

void TestTimeDate::testGetDateMonth()
{
    QFETCH(uint, timestamp);
    QFETCH(uint, value);

    QCOMPARE(timeDate->getDateMonth(timestamp), value);
}

void TestTimeDate::testGetDateDay_data()
{
    QTest::addColumn<uint>("timestamp");
    QTest::addColumn<uint>("value");

    QTest::newRow("current time") << 0u << static_cast<uint>(QDateTime::currentDateTime().date().day());
    QTest::newRow("2019/11/12 16:51:30") << QDateTime(QDate(2019, 11, 12), QTime(16, 51, 30)).toTime_t()
                                         << 12u;
}

void TestTimeDate::testGetDateDay()
{
    QFETCH(uint, timestamp);
    QFETCH(uint, value);

    QCOMPARE(timeDate->getDateDay(timestamp), value);
}

void TestTimeDate::testGetTimeHour_data()
{
    QTest::addColumn<uint>("timestamp");
    QTest::addColumn<uint>("value");

    QTest::newRow("current time") << 0u << static_cast<uint>(QDateTime::currentDateTime().time().hour());
    QTest::newRow("2019/11/12 16:51:30") << QDateTime(QDate(2019, 11, 12), QTime(16, 51, 30)).toTime_t()
                                         << 16u;
}

void TestTimeDate::testGetTimeHour()
{
    QFETCH(uint, timestamp);
    QFETCH(uint, value);

    QCOMPARE(timeDate->getTimeHour(timestamp), value);
}

void TestTimeDate::testGetTimeMinute_data()
{
    QTest::addColumn<uint>("timestamp");
    QTest::addColumn<uint>("value");

    QTest::newRow("current time") << 0u << static_cast<uint>(QDateTime::currentDateTime().time().minute());
    QTest::newRow("2019/11/12 16:51:30") << QDateTime(QDate(2019, 11, 12), QTime(16, 51, 30)).toTime_t()
                                         << 51u;
}

void TestTimeDate::testGetTimeMinute()
{
    QFETCH(uint, timestamp);
    QFETCH(uint, value);

    QCOMPARE(timeDate->getTimeMinute(timestamp), value);
}

void TestTimeDate::testGetTimeSecond_data()
{
    QTest::addColumn<uint>("timestamp");
    QTest::addColumn<uint>("value");

    QTest::newRow("current time") << 0u << static_cast<uint>(QDateTime::currentDateTime().time().second());
    QTest::newRow("2019/11/12 16:51:30") << QDateTime(QDate(2019, 11, 12), QTime(16, 51, 30)).toTime_t()
                                         << 30u;
}

void TestTimeDate::testGetTimeSecond()
{
    QFETCH(uint, timestamp);
    QFETCH(uint, value);

    QCOMPARE(timeDate->getTimeSeconds(timestamp), value);
}
