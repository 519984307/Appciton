/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/17
 **/

#include "TimeDate.h"
#include <QDateTime>
#include <ctime>
#include "IConfig.h"
#include "TimeDefine.h"

TimeDate *TimeDate::_selfObj = NULL;

/**************************************************************************************************
 * 功能：根据配置获取正确的日期显示格式。
 * 参数：
 *      format：返回日期格式；
 *      year4Bits：年是显示4位还是后两位。
 *************************************************************************************************/
void TimeDate::getDateFormat(QString &format, bool year4Bits)
{
    int dateFormat = 0;
    systemConfig.getNumValue("DateTime|DateFormat", dateFormat);

    if (year4Bits)
    {
        if (dateFormat == DATE_FORMAT_Y_M_D)
        {
            format = "yyyy/MM/dd";
        }
        else if (dateFormat == DATE_FORMAT_M_D_Y)
        {
            format = "MM/dd/yyyy";
        }
        else
        {
            format = "dd/MM/yyyy";
        }
    }
    else
    {
        if (dateFormat == DATE_FORMAT_Y_M_D)
        {
            format = "yy/MM/dd";
        }
        else if (dateFormat == DATE_FORMAT_M_D_Y)
        {
            format = "MM/dd/yy";
        }
        else
        {
            format = "dd/MM/yy";
        }
    }
}

/**************************************************************************************************
 * 功能：根据配置获取正确的时间显示格式。
 * 参数：
 *      format：返回时间格式；
 *      showSecond:是否显示秒
 *************************************************************************************************/
static void getTimeFormat(QString &format, bool showSecond)
{
    int timeFormat = 0;

    systemConfig.getNumValue("DateTime|TimeFormat", timeFormat);
    if (showSecond)
    {
        if (TIME_FORMAT_24 == timeFormat)
        {
            format = "hh:mm:ss";
        }
        else
        {
            format = "hh:mm:ss AP";
        }
    }
    else
    {
        if (TIME_FORMAT_24 == timeFormat)
        {
            format = "hh:mm";
        }
        else
        {
            format = "hh:mm AP";
        }
    }
}

/**************************************************************************************************
 * 功能：与标准C库中的time()函数功能一致。
 *************************************************************************************************/
unsigned TimeDate::time(void)
{
    return QDateTime::currentDateTime().toTime_t();
}

/**************************************************************************************************
 * 功能：与标准C库中的difftime()函数功能一致，返回两个time_t型变量之间的时间间隔。
 *************************************************************************************************/
double TimeDate::difftime(unsigned time2, unsigned time1)
{
    return ::difftime(time2, time1);
}

/**************************************************************************************************
 * 功能：获取当前的时间。
 * 参数：
 *      str：返回计算的结果。
 *************************************************************************************************/
void TimeDate::getTime(QString &str, bool showSecond)
{
    QString timeFormat;
    getTimeFormat(timeFormat, showSecond);
    str = QDateTime::currentDateTime().toString(timeFormat);
}

/**************************************************************************************************
 * 功能：获取当前的日期，以设定的日期格式由str返回。
 * 参数：
 *      str：返回计算的结果。
 *************************************************************************************************/
void TimeDate::getDate(QString &str, bool year4Bits)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    QString format;
    getDateFormat(format, year4Bits);
    str = currentDateTime.toString(format);
}

/**************************************************************************************************
 * 功能：获取当前的日期时间，以设定的日期格式由str返回。
 * 参数：
 *      str：返回计算的结果。
 *************************************************************************************************/
void TimeDate::getDateTime(QString &str, bool year4Bits, bool showSecond)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateFormat;
    getDateFormat(dateFormat, year4Bits);

    QString timeFormat;
    getTimeFormat(timeFormat, showSecond);

    str = currentDateTime.toString(dateFormat + " " + timeFormat);
}

/**************************************************************************************************
 * 功能：计算出t表示的时间，以设定的日期格式由str返回。
 * 参数：
 *      t：需计算的时间。
 *      str：返回计算的结果。
 *************************************************************************************************/
void TimeDate::getTime(unsigned t, QString &str, bool showSecond)
{
    QString timeFormat;
    getTimeFormat(timeFormat, showSecond);
    str = QDateTime::fromTime_t(t).toString(timeFormat);
}

