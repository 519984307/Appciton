/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/17
 **/

#pragma once
#include "BatteryMessage.h"
#include "BatteryBarWidget.h"
#include "SystemBoardProvider.h"

#define CFG_CURRENT_DRAW (1080)     // (1080)
#define LOW_BATTERY (1500)  // (1200)
#define LOW_BATTERY_EXIT (1510) // (1210)
#define REPLACE_BATTERY (500)   // (350)
#define REPLACE_BATTERY_EXIT (510)  // (360)
#define SHUTDOWN_BATTERY (210)

#define TIME_CHANGE 120



class IMessageBox;
class PowerManager: public QObject
{
    Q_OBJECT
public:
    static PowerManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PowerManager();
        }
        return *_selfObj;
    }
    static PowerManager *_selfObj;

    ~PowerManager();

public:
    // 主运行入口。
    void run(void);

    // 返回是否有电池接入。
    bool hasBattery(void);

    BatteryIconStatus getStatus(void)
    {
        return batteryBarWidget.getStatus();
    }

    // 是否处于defib禁用
    bool isdefibDisable(void) const
    {
        return _defibDisable;
    }

    // 电池信息。
    BatteryMessage batteryMessage;

private slots:
    void _timerOut();
    void _powerOff();

private:
    PowerManager();

    QTimer *_timer;

    // 更新电池报警信息。
    void _updateBatAlarm(BatOneShotType type, bool isOccure);

    // 关机报警。
    void _shutdownWarn(void);

    void _initBatteryData(void);

    void _moniterRun(void);
    void _configRun(void);
    void _batteryIconNormal(void);

    // 电源线插拔时作出行为
    void _powerLineChange(const PowerSuplyType &lastPowerType, const PowerSuplyType &currentPowerType,
                          const bool &powerTypeChange);

    // 根据报警决定是否修改电池图标
    void _batteryChangeByAlarm(void);

    bool _replaceBattery;           // 更换电池
    bool _lowBattery;               // 低电量
    bool _shutBattery;              // 关机电量
    bool _defibDisable;             // 禁用defib
    bool _isChargePeriod;           // 充电周期（充电开始，到放电后一段时间）
    int _chargrdDelayTime;
    int _timeValue;                 // 剩余运行时间
    PowerSuplyType _powerType;      // 电源类型

    IMessageBox *_shutdownMsgBox;
};

#define powerManager (PowerManager::construction())
#define deletePowerManager() (delete PowerManager::_selfObj)
