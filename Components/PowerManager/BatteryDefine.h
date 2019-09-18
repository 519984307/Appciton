/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/11/12
 **/


#pragma once
#include <QString>

#define LOG_FILE_PATH "/usr/local/nPM/log/"
#define LOG_FILE "/usr/local/nPM/log/log.xml"

/**************************************************************************************************
 * OneShot事件定义，
 *************************************************************************************************/
enum BatOneShotType
{
    BAT_ONESHOT_REPLACE_BATTERY,
    BAT_ONESHOT_LOW_BATTERY,
    BAT_ONESHOT_NO_BATTERY,
    BAT_ONESHOT_BATTERY_ERROR,
    BAT_ONESHOT_BATTERY_COMM_FAULT,
    BAT_ONESHOT_BATTERY_CALIBRATION,
    BAT_ONESHOT_NR,
};

enum BatStatus
{
    BAT_STATUS_NONE = -1,               // 未知状态
    BAT_STATUS_FULLY_CHARGED,           // 满电状态
    BAT_STATUS_ABOVE_3_HOURS,           // 超过3小时状态
    BAT_STATUS_ABOVE_2_HOURS,           // 超过2小时状态
    BAT_STATUS_ABOVE_1_HOURS,           // 超过1小时状态
    BAT_STATUS_BELOW_1_HOURS,           // 低于1小时状态
    BAT_STATUS_LOW_BATTERY,             // 低电量状态
    BAT_STATUS_SHUT_DOWN,               // 电量不足关机状态
    BAT_STATUS_NO_BATTERY,              // 无电池状态
    BAT_STATUS_ERROR,                   // 电池错误
    BAT_STATUS_COMM_FAULT,              // 电池通信错误
    BAT_STATUS_NR,
};

enum BatteryPowerLevel
{
    BAT_VOLUME_NONE = -1,
    BAT_VOLUME_0,
    BAT_VOLUME_1,
    BAT_VOLUME_2,
    BAT_VOLUME_3,
    BAT_VOLUME_4,
    BAT_VOLUME_5
};

// 每个等级对应的ADC值
enum BatteryLevelAdc
{
    BAT_LEVEL_0 = 2749,
    BAT_LEVEL_1 = 2921,
    BAT_LEVEL_2 = 3093,
    BAT_LEVEL_3 = 3265,
    BAT_LEVEL_4 = 3437,
    BAT_LEVEL_5 = 3609
};

// 电池剩余时间
enum BatteryRemainTime
{
    BAT_REMAIN_TIME_NULL = -2,
    BAT_REMAIN_TIME_LOW,
    BAT_REMAIN_TIME_1_HOUR,
    BAT_REMAIN_TIME_2_HOUR,
    BAT_REMAIN_TIME_5_HOUR,
    BAT_REMAIN_TIME_7_HOUR,
    BAT_REMAIN_TIME_8_HOUR
};

