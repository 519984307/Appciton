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
    void testDifftime();
    void testGetTime();
    void testGetDate();
    void testGetTimeStr();
    void testGetDateStr();
    void testGetDateYear();
    void testGetDateMonth();
    void testGetDateDay();
    void testGetTimeHour();
    void testGetTimeMinute();
    void testGetTimeSecond();
    void testGetTimeMsec();
private:
};

#endif  //  TESTTIMEDATE_H
