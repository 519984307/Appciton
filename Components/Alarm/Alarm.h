/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/3
 **/

#pragma once
#include <QString>
#include "AlarmParamIFace.h"
#include "EventStorageManagerInterface.h"
#include "AlarmInterface.h"
#include <QMultiMap>

class Alarm : public AlarmInterface
{
public:
    struct AlarmInfo
    {
        int paramid;
        int alarmType;
        unsigned timestamp;
        bool isOneshot;
        char order;            // record the order of this alarm when many alarms happen in the same timestamp

        bool operator==(const AlarmInfo &info)
        {
            if (info.paramid == paramid && info.alarmType == alarmType && info.timestamp == timestamp
                    && info.isOneshot == isOneshot)
            {
                return true;
            }
            return false;
        }
    };

    static Alarm &getInstance(void);
    void clear();

    // 注册报警源。
    void addLimtSource(AlarmLimitIFace *alarmSource);
    void addOneShotSource(AlarmOneShotIFace *alarmSource);

    // 设置静音键状态。
    void updateMuteKeyStatus(bool isPressed);

    /**
     * @brief updateResetKeyStatus update the status of the reset key
     * @param isPressed key is press or not
     */
    void updateResetKeyStatus(bool isPressed);

    // 获取报警源参数的值。
    void getAlarmSourceValue(qint32 *data, int len);

    // 获取报警源参数的报警状态。
    char getAlarmSourceStatus(const QString &sourceName, SubParamID id, bool ignoreLatched = true);

    // 主运行，需1秒调用一次。
    void mainRun(unsigned t);

    // 添加报警状态
    void addAlarmStatus(AlarmStatus status);
    int getAlarmStatusCount();

    // get the alarm string from alarm source
    const char *getAlarmMessage(ParamID paramId, int alarmType, bool isOneshotAlarm);

    // get the current alarm info list
    QList<AlarmInfo> getCurrentPhyAlarmInfo();

    // check specific oneshot alarm is active
    bool getOneShotAlarmStatus(AlarmOneShotIFace *iface, int alarmId);

    // according to SubParamID get alarmLimitIFace
    AlarmLimitIFace *getAlarmLimitIFace(SubParamID id);

    // according to SubParamID get AlarmOneShotIFace
    AlarmOneShotIFace *getAlarmOneShotIFace(SubParamID id);

    // 析构。
    virtual ~Alarm();

    static QString getPhyAlarmMessage(ParamID paramId, int alarmType, bool isOneShot);

    /**
     * @brief setLatchLockSta 设置栓锁是否打开
     * @param status 栓锁打开状态
     */
    void setLatchLockSta(bool status);

    /**
     * @brief removeAllLimitAlarm 移除生理报警的跟踪对象
     */
    void removeAllLimitAlarm();

private:
    unsigned _timestamp;
    QMultiMap<ParamID, AlarmLimitIFace *> _limitSources;
    QMultiMap<ParamID, AlarmOneShotIFace *> _oneshotSources;
    QList<AlarmStatus> _alarmStatusList;
    AlarmStatus _curAlarmStatus;
    bool _isLatchLock;       // 栓锁状态
    // 构造。
    Alarm();

    // 生成一个报警的ID，该ID再Alarm内部使用，用于唯一标识一个报警源。
    void _getAlarmID(AlarmParamIFace *alarmSource, int ID, QString &name);

    /**
     * @brief _handleLimitAlarm 处理超限报警
     * @param alarmSource   报警源
     * @param alarmParam    用于获取第一次发生超限报警的参数ID
     */
    void _handleLimitAlarm(AlarmLimitIFace *alarmSource, QList<ParamID> &alarmParam);

    /**
     * @brief _handleOneShotAlarm 处理Oneshot报警
     * @param alarmSource   报警源
     * @param alarmParam    用于获取第一次发生PhyOneshot报警的参数ID
     */
    void _handleOneShotAlarm(AlarmOneShotIFace *alarmSource, QList<ParamID> &alarmParam);

    void _handleAlarm(void);

    bool _alarmLightOnAlarmReset;       // 报警复位时的报警灯

    struct EventInfo
    {
        EventInfo()
            : alarmSource(NULL),
              waveId(-1)
        {}
        AlarmParamIFace *alarmSource;
        int waveId;
        AlarmInfoSegment infoSegment;
    };
    QList<EventInfo> eventList;
};
#define alertor (Alarm::getInstance())