/**************************************************************************************************
 * 功能：计算出t表示的日期，以设定的日期格式由str返回。
 * 参数：
 *      t：需计算的时间。
 *      str：返回计算的结果。
 *************************************************************************************************/
void TimeDate::getDate(unsigned t, QString &str, bool year4Bits)
{
    QDateTime currentDateTime = QDateTime::fromTime_t(t);

    QString format;
    getDateFormat(format, year4Bits);
    str = currentDateTime.toString(format);
}

/**************************************************************************************************
 * 功能：计算出t表示的日期时间，以设定的日期格式由str返回。
 * 参数：
 *      t：需计算的时间。
 *      str：返回计算的结果。
 *************************************************************************************************/
void TimeDate::getDateTime(unsigned t, QString &str, bool year4Bits, bool showSecond)
{
    QDateTime currentDateTime = QDateTime::fromTime_t(t);

    QString format;
    getDateFormat(format, year4Bits);

    QString timeFormat;
    getTimeFormat(timeFormat, showSecond);

    str = currentDateTime.toString(format + " " + timeFormat);
}

/**************************************************************************************************
 * 功能：获取当前时间具体的年份。
 * 参数：无
 *************************************************************************************************/
unsigned int TimeDate::getDateYear(unsigned time)
{
    QDateTime currentDateTime;

    if (0 == time)
    {
        currentDateTime = QDateTime::currentDateTime();
    }
    else
    {
        currentDateTime = QDateTime::fromTime_t(time);
    }

    return currentDateTime.date().year();
}

/**************************************************************************************************
 * 功能：获取当前时间具体的月份。
 * 参数：
 *     time:时间(秒表示)
 *************************************************************************************************/
unsigned int TimeDate::getDateMonth(unsigned time)
{
    QDateTime currentDateTime;

    if (0 == time)
    {
        currentDateTime = QDateTime::currentDateTime();
    }
    else
    {
        currentDateTime = QDateTime::fromTime_t(time);
    }

    return currentDateTime.date().month();
}

/**************************************************************************************************
 * 功能：获取当前时间具体的天。
 * 参数：
 *     time:时间(秒表示)
 *************************************************************************************************/
unsigned int TimeDate::getDateDay(unsigned time)
{
    QDateTime currentDateTime;

    if (0 == time)
    {
        currentDateTime = QDateTime::currentDateTime();
    }
    else
    {
        currentDateTime = QDateTime::fromTime_t(time);
    }

    return currentDateTime.date().day();
}

/**************************************************************************************************
 * 功能：获取具体的时。
 * 参数：
 *     time:时间(秒表示)
 *************************************************************************************************/
unsigned int TimeDate::getTimeHour(unsigned time)
{
    QDateTime currentDateTime;

    if (0 == time)
    {
        currentDateTime = QDateTime::currentDateTime();
    }
    else
    {
        currentDateTime = QDateTime::fromTime_t(time);
    }

    return currentDateTime.time().hour();
}

/**************************************************************************************************
 * 功能：获取具体的分钟。
 * 参数：
 *     time:时间(秒表示)
 *************************************************************************************************/
unsigned int TimeDate::getTimeMinute(unsigned time)
{
    QDateTime currentDateTime;

    if (0 == time)
    {
        currentDateTime = QDateTime::currentDateTime();
    }
    else
    {
        currentDateTime = QDateTime::fromTime_t(time);
    }

    return currentDateTime.time().minute();
}

/**************************************************************************************************
 * 功能：获取具体的秒。
 * 参数：
 *     time:时间(秒表示)
 *************************************************************************************************/
unsigned int TimeDate::getTimeSenonds(unsigned time)
{
    QDateTime currentDateTime;

    if (0 == time)
    {
        currentDateTime = QDateTime::currentDateTime();
    }
    else
    {
        currentDateTime = QDateTime::fromTime_t(time);
    }

    return currentDateTime.time().second();
}

/**************************************************************************************************
 * 功能：获取具体的毫秒。
 *************************************************************************************************/
unsigned int TimeDate::getTimeMsec(void)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    return currentDateTime.time().msec();
}

/**************************************************************************************************
 * 功能：构造。
 *************************************************************************************************/
TimeDate::TimeDate()
{
}

/**************************************************************************************************
 * 功能：析构。
 *************************************************************************************************/
TimeDate::~TimeDate()
{
}

