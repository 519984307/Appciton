/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by tongzhou fang  fangtongzhou@blmed.cn, 2019/3/20
 **/

#include <QString>
#include <QtTest>

class TestNIBPMonitorStopStatePrivate;
class TestNIBPMonitorStopState : public QObject
{
    Q_OBJECT

public:
    TestNIBPMonitorStopState();
    ~TestNIBPMonitorStopState();

private Q_SLOTS:
    void testEnter_data();
    void testEnter();
    void testHandleNIBPEvent_data();
    void testHandleNIBPEvent();
private:
    TestNIBPMonitorStopStatePrivate* const d_ptr;
};
