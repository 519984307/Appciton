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
#include <QDebug>


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
        : callSta(false),
          pulseTimerID(-1)
    {
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
     * @brief getAlarmLevelType 获取指定报警类型和报警等级是否呼叫护士
     * @param type
     * @param prio
     * @return
     */
    bool getAlarmLevelType(AlarmType type, AlarmPriority prio);

    /**
     * @brief writeNurseCallSta 护士呼叫文件写入电平值
     * @param sta
     */
    void writeNurseCallSta(bool sta);

    int callSta;                   // 当前信号状态
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

void NurseCallManager::callNurse(AlarmType type, AlarmPriority prio, bool alarmSta)
{
    if (d_ptr->getSignalType() == NurseCallManagerPrivate::SIGNAL_TYPE_CONTINUOUS
            && d_ptr->getAlarmLevelType(type, prio))
    {
        // 每种报警类型和报警优先级是否报警对应一个位
        int pos = type * 3 + prio;
        bool posAlarmSta = d_ptr->callSta & (1 << pos);  // 当前位对应的报警状态
        if (posAlarmSta != alarmSta)  // 护士呼叫状态与当前位对应不相等进行更新护士呼叫状态
        {
            bool prvSta = d_ptr->callSta;
            if (alarmSta)
            {
                d_ptr->callSta |= 1 << pos;
            }
            else
            {
                d_ptr->callSta &= ~(1 << pos);
            }
            bool curSta = d_ptr->callSta;
            if (prvSta != curSta)                       // 护士呼叫状态发生0和1的变化时才进行写入操作
            {
                d_ptr->writeNurseCallSta(curSta);
            }
        }
    }
}

void NurseCallManager::callNurse(AlarmType type, AlarmPriority prio)
{
    if (d_ptr->getSignalType() == NurseCallManagerPrivate::SIGNAL_TYPE_PULSE
            && d_ptr->getAlarmLevelType(type, prio))
    {
        if (!d_ptr->callSta)
        {
            d_ptr->callSta = 1;
            d_ptr->writeNurseCallSta(true);
            d_ptr->pulseTimerID = startTimer(1000);
        }
    }
}

void NurseCallManager::upDateCallSta()
{
    d_ptr->callSta = 0;
}

void NurseCallManager::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == d_ptr->pulseTimerID)
    {
        d_ptr->callSta = 0;
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

bool NurseCallManagerPrivate::getAlarmLevelType(AlarmType type, AlarmPriority prio)
{
    int typeValue = 0;
    int prioValue = 0;
    if (type == ALARM_TYPE_TECH)
    {
        systemConfig.getNumValue("Others|AlarmType|Technology", typeValue);
    }
    else
    {
        systemConfig.getNumValue("Others|AlarmType|Physiology", typeValue);
    }

    if (prio == ALARM_PRIO_HIGH)
    {
        systemConfig.getNumValue("Others|AlarmLevel|High", prioValue);
    }
    else if (prio == ALARM_PRIO_MED)
    {
        systemConfig.getNumValue("Others|AlarmLevel|Med", prioValue);
    }
    else if (prio == ALARM_PRIO_LOW)
    {
        systemConfig.getNumValue("Others|AlarmLevel|Low", prioValue);
    }

    return prioValue && typeValue;
}

void NurseCallManagerPrivate::writeNurseCallSta(bool sta)
{
    QString data;
    if (getContactType() == NORMALLY_CLOSED)
    {
        int intSta = sta;
        data = QString::number(intSta);
    }
    else
    {
        int intSta = !sta;
        data = QString::number(intSta);
    }
    QFile callFile("/sys/class/pmos/nurse_call");
    if (!callFile.open(QIODevice::ReadWrite))
    {
        qDebug() << "fail to open nurse_call file";
        return;
    }
    callFile.write(data.toAscii());
    callFile.close();
}
