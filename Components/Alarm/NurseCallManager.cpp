/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/4/2
 **/

#include "NurseCallManager.h"
#include "stddef.h"
#include "IConfig.h"
#include <QFile>
#include <unistd.h>
#include <QTimerEvent>

class NurseCallManagerPrivate
{
public:
    enum SignalType
    {
        SIGNAL_TYPE_CONTINUOUS,
        SIGNAL_TYPE_PULSE
    };

    enum ContactType
    {
        NORMALLY_CLOSED,
        NORMALLY_OPEN
    };
    NurseCallManagerPrivate()
        : callSta(false), callFile("/sys/class/pmos/nurse_call"),
          pulseTimerID(-1)
    {
        if (!callFile.open(QIODevice::ReadWrite))
        {
            qDebug() << "fail to open nurse_call file";
        }
    }

    /**
     * @brief getSignalType 获取信号类型
     * @return
     */
    SignalType getSignalType();

    /**
     * @brief getContactType 获取电平触发方式
     * @return
     */
    ContactType getContactType();

    /**
     * @brief writeNurseCallSta 护士呼叫文件写入电平值
     * @param sta
     */
    void writeNurseCallSta(bool sta);

    bool callSta;
    QFile callFile;
    int pulseTimerID;
};

NurseCallManager &NurseCallManager::getInstance()
{
    static NurseCallManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new NurseCallManager();
        NurseCallManagerInterface *old = NurseCallManagerInterface::registerNurseCallManagerInterface(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

bool NurseCallManager::getAlarmLevelHigh()
{
    int value = 0;
    systemConfig.getNumValue("Others|AlarmLevel|High", value);
    return value;
}

bool NurseCallManager::getAlarmLevelMed()
{
    int value = 0;
    systemConfig.getNumValue("Others|AlarmLevel|Med", value);
    return value;
}

bool NurseCallManager::getAlarmLevelLow()
{
    int value = 0;
    systemConfig.getNumValue("Others|AlarmLevel|Low", value);
    return value;
}

bool NurseCallManager::getAlarmTypeTech()
{
    int value = 0;
    systemConfig.getNumValue("Others|AlarmType|Technology", value);
    return value;
}

bool NurseCallManager::getAlarmTypePhy()
{
    int value = 0;
    systemConfig.getNumValue("Others|AlarmType|Physiology", value);
    return value;
}

void NurseCallManager::callNurse(bool signalSta)
{
    NurseCallManagerPrivate::SignalType signalType = d_ptr->getSignalType();
    if (signalType == NurseCallManagerPrivate::SIGNAL_TYPE_CONTINUOUS)
    {
        if (signalSta != d_ptr->callSta)
        {
            d_ptr->writeNurseCallSta(signalSta);
        }
    }
    else
    {
        if (!d_ptr->callSta && signalSta)
        {
            d_ptr->writeNurseCallSta(signalSta);
            d_ptr->pulseTimerID = startTimer(1000);
        }
    }
}

void NurseCallManager::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == d_ptr->pulseTimerID)
    {
        d_ptr->writeNurseCallSta(0);
        killTimer(d_ptr->pulseTimerID);
        d_ptr->pulseTimerID = -1;
    }
}

NurseCallManager::NurseCallManager()
    : QObject(),
      NurseCallManagerInterface(),
      d_ptr(new NurseCallManagerPrivate())
{
}

NurseCallManager::~NurseCallManager()
{
    delete d_ptr;
}

NurseCallManagerPrivate::SignalType NurseCallManagerPrivate::getSignalType()
{
    int value = 0;
    systemConfig.getNumValue("Others|SignalType", value);
    return static_cast<SignalType>(value);
}

NurseCallManagerPrivate::ContactType NurseCallManagerPrivate::getContactType()
{
    int value = 0;
    systemConfig.getNumValue("Others|TriggerMode", value);
    return static_cast<ContactType>(value);
}

void NurseCallManagerPrivate::writeNurseCallSta(bool sta)
{
    callSta = sta;
    if (!callFile.exists())
    {
        return;
    }

    QByteArray data;
    data.resize(1);
    if (getContactType() == NORMALLY_CLOSED)
    {
        data[0] = sta;
    }
    else
    {
        data[0] = !sta;
    }
    callFile.write(data);

    if (callFile.size() > 10 * 1024 * 1024)
    {
        // 超过10m清空文件并重新打开
        callFile.close();
        callFile.open(QIODevice::Truncate);
        callFile.close();
        callFile.open(QIODevice::WriteOnly);
    }
}
