/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/16
 **/

#include "TestBatteryTime.h"
#include <QFile>
#include <QDir>
#include "Framework/Config/XmlParser.h"
#include <QTime>

class TestBatteryTimePrivate
{
public:
    TestBatteryTimePrivate()
        : status(false)
    {}
    ~TestBatteryTimePrivate(){}
    bool status;
};

TestBatteryTime &TestBatteryTime::getInstance()
{
    static TestBatteryTime *instance = NULL;
    if (instance == NULL)
    {
        instance = new TestBatteryTime;
    }
    return *instance;
}

TestBatteryTime::~TestBatteryTime()
{
    delete d_ptr;
}

void TestBatteryTime::Record(const BatteryPowerLevel &level, const unsigned int &ad, const QTime &time)
{
    XmlParser xmlParser;
    if (d_ptr->status && xmlParser.open(QString(LOG_FILE)))
    {
        QString nodeName = QString("BAT_%1").arg(QString::number(static_cast<int>(level)));
        QString oldAD;
        xmlParser.getValue(nodeName + QString("|AD"), oldAD);
        if (!oldAD.isEmpty() && ad != 0 && time.isValid())
        {
            return;
        }
        if (ad == 0)
        {
            xmlParser.setValue(nodeName + QString("|AD"), QString());
        }
        else
        {
            xmlParser.setValue(nodeName + QString("|AD"), QString::number(ad));
        }
        xmlParser.setValue(nodeName + QString("|Time"), time.toString("hh:mm:ss"));
        xmlParser.saveToFile();
    }
}

void TestBatteryTime::open()
{
    d_ptr->status = true;
}

TestBatteryTime::TestBatteryTime()
    : d_ptr(new TestBatteryTimePrivate)
{
}
