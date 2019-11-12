/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/1/5
 **/


#pragma once
#include <QString>
#include "TimeDefine.h"

class TimeDatePrivate;
class TimeDate
{
public:
    /**
     * @brief getInstance get the time date instance
     * @return pointer to the instance
     */
    static TimeDate *getInstance();

    /**
     * @brief setDateFormat
     * @param format
     */
    void setDateFormat(const DateFormat &format);

    /**
     * @brief getDateFormat get the date format
     * @return the current date format
     */
    DateFormat getDateFormat() const;

    /**
     * @brief setTimeFormat set the time format
     * @param foramt
     */
    void setTimeFormat(const TimeFormat &format);

    /**
     * @brief getTimeFormat get the time formate
     * @return  the current time format
     */
    TimeFormat getTimeFormat() const;

    /**
     * @brief ~TimeDate destructor
     */
    virtual ~TimeDate();

    /**
     * @brief time get current timestamp
     */
    unsigned time(void);

    /**
     * @brief difftime claculate time difference
     * @param time2 end time
     * @param time1 start time
     * @return  the number of seconds elapsed between time2 and time1
     */
    double difftime(unsigned time2, unsigned time1);

    /**
     * @brief getTime get the current time string
     * @param showSecond whether show seconds
     * @return the time string
     */
    QString getTime(bool showSecond = false);

    /**
     * @brief getDate get the current date
     * @param year4Bits whether use 4 digits to show the year value
     * @return the date string
     */
    QString getDate(bool year4Digits = false);

    /**
     * @brief getDateTime get the current datetime string
     * @param year4Bits whether use 4 digits to represent the year value
     * @param showSecond whether show seconds
     * @return  the date string
     */
    QString getDateTime(bool year4Bits = false, bool showSecond = false);

    /**
     * @brief getTime get the time string of specific timestamp
     * @param the timestamp
     * @param showSecond whether show seconds
     * @return the time string
     */
    QString getTime(unsigned t, bool showSecond = false);

    /**
     * @brief getDate get the date string of specific timestamp
     * @param t the timestamp
     * @param year4Bits whether use 4 digits to represent year value
     * @return the date string
     */
    QString getDate(unsigned t, bool year4Digits = false);

    /**
     * @brief getDateTime get the datetime string of specific timestamp
     * @param t the timestamp
     * @param year4digits whether use 4 digits to represent year value
     * @param showSecond whether show second value
     * @return the datetime string
     */
    QString getDateTime(unsigned t, bool year4Digits = false, bool showSecond = false);

    /**
     * @brief getDateYear get the year value of specific timestamp
     * @param time the timestamp, 0 means current timestamp
     * @return  the year value
     */
    unsigned int getDateYear(unsigned time = 0);

    /**
     * @brief getDateMonth get the month value of specific timestamp
     * @param time the timestamp, 0 means current timestamp
     * @return  the month value
     */
    unsigned int getDateMonth(unsigned time = 0);

    /**
     * @brief getDateDay get the day value of specific timestamp
     * @param time the timestamp, 0 means current timestamp
     * @return the day value
     */
    unsigned int getDateDay(unsigned time = 0);

    /**
     * @brief getTimeHour get the hour value of specific timestamp
     * @param time the timesamp, 0 means current timestamp
     * @return  the hour value
     */
    unsigned int getTimeHour(unsigned time = 0);

    /**
     * @brief getTimeMinute get the minute value of specific timestamp
     * @param time the timesamp, 0 means current timestamp
     * @return  the minute value
     */
    unsigned int getTimeMinute(unsigned time = 0);

    /**
     * @brief getTimeSecond get the second value of specific timestamp
     * @param time the timesamp, 0 means current timestamp
     * @return  the second value
     */
    unsigned int getTimeSeconds(unsigned time = 0);

protected:
    TimeDate();
    static TimeDate *instance;

private:
    Q_DISABLE_COPY(TimeDate)
    TimeDatePrivate * const d_ptr;
};

#define timeDate (TimeDate::getInstance())
