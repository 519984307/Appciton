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

class TimeDate
{
public:
    static TimeDate &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TimeDate();
        }
        return *_selfObj;
    }
    static TimeDate *_selfObj;

    // 析构。
    virtual ~TimeDate();

    // 与标准C库中的time()函数功能一致。
    unsigned time(void);

    // 返回两个time_t型变量之间的时间间隔。
    double difftime(unsigned time2, unsigned time1);

    // 获取时间/日期。
    void getTime(QString &str, bool showSecond = false);
    void getDate(QString &str, bool year4Bits = false);
    void getDateTime(QString &str, bool year4Bits = false, bool showSecond = false);

    // 用指定的time值计算出时间/日期。
    void getTime(unsigned t, QString &str, bool showSecond = false);
    void getDate(unsigned t, QString &str, bool year4Bits = false);
    void getDateTime(unsigned t, QString &str, bool year4Bits = false, bool showSecond = false);

    //获取具体的年，月，日，时，分，秒，毫秒
    unsigned int getDateYear(unsigned time = 0);
    unsigned int getDateMonth(unsigned time = 0);
    unsigned int getDateDay(unsigned time = 0);
    unsigned int getTimeHour(unsigned time = 0);
    unsigned int getTimeMinute(unsigned time = 0);
    unsigned int getTimeSenonds(unsigned time = 0);
    unsigned int getTimeMsec(void);

    /**
     * @brief getMaxDay
     * @param year
     * @param month
     * @return
     */
    int getMaxDay(int year, int month);

private:
    TimeDate();
};
#define timeDate (TimeDate::construction())
#define deleteTimeDate() (delete TimeDate::_selfObj)
