#pragma once
#include <QString>
#include "BatteryDefine.h"

class BatteryMessage
{
public:

    BatteryMessage();

    ~BatteryMessage();
public:
    // 更新电池数据。
    void updateFixedBatMessage(unsigned char *data);
    void updatePeriodicBatMessage(unsigned char *data);
    void updatePeriodicBatValue(unsigned char *data);

    // 设置电池的参数
    void setBatParam(CanSetBatParam batParam);

    // 电池信息初始化
    void setInit(void);

    // 获取电池温度
    unsigned int getTemperature(void);

    // 获取电池电压
    unsigned int getVoltage(void);

    // 获取电池ADC采集电压
    unsigned int getVoltageADC(void);

    // 获取电池电流
    short getCurrent(void);

    // 获取每分钟平均电流
    short getAverageCurrent(void);

    // 获取相对电荷状态，即相对电池剩余百分比
    unsigned int getRelativeStateOfCharge(void);

    // 获取绝对电荷状态，即绝对电池剩余百分比
    unsigned int getAbsoluteStateOfCharge(void);

    // 获取剩余电池容量
    unsigned int getRemainingCapacity(void);

    // 获取电池剩余运行时间
    unsigned int getRunTimeToEmpty(void);

    // 获取电池平均剩余运行时间
    unsigned int getAverageTimeToEmpty(void);

    // 获取电池平均充满时间
    unsigned int getAverageTimeToFull(void);

    // 获取电池状态信息
    unsigned char *getStatus(void);

    // 获取电池模式信息
    unsigned char *getMode(void);

    // 设置电池状态
    void setBatteryStatus(BatStatus status);

    // 获取电池状态。
    BatStatus getBatteryStatus(void);

    // 获取是否需要校准。
    bool getCondition(void);

    // 获取是否充满。
    bool getFullyCharged(void);

private:
    BatStatus _curBarStatus;      // 当前电池状态
    BatMessage _batMessage;       // 电池数据
    unsigned char _statusData[2];
    unsigned char _modeData[2];

    bool _conditionFlag;          // 是否需要校准
    bool _fullyChargedFlag;       // 是否充满
};