struct FixedBatMessage
{
    unsigned char remainingCapacityAlarm[2];
    unsigned char remainingTimeAlarm[2];
    unsigned char batteryMode[2];
    unsigned char atRate[2];
    unsigned char atRateTimeToFull[2];
    unsigned char atRateTimeToempty[2];
    unsigned char atRateOK[2];
    unsigned char maxError[2];
    unsigned char fullChargeCapacity[2];
    unsigned char cycleCount[2];
    unsigned char designCapacity[2];
    unsigned char designVoltage[2];
    unsigned char specificationInfo[2];
    unsigned char manufactureData[2];
    unsigned char softwareVersion[4];
    unsigned char packStatusAndConf[2];
    unsigned char hardwareVersion;
    unsigned char batteryType;
    unsigned char numOfFullDepthOfDischarge[2];
    unsigned char numOfDODCycle0_10[2];
    unsigned char numOfDODCycle11_25[2];
    unsigned char numOfDODCycle26_50[2];
    unsigned char numOfDODCycle51_75[2];
    unsigned char numOfDODCycle76_95[2];
    unsigned char minLifetimePackVoltage[2];
    unsigned char maxLifetimePackVoltage[2];
    unsigned char minLifetimePackTempDurDis[2];
    unsigned char maxLifetimePackTempDurDis[2];
    unsigned char minLifetimePackTempDurCharge[2];
    unsigned char maxLifetimePackTempDurCharge[2];
    unsigned char maxLifetimeCurrentDurCharge[2];
    unsigned char maxLifetimeCurrentDurDis[2];
    unsigned char totalDisAhThroughput[2];
    unsigned char totalChargeAhThroughput[2];
    unsigned char totalnumOfChargeCycles[2];
    unsigned char lossOfPowerToPCBDet;
    unsigned char watchdogResetDet;
    unsigned char gasGaugeErrorDet;
    unsigned char currentOutOfRangeDet;
    unsigned char packOverVoltageDet;
    unsigned char packUnderVoltageDet;
    unsigned char packOverTempDetDurCharge;
    unsigned char packUnderTempDet;
    unsigned char vccSupplyOverUnderDet;
    unsigned char clkFreOutOfSpecification;
    unsigned char programCRCErrDet;
    unsigned char cpuErrDet;
    unsigned char absBatCapBelow60;
    unsigned char serialFlashErrDet;
    unsigned char invalidCRCOnFlashDataDet;
    unsigned char microRamErrDet;
    unsigned char packOverTempDetDurDis;
    unsigned char numOfA2DSelfTestErr;
    unsigned char numOfPackDisableErr;
    unsigned char numOfInitRecalRequiredErr;
    unsigned char numOfConfInvalidErr;
    unsigned char numOfChargeProtocolErr;
    unsigned char numOfGasGaugeCommErr;
    unsigned char numOfErrRB4SafeInterrupts;
    unsigned char numOfButtonStuckErr;
    unsigned char numOfSoftwareResets;
    unsigned char numOfStackUnderflowResets;
    unsigned char numOfStackOverflowResets;
    unsigned char minutesOfPacing[3];
};

struct PeriodBatMessage
{
    void init()
    {
        voltage[0] = 0x00;
        voltage[1] = 0x00;
        relStateOfCharge[0] = 0x00;
        relStateOfCharge[1] = 0x00;
        remainingCapacity[0] = 0xff;
        remainingCapacity[1] = 0xff;
        runTimeOfEmpty[0] = 0xff;
        runTimeOfEmpty[1] = 0xff;
        averageTimeToEmpty[0] = 0xff;
        averageTimeToEmpty[1] = 0xff;
        mode[0] = 0x00;
        mode[1] = 0x00;
    }
    unsigned char temperature[2];
    unsigned char voltage[2];
    unsigned char current[2];
    unsigned char averageCurrent[2];
    unsigned char relStateOfCharge[2];
    unsigned char AbsStateOfCharge[2];
    unsigned char remainingCapacity[2];
    unsigned char runTimeOfEmpty[2];
    unsigned char averageTimeToEmpty[2];
    unsigned char averageTimeToFull[2];
    unsigned char status[2];
    unsigned char mode[2];
    unsigned char vcell[8];
    unsigned char drainCyclesRemaining[1];
    unsigned char elapsedTime[4];
    unsigned char error[4];
};

// 电池信息结构体
struct BatMessage
{
    BatMessage()
    {
        memset(this, 0, sizeof(FixedBatMessage) + sizeof(PeriodBatMessage));
    }

    void init()
    {
        periodBatMessage.init();
        voltageADC[0] = 0xff;
        voltageADC[1] = 0xff;
    }

    FixedBatMessage fixedBatMessage;
    PeriodBatMessage periodBatMessage;
    unsigned char voltageADC[2];
    QString serialNumber;
    QString manufacturerName;
    QString deviceName;
    QString deviceChemistry;
};

// 可设置的电池参数结构体
struct CanSetBatParam
{
    CanSetBatParam()
    {
        memset(this, 0xff, sizeof(CanSetBatParam));
    }
    unsigned char serialNumber[11];
    unsigned char remainingCapacityAlarm[2];
    unsigned char remainingTimeAlarm[2];
    unsigned char atRate[2];
};

// 将定义的枚举转换成符号。
class BatterySymbol
{
public:
    static const char *convert(BatOneShotType index)
    {
        static const char *symbol[BAT_ONESHOT_NR] =
        {
            "ReplaceBattery",
            "LowBattery",
            "BatteryDisconnected",
            "BatteryError",
            "BatteryCommFault",
            "BatteryCalibrationRequired"
        };
        return symbol[index];
    }
};
