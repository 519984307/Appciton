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
#include "TimeDate.h"
class TestTimeDate : public QObject
{
    Q_OBJECT

public:
    TestTimeDate();
private Q_SLOTS:
    void testTime_data();
    void testTime();
    void testDifftime_data();
    void testDifftime();
    void testGetTime_data();
    void testGetTime();
    void testGetDate_data();
    void testGetDate();
    void testGetTimeStr_data();
    void testGetTimeStr();
    void testGetDateStr_data();
    void testGetDateStr();
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
    void testGetTimeMsec_data();
    void testGetTimeMsec();
private:
};

#endif  //  TESTTIMEDATE_H
