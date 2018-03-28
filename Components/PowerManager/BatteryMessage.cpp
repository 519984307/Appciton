#include "BatteryMessage.h"
#include "SystemBoardProvider.h"
#include "SystemBoardProviderDefine.h"
#include "Debug.h"

/***************************************************************************************************
 * 构造
 **************************************************************************************************/
BatteryMessage::BatteryMessage()
{
    systemBoardProvider.queryFixedBatMessage();
    _curBarStatus = BAT_STATUS_NONE;

    // 给剩余电量参数设置一个初始值
    setInit();
}

/***************************************************************************************************
 * 析构
 **************************************************************************************************/
BatteryMessage::~BatteryMessage()
{

}

/***************************************************************************************************
 * 函数说明:
 *      解析不变动的电池信息数据包
 * 函数参数:
 *      data:电池数据
 **************************************************************************************************/
void BatteryMessage::updateFixedBatMessage(unsigned char *data)
{
    memcpy(&_batMessage.fixedBatMessage, data, sizeof(_batMessage.fixedBatMessage));

    int offSet = sizeof(_batMessage.fixedBatMessage);
    char str1[data[offSet] + 1];
    memcpy(str1, &data[offSet + 1], data[offSet]);
    str1[data[offSet]] = '\0';
    _batMessage.serialNumber = str1;


    offSet += 1 + data[offSet];
    char str2[data[offSet] + 1];
    memcpy(str2, &data[offSet + 1], data[offSet]);
    str2[data[offSet]] = '\0';
    _batMessage.manufacturerName = str2;

    offSet += 1 + data[offSet];
    char str3[data[offSet] + 1];
    memcpy(str3, &data[offSet + 1], data[offSet]);
    str3[data[offSet]] = '\0';
    _batMessage.deviceName = str3;

    offSet += 1 + data[offSet];
    char str4[data[offSet] + 1];
    memcpy(str4, &data[offSet + 1], data[offSet]);
    str4[data[offSet]] = '\0';
    _batMessage.deviceChemistry = str4;

    return;
}

/***************************************************************************************************
 * 函数说明:
 *      解析周期性的电池信息数据包
 * 函数参数：
 *      data:电池数据
 **************************************************************************************************/
void BatteryMessage::updatePeriodicBatMessage(unsigned char *data)
{
    memcpy(&_batMessage.periodBatMessage, data, sizeof(_batMessage.periodBatMessage));

    if (_batMessage.periodBatMessage.status[0] & 0x20)
    {
        _fullyChargedFlag = true;
    }
    else
    {
        _fullyChargedFlag = false;
    }

    if (_batMessage.periodBatMessage.mode[0] & 0x80)
    {
        _conditionFlag = true;
    }
    else
    {
        _conditionFlag = false;
    }

    return;
}

/***************************************************************************************************
 * 函数说明:
 *      解析周期性的电池ADC采集电压
 * 函数参数：
 *      data:电池电压数据
 **************************************************************************************************/
void BatteryMessage::updatePeriodicBatValue(unsigned char *data)
{
    memcpy(&_batMessage.voltageADC, data, sizeof(_batMessage.voltageADC));
    return;
}

/***************************************************************************************************
 * 函数说明:
 *      设置电池参数,不需要设置的参数，发送0xff
 * 函数参数：
 *      CanSetBatParam:电池参数
 **************************************************************************************************/
void BatteryMessage::setBatParam(CanSetBatParam batParam)
{
    systemBoardProvider.sendCmd(MSG_CMD_CFG_BAT_ARGS, (unsigned char *)&batParam, sizeof(batParam));
    return;
}

/***************************************************************************************************
 * 函数说明:
 *      电池信息初始化
 **************************************************************************************************/
void BatteryMessage::setInit()
{
    _batMessage.init();
    _conditionFlag = false;
    _fullyChargedFlag = false;
}

/***************************************************************************************************
 * 函数说明:
 *      获取电池温度
 * 返回值:
 *      0-6553.5k
 **************************************************************************************************/
unsigned int BatteryMessage::getTemperature(void)
{
    return _batMessage.periodBatMessage.temperature[0] + (_batMessage.periodBatMessage.
        temperature[1] << 8);
}

/***************************************************************************************************
 * 函数说明:
 *      获取电池电压。
 * 返回值:
 *     0-65535mV
 **************************************************************************************************/
unsigned int BatteryMessage::getVoltage(void)
{
    return _batMessage.periodBatMessage.voltage[0] + (_batMessage.periodBatMessage.voltage[1] << 8);
}

/***************************************************************************************************
 * 函数说明:
 *      获取电池ADC采集电压。
 * 返回值:
 *     0-65535mV
 **************************************************************************************************/
unsigned int BatteryMessage::getVoltageADC(void)
{
    return _batMessage.voltageADC[0] + (_batMessage.voltageADC[1] << 8);
}

/***************************************************************************************************
 * 函数说明:
 *      获取电池电流
 * 返回值:
 *      0到32767mA为充电，0到-32768为放电
 **************************************************************************************************/
