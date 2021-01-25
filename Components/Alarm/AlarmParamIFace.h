/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/19
 **/

#pragma once
#include "AlarmDefine.h"
#include "ParamDefine.h"
#include <stdlib.h>
#include <QMap>
#include <QLinkedList>

class AlarmParamIFace
{
public:
    // 报警源的名字。
    virtual QString getAlarmSourceName(void) = 0;

    // 获取对应的参数ID。
    virtual ParamID getParamID(void) = 0;

    // 报警源的个数。
    virtual int getAlarmSourceNR(void) = 0;

    // 获取报警对应的波形ID，该波形将被存储。
    virtual WaveformID getWaveformID(int id) = 0;

    // 生理参数报警级别。
    virtual AlarmPriority getAlarmPriority(int id) = 0;

    // 生理报警使能
    virtual bool isAlarmEnable(int id) = 0;

    // 获取id对应的子参数ID。
    virtual SubParamID getSubParamID(int id) = 0;

    // 将报警ID转换成字串。
    virtual const char *toString(int id) = 0;

    /**
     * @brief getAlarmDelay get the alarm audio delay time
     * @return  alarm audio delay time
     */
    virtual int getAlarmDelay(int /*id*/) { return 0;}
};

class AlarmLimitIFace: public AlarmParamIFace
{
public:
    // 获取指定的生理参数测量数据。
    virtual int getValue(int id) = 0;

    // 生理参数报警使能。
    virtual bool isAlarmEnable(int id) = 0;

    // 生理参数报警上限。
    virtual int getUpper(int id) = 0;

    // 生理参数报警下限。
    virtual int getLower(int id) = 0;

    // 报警参数值比较
    virtual int getCompare(int value, int id) = 0;

    //产生报警通知
    virtual void notifyAlarm(int /*id*/, bool /*isAlarm*/) { }

    // 获取id对应的子参数ID。
    virtual SubParamID getSubParamID(int id) = 0;

    AlarmLimitIFace()
    {
    }

    virtual ~AlarmLimitIFace()
    {
    }
};

// OneShot报警接口。
// 说明：参数对象在实现该接口时需要处理Oneshot报警发生后立即恢复正常的情况，由于报警部件使用的
// 是拉数据方式，所以报警取数据时有可能会漏掉该报警。因此，具体实现该接口时不能丢失这种情况的报警，
// 否则就是一种严重错误。具体实现可以参考ECG部分的实现。
class AlarmOneShotIFace: public AlarmParamIFace
{
public:
    // 该报警是否为生命报警，技术报警和生理/命报警分开存放。
    virtual AlarmType getAlarmType(int id)
    {
        Q_UNUSED(id)
        return ALARM_TYPE_TECH;
    }

    // 生理参数报警使能。
    virtual bool isAlarmEnable(int /*id*/) {return true;}

    AlarmOneShotIFace()
    {
    }

    virtual ~AlarmOneShotIFace()
    {
    }

public:
    // 清除所有的报警。
    void clear(void);

    /**
     * @brief clear  清除除了list中所有其他报警
     * @param ID
     */
    void clearRestOfAlarm(QList<int> ID);

    virtual void setOneShotAlarm(int id, bool newStatus);

    // 报警条件是否满足，满足则返回true。
    virtual bool isAlarmed(int id);

    // newest alarm status
    virtual bool newestAlarmStatus(int id);

    //产生报警通知
    virtual void notifyAlarm(int /*id*/, bool /*isAlarm*/) { }

    // latch后是否不在显示报警信息
    virtual bool isRemoveAfterLatch(int /*id*/) {return false;}

    // 报警确认后是否移除灯光
    virtual bool isRemoveLightAfterConfirm(int) {return true;}

    // 是否需要删除报警
    virtual bool isNeedRemove(int /*id*/) {return false;}

    // 获取id对应的子参数ID。
    virtual SubParamID getSubParamID(int) {return SUB_PARAM_NONE;}

private:
    struct OneShotAlarmDesc
    {
        explicit OneShotAlarmDesc(bool alarm)
        {
            isHandled = false;
            alarmList.clear();
            alarmList.append(alarm);
        }

        void reset(void)
        {
            isHandled = false;
            alarmList.clear();
            alarmList.append(false);
        }

        bool isHandled;
        QList<bool> alarmList;
    };

    typedef QMap<int, OneShotAlarmDesc> OneShotMap;
    OneShotMap _oneShotAlarms;
};
