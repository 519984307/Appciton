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

TimeDate *TimeDate::instance = NULL;
class TimeDatePrivate
{
public:
    TimeDatePrivate() :timeFormat(TIME_FORMAT_12), dateFormat(DATE_FORMAT_Y_M_D) {}

    /**
     * @brief getTimeFormatStr get the timeformat string
     * @param showSecond whether show second
     * @return the format string
     */
    QString getTimeFormatStr(bool showSecond)
    {
        QString format;
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
        return  format;
    }

    /**
     * @brief getDateFormatStr get the date fromat string
     * @param year4digits show four digits year number
     * @return  the date format string
     */
    QString getDateFormatStr(bool year4digits)
    {
        QString format;
        if (year4digits)
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
        return format;
    }

    TimeFormat timeFormat;
    DateFormat dateFormat;
};

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
QString TimeDate::getTime(bool showSecond)
{
    QString timeFormat = d_ptr->getTimeFormatStr(showSecond);
    return QDateTime::currentDateTime().toString(timeFormat);
}

/**************************************************************************************************
 * 功能：获取当前的日期，以设定的日期格式由str返回。
 * 参数：
 *      str：返回计算的结果。
 *************************************************************************************************/
QString TimeDate::getDate(bool year4Digits)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    QString format = d_ptr->getDateFormatStr(year4Digits);
    return currentDateTime.toString(format);
}

/**************************************************************************************************
 * 功能：获取当前的日期时间，以设定的日期格式由str返回。
 * 参数：
 *      str：返回计算的结果。
 *************************************************************************************************/
QString TimeDate::getDateTime(bool year4Bits, bool showSecond)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    QString dateFormat = d_ptr->getDateFormatStr(year4Bits);

    QString timeFormat = d_ptr->getTimeFormatStr(showSecond);

    return currentDateTime.toString(dateFormat + " " + timeFormat);
}

/**************************************************************************************************
 * 功能：计算出t表示的时间，以设定的日期格式由str返回。
 * 参数：
 *      t：需计算的时间。
 *      str：返回计算的结果。
 *************************************************************************************************/
QString TimeDate::getTime(unsigned t, bool showSecond)
{
    QString timeFormat = d_ptr->getTimeFormatStr(showSecond);
    return QDateTime::fromTime_t(t).toString(timeFormat);
}

/**************************************************************************************************
 * 功能：计算出t表示的日期，以设定的日期格式由str返回。
 * 参数：
 *      t：需计算的时间。
 *      str：返回计算的结果。
 *************************************************************************************************/
QString TimeDate::getDate(unsigned t, bool year4Digits)
{
    QDateTime currentDateTime = QDateTime::fromTime_t(t);

    QString format = d_ptr->getDateFormatStr(year4Digits);
    return currentDateTime.toString(format);
}

/**************************************************************************************************
 * 功能：计算出t表示的日期时间，以设定的日期格式由str返回。
 * 参数：
 *      t：需计算的时间。
 *      str：返回计算的结果。
 *************************************************************************************************/
QString TimeDate::getDateTime(unsigned t, bool year4Digits, bool showSecond)
{
    QDateTime currentDateTime = QDateTime::fromTime_t(t);

    QString format  = d_ptr->getDateFormatStr(year4Digits);

    QString timeFormat = d_ptr->getTimeFormatStr(showSecond);

    return currentDateTime.toString(format + " " + timeFormat);
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
unsigned int TimeDate::getTimeSeconds(unsigned time)
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
 * 功能：构造。
 *************************************************************************************************/
TimeDate::TimeDate()
    :d_ptr(new TimeDatePrivate())
{
}

/**************************************************************************************************
 * 功能：析构。
 *************************************************************************************************/
TimeDate *TimeDate::getInstance()
{
    if (instance == NULL)
    {
        instance = new TimeDate();
    }
    return instance;
}

void TimeDate::setDateFormat(const DateFormat &format)
{
    if (format >= DATE_FORMAT_NR)
    {
        return;
    }

    d_ptr->dateFormat = format;
}

DateFormat TimeDate::getDateFormat() const
{
    return d_ptr->dateFormat;
}

void TimeDate::setTimeFormat(const TimeFormat &format)
{
    if (format >= TIME_FORMAT_NR)
    {
        return;
    }

    d_ptr->timeFormat = format;
}

TimeFormat TimeDate::getTimeFormat() const
{
    return d_ptr->timeFormat;
}

TimeDate::~TimeDate()
{
    delete d_ptr;
}
