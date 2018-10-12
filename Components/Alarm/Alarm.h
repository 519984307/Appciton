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
#include "AlarmIndicator.h"
#include "SoundManager.h"
#include <QMultiMap>

class Alarm
{
public:
    struct AlarmInfo
    {
        int paramid;
        int alarmType;
        unsigned timestamp;
        bool isOneshot;
        char order;            // record the order of this alarm when many alarms happen in the same timestamp
    };

    static Alarm &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new Alarm();
        }
        return *_selfObj;
    }
    static Alarm *_selfObj;

    // 注册报警源。
    void addLimtSource(AlarmLimitIFace &alarmSource);
    void addOneShotSource(AlarmOneShotIFace &alarmSource);

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
    char getAlarmSourceStatus(const QString &sourceName, SubParamID id);

    // 主运行，需1秒调用一次。
    void mainRun(unsigned t);

    // 添加报警状态
    void addAlarmStatus(AlarmAudioStatus status);

    // get the alarm string from alarm source
    const char *getAlarmMessage(ParamID paramId, int alarmType, bool isOneshotAlarm);

    // get the current alarm info list
    QList<AlarmInfo> getCurrentPhyAlarmInfo();

    // check specific oneshot alarm is active
    bool getOneShotAlarmStatus(AlarmOneShotIFace *iface, int alarmId);

    // according to SubParamID get alarmLimitIFace
    AlarmLimitIFace *getAlarmLimitIFace(SubParamID id);

    // 析构。
    virtual ~Alarm();

    static QString getPhyAlarmMessage(ParamID paramId, int alarmType, bool isOneShot);

    /**
     * @brief setLatchLockSta 设置栓锁是否打开
     * @param status 栓锁打开状态
     */
    void setLatchLockSta(bool status);

    /**
     * @brief updateStandbyMode  更新待机模式
     * @param isEnable  true--更新为待机模式；false--更新为非待机模式
     */
    void updateStandbyMode(bool isEnable);

private:
    unsigned _timestamp;
    QMultiMap<ParamID, AlarmLimitIFace *> _limitSources;
    QMultiMap<ParamID, AlarmOneShotIFace *> _oneshotSources;
    QList<AlarmAudioStatus> _alarmStatusList;
    AlarmAudioStatus _curAlarmStatus;
    bool _isLatchLock;       // 栓锁状态
    bool _isInStandby;    // 是否处于待机模式中
    // 构造。
    Alarm();

    // 生成一个报警的ID，该ID再Alarm内部使用，用于唯一标识一个报警源。
    void _getAlarmID(AlarmParamIFace *alarmSource, int ID, QString &name);

    // 判断报警。
    void _handleLimitAlarm(AlarmLimitIFace *alarmSource, QList<ParamID> &alarmParam);
    void _handleOneShotAlarm(AlarmOneShotIFace *alarmSource);

    void _handleAlarm(void);
};
#define alertor (Alarm::construction())
#define deleteAlarm() (delete Alarm::_selfObj)
