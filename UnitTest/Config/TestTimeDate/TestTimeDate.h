/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <Fangtongzhou@blmed.cn>, 2019/3/5
 **/
#ifndef TESTTIMEDATE_H
#define TESTTIMEDATE_H

#include <QString>
#include <QtTest>
#include "Framework/TimeDate/TimeDate.h"

class TestTimeDate : public QObject, public TimeDate
{
    Q_OBJECT
public:
    TestTimeDate();

private Q_SLOTS:
    void init();
    void cleanup();
    void testGetDateFormat();
    void testSetDateFormat();
    void testGetTimeFormat();
    void testSetTimeformat();
    void testTime();
    void testDifftime_data();
    void testDifftime();
    void testGetTime_data();
    void testGetTime();
    void testGetDate_data();
    void testGetDate();
    void testGetDateTime_data();
    void testGetDateTime();
    void testGetTimeStr_data();
    void testGetTimeStr();
    void testGetDateStr_data();
    void testGetDateStr();
    void testGetDateTimeStr_data();
    void testGetDateTimeStr();
    void testGetDateYear_data();
    void testGetDateYear();
    void testGetDateMonth_data();
    void testGetDateMonth();
    void testGetDateDay_data();
    void testGetDateDay();
    void testGetTimeHour_data();
    void testGetTimeHour();
    void testGetTimeMinute_data();
    void testGetTimeMinute();
    void testGetTimeSecond_data();
    void testGetTimeSecond();
private:
};

Q_DECLARE_METATYPE(TimeFormat)
Q_DECLARE_METATYPE(DateFormat)

#endif  //  TESTTIMEDATE_H
