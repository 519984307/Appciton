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
#include "XmlParser.h"
#include <QTime>

#define LOG_FILE_PATH "/usr/local/nPM/log/"
#define LOG_FILE "/usr/local/nPM/log/log.xml"

class TestBatteryTimePrivate
{
public:
    TestBatteryTimePrivate(){}
    ~TestBatteryTimePrivate(){}
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
    if (xmlParser.open(QString(LOG_FILE)))
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

TestBatteryTime::TestBatteryTime()
    : d_ptr(new TestBatteryTimePrivate)
{
    QDir logDir(QString(LOG_FILE_PATH));
    if (!logDir.exists())
    {
        // 不存在就新建
        logDir.mkpath(QString(LOG_FILE_PATH));
    }
    QFile logFile(QString(LOG_FILE));
    if (!logFile.exists())
    {
        logFile.open(QIODevice::ReadWrite);
        logFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        logFile.write("\n<BatteryInfo>");
        for (int i = BAT_VOLUME_0; i <= BAT_VOLUME_5 ; i++)
        {
            // add bat_i
            QString nodeHead = QString("\n\t<BAT_%1>").arg(QString::number(i));
            QString nodeTail = QString("\n\t</BAT_%1>").arg(QString::number(i));
            logFile.write(nodeHead.toLocal8Bit());
            logFile.write(nodeTail.toLocal8Bit());
        }
        logFile.write("\n</BatteryInfo>");
        logFile.close();
    }
    XmlParser xmlParser;
    if (xmlParser.open(QString(LOG_FILE)))
    {
        for (int i = BAT_VOLUME_0; i <= BAT_VOLUME_5; i++)
        {
            QString nodeName = QString("BAT_%1").arg(QString::number(i));
            if (!xmlParser.hasNode(QString("%1|AD").arg(nodeName)))
            {
                xmlParser.addNode(nodeName, "AD");
            }
            if (!xmlParser.hasNode(QString("%1|Time").arg(nodeName)))
            {
                xmlParser.addNode(nodeName, "Time");
            }
        }
        xmlParser.saveToFile();
    }
}