short BatteryMessage::getCurrent(void)
{
    return _batMessage.periodBatMessage.current[0] + (_batMessage.periodBatMessage.current[1] << 8);
}

/***************************************************************************************************
 * 函数说明:
 *      获取每分钟平均电流
 * 返回值:
 *      0到32767mA为充电，0到-32768为放电
 **************************************************************************************************/
short BatteryMessage::getAverageCurrent(void)
{
    return _batMessage.periodBatMessage.averageCurrent[0] + (_batMessage.periodBatMessage.
            averageCurrent[1] << 8);
}

/***************************************************************************************************
 * 函数说明：
 *      获取相对电荷状态，即相对电池剩余百分比
 * 返回值:
 *      0-100%
 **************************************************************************************************/
unsigned int BatteryMessage::getRelativeStateOfCharge(void)
{
    return _batMessage.periodBatMessage.relStateOfCharge[0] + (_batMessage.periodBatMessage.
            relStateOfCharge[1] << 8);
}

/***************************************************************************************************
 * 函数说明：
 *      获取绝对电荷状态，即绝对电池剩余百分比
 * 返回值:
 *      0-100+%
 **************************************************************************************************/
unsigned int BatteryMessage::getAbsoluteStateOfCharge(void)
{
    return _batMessage.periodBatMessage.AbsStateOfCharge[0] + (_batMessage.periodBatMessage.
            AbsStateOfCharge[1] << 8);
}

/***************************************************************************************************
 * 函数说明:
 *      获取剩余电池容量
 * 返回值：
 *      0-65535mAh
 **************************************************************************************************/
unsigned int BatteryMessage::getRemainingCapacity(void)
{
    return _batMessage.periodBatMessage.remainingCapacity[0] + (_batMessage.periodBatMessage.
            remainingCapacity[1] << 8);
}

/***************************************************************************************************
 * 函数说明:
 *      获取电池剩余运行时间
 * 返回值:
 *      0-65534minutes 65535表示电池没有在放电
 **************************************************************************************************/
unsigned int BatteryMessage::getRunTimeToEmpty(void)
{
    return _batMessage.periodBatMessage.runTimeOfEmpty[0] + (_batMessage.periodBatMessage.
            runTimeOfEmpty[1] << 8);
}

/***************************************************************************************************
 * 函数说明:
 *      获取电池平均剩余运行时间
 * 返回值：
 *      0-65534minutes 65535表示电池没有在放电
 **************************************************************************************************/
unsigned int BatteryMessage::getAverageTimeToEmpty(void)
{
    return _batMessage.periodBatMessage.averageTimeToEmpty[0] + (_batMessage.periodBatMessage.
            averageTimeToEmpty[1] << 8);
}

/***************************************************************************************************
 * 函数说明：
 *      获取电池平均充满时间
 * 返回值：
 *      0-65534minutes 65535表示电池没有在充电
 **************************************************************************************************/
unsigned int BatteryMessage::getAverageTimeToFull(void)
{
    return _batMessage.periodBatMessage.averageTimeToFull[0] + (_batMessage.periodBatMessage.
            averageTimeToFull[1] << 8);
}

/***************************************************************************************************
 * 函数说明:
 *      获取电池状态
 *      对应电池0x16
 * 返回值:
 *      返回值含警报、状态位及错误代码
 **************************************************************************************************/
unsigned char *BatteryMessage::getStatus(void)
{
    _statusData[0] = _batMessage.periodBatMessage.status[0];
    _statusData[1] = _batMessage.periodBatMessage.status[1];

    return _statusData;
    //    return _batMessage.periodBatMessage.status[0] + (_batMessage.periodBatMessage.status[1] << 8);
}

/***************************************************************************************************
 * 函数说明:
 *      电池模式信息
 *      对应电池0x03
 **************************************************************************************************/
unsigned char *BatteryMessage::getMode()
{
    _modeData[0] = _batMessage.periodBatMessage.mode[0];
    _modeData[1] = _batMessage.periodBatMessage.mode[1];

    return _modeData;
}

/***************************************************************************************************
 * 函数说明:
 *      设置当前电池电池状态
 * 返回值:
 *      电池状态
 **************************************************************************************************/
void BatteryMessage::setBatteryStatus(BatStatus status)
{
    _curBarStatus = status;
}

/***************************************************************************************************
 * 函数说明:
 *      获取当前电池电池状态
 * 返回值:
 *      电池状态
 **************************************************************************************************/
BatStatus BatteryMessage::getBatteryStatus(void)
{
    return _curBarStatus;
}

/***************************************************************************************************
 * 函数说明:
 *      获取是否需要校准。
 **************************************************************************************************/
bool BatteryMessage::getCondition(void)
{
    return _conditionFlag;
}

/***************************************************************************************************
 * 函数说明:
 *      获取电池是否充满。
 **************************************************************************************************/
bool BatteryMessage::getFullyCharged(void)
{
    return _fullyChargedFlag;
}
