#include "PowerManager.h"
#include "WindowManager.h"
#include "BatteryAlarm.h"
#include "BatteryIconWidget.h"
#include "SystemStatusBarWidget.h"
#include "Debug.h"
#include "PrintManager.h"
#include <QTimer>
#include "IMessageBox.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"

PowerManager *PowerManager::_selfObj = NULL;

/***************************************************************************************************
 * 构造
 **************************************************************************************************/
PowerManager::PowerManager()
{
    _initBatteryData();
    _powerType = systemBoardProvider.getPowerSuplyType();

    _timer = new QTimer();
    _timer->setInterval(5000);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timerOut()));

    _shutdownMsgBox = new  IMessageBox (trs("BatteryShutDown"), trs("Systemwillshutdownatonce"), false);
    _shutdownMsgBox->setYesKeyVisible(false);
}

/***************************************************************************************************
 * 析构
 **************************************************************************************************/
PowerManager::~PowerManager()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

/***************************************************************************************************
 * 函数说明:
 *      电源管理主运行函数，用于处理电源的变化。1s运行一次
 **************************************************************************************************/
void PowerManager::run()
{
    _moniterRun();
}

/***************************************************************************************************
 * 监护模式下的电池管理。
 **************************************************************************************************/
void PowerManager::_moniterRun(void)
{
    // 电池错误与电池通讯错误计数。
    static int errorCount = 0;          // 检测结果为电池错误的次数
    static int commFault = 0;           // 检测结果为通信中断的次数

    static PowerSuplyType lastPowerType = POWER_SUPLY_UNKOWN;


    int batteryVoltage = 0;
    int batteryADCVoltage = 0;
    int batteryCapacity = 0;
    //    int batterypercent = 0;
    bool replaceBattery = _replaceBattery;    // 上次replaceBattery的情况
    bool batteryError = false;
    bool batteryCOMFault = false;
    bool powerTypeChange = false;
    bool calibrationFlag = false;

    //lowBattery 1s刷新一次,电池电量图标则看情况.
    batteryVoltage = batteryMessage.getVoltage();
    batteryADCVoltage = batteryMessage.getVoltageADC();
    batteryCapacity = batteryMessage.getRemainingCapacity();
    //    batterypercent = batteryMessage.getRelativeStateOfCharge();



    if (_isChargePeriod)
    {
        _chargrdDelayTime ++;
        if (_chargrdDelayTime > 90)
        {
            _isChargePeriod = false;
            _chargrdDelayTime = 0;
        }
    }

    lastPowerType = _powerType;
    PowerSuplyType currentPowerType = systemBoardProvider.getPowerSuplyType();
    if (_powerType != currentPowerType)
    {
        powerTypeChange = true;
        _powerType = currentPowerType;
    }

    //电源线插拔时作出行为
    _powerLineChange(lastPowerType,currentPowerType,powerTypeChange);

    //弹出关机提示框后，5s若有AC接入，则不关机
    if (_shutBattery)
    {
        if (currentPowerType != POWER_SUPLY_BAT)
        {
            //不关机就打开储存功能；
            systemManager.turnOff(false);
            _timer->stop();
            _shutBattery = false;
            if (_shutdownMsgBox != NULL && _shutdownMsgBox->isVisible())
            {
                _shutdownMsgBox->close();
            }
        }
    }



    //    unsigned char *data;
    //    data = powerManager.batteryMessage.getStatus();
    //    debug("0x%02x, 0x%02x\n",data[1],data[0]);
    //    debug("battery voltage: %d   battery capacity: %d  time: %d    %d%\n",batteryVoltage,batteryCapacity,_timeValue,batterypercent);


    // 根据电池信息修改电池图标。
    if (currentPowerType == POWER_SUPLY_UNKOWN)
    {
        //判断电压
        if (batteryADCVoltage > 0 && batteryADCVoltage <= 10500)
        {
            //在defib charging状态下
            if(_isChargePeriod)
            {
                if (batteryADCVoltage < 9852)
                {
                    _lowBattery = true;
                    if (batteryADCVoltage < 9751)
                    {
                        _replaceBattery = true;
                        if (batteryADCVoltage < 9724 && replaceBattery)
                        {
                            // 若AC没接入则发送关机请求
                            if (!_shutBattery)
                            {
                                _shutdownWarn();
                            }
                        }
                    }
                }
            }
            //在printing状态下
            //在monitor状态下
            else
            {
                if (batteryADCVoltage < 10500)
                {
                    _lowBattery = true;
                    if (batteryADCVoltage < 10160)
                    {
                        _replaceBattery = true;

                        if (batteryADCVoltage < 10020 && replaceBattery)
                        {
                            // 若AC没接入则发送关机请求
                            if (!_shutBattery)
                            {
                                _shutdownWarn();
                            }
                        }
                    }
                }
            }
        }
        if (currentPowerType != POWER_SUPLY_BAT)
        {
            if (batteryADCVoltage > 10305)
            {
                _replaceBattery = false;
            }
            //电压大于10.5V时，取消lowBattery，replaceBattery报警
            if (batteryADCVoltage > 10600)
            {
                _lowBattery = false;
                _replaceBattery = false;
            }
        }

        // 电池图标显示
        if (systemBoardProvider.getErrorWaringCode() == ERR_CODE_BAT_COMM_FAULT) // 电池通信中断。
        {
            errorCount = 0;
            commFault++;
            if (commFault >= 120)
            {
                batteryCOMFault = true;
                if (commFault == 120)
                {
                    batteryBarWidget.setStatus(BATTERY_COM_FAULT);
                    ErrorLogItem *item = new CriticalFaultLogItem();
                    item->setName("System Board Error");
                    item->setLog("Battery Communication Fault.\r\n");
                    item->setSubSystem(ErrorLogItem::SUB_SYS_SYSTEM_BOARD);
                    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
                    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
                    errorLog.append(item);
                }
                commFault = 120;
            }
            else
            {
                _batteryIconNormal();
            }
        }
        else if (systemBoardProvider.getErrorWaringCode() == ERR_CODE_BAT_ERROR)      // 有电池错误。
        {
            commFault = 0;
            errorCount++;  // todo: 使用这种方法计时不准确，后期可使用gettimeofday()。
            if (errorCount >= 300)
            {
                batteryError = true;
                if (errorCount == 300)
                {
                    batteryBarWidget.setStatus(BATTERY_ERROR);
                    ErrorLogItem *item = new CriticalFaultLogItem();
                    item->setName("System Board Error");
                    item->setLog("Battery Error.\r\n");
                    item->setSubSystem(ErrorLogItem::SUB_SYS_SYSTEM_BOARD);
                    item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
                    item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
                    errorLog.append(item);
                }
                errorCount = 300;
            }
            else
            {
                _batteryIconNormal();
            }
        }
        else
        {
            _batteryIconNormal();
        }

        errorCount = 0;
        commFault = 0;

        batteryBarWidget.setStatus(BATTERY_NOT_EXISTED);
        //初始化数据，只有当电池拔出时更新
        batteryMessage.setInit();
        _initBatteryData();
    }
    else if (currentPowerType == POWER_SUPLY_AC)  // AC供电。
    {
        errorCount = 0;
        commFault = 0;

        batteryBarWidget.setStatus(BATTERY_NOT_EXISTED);
        //初始化数据，只有当电池拔出时更新
        batteryMessage.setInit();
        _initBatteryData();
    }
    else // 有电池供电。
    {
        if (systemBoardProvider.getErrorWaringCode() == ERR_CODE_NONE)   // 无电池错误。
        {
            errorCount = 0;
            commFault = 0;
            // todo,判断是否为low battery
            // 充电/准备阶段 home按键禁用
            if ((batteryVoltage > 0 && batteryVoltage <= 10570 ) ||
                    (batteryCapacity > 0 && batteryCapacity <= LOW_BATTERY))
            {
                //判断电池电量
                if (batteryCapacity > 0 && batteryCapacity < LOW_BATTERY)
                {
                    _lowBattery = true;
                    if (currentPowerType == POWER_SUPLY_BAT)
                    {
                        if (batteryCapacity < REPLACE_BATTERY)
                        {
                            _replaceBattery = true;
                            // 一定要发生了replaceBattery后，才能关机
                            if (batteryCapacity < SHUTDOWN_BATTERY && replaceBattery)
                            {
                                if (!_shutBattery)
                                {
                                    _shutdownWarn();
                                }
                            }
                        }
                    }
                }

                //判断电压
                //在defib charging状态下
                if(_isChargePeriod)
                {

                    if (batteryVoltage < 10280)
                    {
                        _lowBattery = true;

                        if (currentPowerType == POWER_SUPLY_BAT)
                        {
                            if (batteryVoltage < 10020)
                            {
                                _replaceBattery = true;

                                if (batteryVoltage < 9986 && replaceBattery)
                                {
                                    // 若AC没接入则发送关机请求
                                    if (!_shutBattery)
                                    {
                                        _shutdownWarn();
                                    }
                                }
                            }
                        }
                    }
                }
                //在monitor状态下
                else
                {
                    if (batteryVoltage < 10570)
                    {
                        _lowBattery = true;

                        if (currentPowerType == POWER_SUPLY_BAT)
                        {
                            if (batteryVoltage < 10230)
                            {
                                _replaceBattery = true;

                                if (batteryVoltage < 10090 && replaceBattery)
                                {
                                    // 发送shut down到pd模块
                                    // todo
                                    // Assert the HOST_INT signal line
                                    // Halt collettion of snapshots, full disclosure, trend, error and 12 Lead
                                    // ECG data;
                                    // complete storage of any data not yet written to non-volatile memory
                                    // shut the system down
                                    // 若AC没接入则发送关机请求
                                    if (!_shutBattery)
                                    {
                                        _shutdownWarn();
                                    }
                                }
                            }
                        }
                    }
                }

            }
            ////电量大于1200mh，电压大于10.57V时，取消lowBattery，replaceBattery报警
            //电量大于350mh，电压大于10.2V时，取消replaceBattery报警
            if (currentPowerType != POWER_SUPLY_BAT)
            {
                if (batteryVoltage > 10300 && batteryCapacity > REPLACE_BATTERY_EXIT)
                {
                    _replaceBattery = false;
                }

                if (batteryVoltage > 10580 && batteryCapacity > LOW_BATTERY_EXIT)
                {
                    _lowBattery = false;
                    _replaceBattery = false;
                }
            }

            // 判断电池是否需要校准。
            if (batteryMessage.getCondition())
            {
                calibrationFlag = true;
                batteryBarWidget.setStatus(BATTERY_CALIBRATION_REQUIRED);
            }
            else
            {
                _batteryIconNormal();
            }
        }
        else
        {
            //判断电压
            if (batteryADCVoltage > 0 && batteryADCVoltage <= 10500)
            {
                //在defib charging状态下
                if(_isChargePeriod)
                {
                    if (batteryADCVoltage < 9852)
                    {
                        _lowBattery = true;
                        if (batteryADCVoltage < 9751)
                        {
                            _replaceBattery = true;

                            if (batteryADCVoltage < 9724 && replaceBattery)
                            {
                                // 若AC没接入则发送关机请求
                                if (!_shutBattery)
                                {
                                    _shutdownWarn();
                                }
                            }
                        }
                    }
                }
                //在printing状态下
                //在monitor状态下
                else
                {
                    if (batteryADCVoltage < 10500)
                    {
                        _lowBattery = true;
                        if (batteryADCVoltage < 10160)
                        {
                            _replaceBattery = true;

                            if (batteryADCVoltage < 10020 && replaceBattery)
                            {
                                // 若AC没接入则发送关机请求
                                if (!_shutBattery)
                                {
                                    _shutdownWarn();
                                }
                            }
                        }
                    }
                }
            }
            else if (currentPowerType != POWER_SUPLY_BAT)
            {
                if (batteryADCVoltage > 10305)
                {
                    _replaceBattery = false;
                }
                //电压大于10.5V时，取消lowBattery，replaceBattery报警
                if (batteryADCVoltage > 10600)
                {
                    _lowBattery = false;
                    _replaceBattery = false;
                }
            }
            if (currentPowerType != POWER_SUPLY_BAT)
            {
                if (batteryADCVoltage > 10305)
                {
                    _replaceBattery = false;
                }
                //电压大于10.5V时，取消lowBattery，replaceBattery报警
                if (batteryADCVoltage > 10600)
                {
                    _lowBattery = false;
                    _replaceBattery = false;
                }
            }

            // 电池图标显示
            if (systemBoardProvider.getErrorWaringCode() == ERR_CODE_BAT_COMM_FAULT) // 电池通信中断。
            {
                errorCount = 0;
                commFault++;
                if (commFault >= 120)
                {
                    batteryCOMFault = true;
                    if (commFault == 120)
                    {
                        batteryBarWidget.setStatus(BATTERY_COM_FAULT);
                        ErrorLogItem *item = new CriticalFaultLogItem();
                        item->setName("System Board Error");
                        item->setLog("Battery Communication Fault.\r\n");
                        item->setSubSystem(ErrorLogItem::SUB_SYS_SYSTEM_BOARD);
                        item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
                        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
                        errorLog.append(item);
                    }
                    commFault = 120;
                }
                else
                {
                    _batteryIconNormal();
                }
            }
            else if (systemBoardProvider.getErrorWaringCode() == ERR_CODE_BAT_ERROR)      // 有电池错误。
            {
                commFault = 0;
                errorCount++;  // todo: 使用这种方法计时不准确，后期可使用gettimeofday()。
                if (errorCount >= 300)
                {
                    batteryError = true;
                    if (errorCount == 300)
                    {
                        batteryBarWidget.setStatus(BATTERY_ERROR);
                        ErrorLogItem *item = new CriticalFaultLogItem();
                        item->setName("System Board Error");
                        item->setLog("Battery Error.\r\n");
                        item->setSubSystem(ErrorLogItem::SUB_SYS_SYSTEM_BOARD);
                        item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
                        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
                        errorLog.append(item);
                    }
                    errorCount = 300;
                }
                else
                {
                    _batteryIconNormal();
                }
            }
            else
            {
                _batteryIconNormal();
            }
        }
    }

    if (currentPowerType != POWER_SUPLY_BAT)
    {
        _replaceBattery = false;
        _lowBattery = false;
        _defibDisable = false;
    }

    //根据报警决定是否修改电池图标
    _batteryChangeByAlarm();

    _updateBatAlarm(BAT_ONESHOT_REPLACE_BATTERY, _replaceBattery);
    _updateBatAlarm(BAT_ONESHOT_LOW_BATTERY, _lowBattery);
    _updateBatAlarm(BAT_ONESHOT_NO_BATTERY, !hasBattery());
    _updateBatAlarm(BAT_ONESHOT_BATTERY_ERROR, batteryError);
    _updateBatAlarm(BAT_ONESHOT_BATTERY_COMM_FAULT, batteryCOMFault);
    _updateBatAlarm(BAT_ONESHOT_BATTERY_CALIBRATION, calibrationFlag);
}

/***************************************************************************************************
 * 服务模式下的电池管理。
 **************************************************************************************************/
void PowerManager::_configRun(void)
{
    int batteryVoltage = 0;
    int batteryADCVoltage = 0;
    int batteryCapacity = 0;

    PowerSuplyType currentPowerType = systemBoardProvider.getPowerSuplyType();


    //弹出关机提示框后，5s若有AC接入，则不关机
    if (_shutBattery)
    {
        if (currentPowerType == POWER_SUPLY_AC || currentPowerType == POWER_SUPLY_AC_BAT)
        {
            systemManager.turnOff(false);
            _timer->stop();
            _shutBattery = false;
            if (_shutdownMsgBox != NULL && _shutdownMsgBox->isVisible())
            {
                _shutdownMsgBox->close();
            }
        }
    }

    batteryVoltage = batteryMessage.getVoltage();
    batteryADCVoltage = batteryMessage.getVoltageADC();
    batteryCapacity = batteryMessage.getRemainingCapacity();

    //    debug("battery voltage: %d   battery capacity: %d   ADC:  %d \n",batteryVoltage,batteryCapacity,batteryADCVoltage);

    if (currentPowerType == POWER_SUPLY_BAT)  // Battery供电。
    {
        if (systemBoardProvider.getErrorWaringCode() == ERR_CODE_NONE)   // 无电池错误。
        {
            if ((batteryVoltage > 0 && batteryVoltage <= 10090 ) ||
                    (batteryCapacity > 0 && batteryCapacity <= SHUTDOWN_BATTERY))
            {
                //判断电池电量
                if (batteryCapacity < SHUTDOWN_BATTERY)
                {
                    if (!_shutBattery)
                    {
                        _shutdownWarn();
                    }
                }

                if (batteryVoltage < 10090)
                {
                    if (!_shutBattery)
                    {
                        _shutdownWarn();
                    }
                }
            }
        }
        else
        {
            //判断电压
            if (batteryADCVoltage > 0 && batteryADCVoltage <= 10020)
            {
                if (batteryADCVoltage < 10020)
                {
                    // 若AC没接入则发送关机请求
                    if (!_shutBattery)
                    {
                        _shutdownWarn();
                    }
                }
            }
        }
    }
    else if(currentPowerType == POWER_SUPLY_UNKOWN)
    {
        //判断电压
        if (batteryADCVoltage > 0 && batteryADCVoltage <= 10020)
        {
            if (batteryADCVoltage < 10020)
            {
                // 若AC没接入则发送关机请求
                if (!_shutBattery)
                {
                    _shutdownWarn();
                }
            }
        }
    }
}

/***************************************************************************************************
 *    刷新电池图标
 **************************************************************************************************/
void PowerManager::_batteryIconNormal(void)
{
    if (batteryMessage.getFullyCharged())
    {
        batteryBarWidget.setIconFull();
    }
    else if (_lowBattery || _replaceBattery || _shutBattery)
    {
        batteryBarWidget.setIconLow();
    }
    else
    {
        batteryBarWidget.setIcon(_timeValue);
    }
    batteryBarWidget.setStatus(BATTERY_NORMAL);
}

void PowerManager::_powerLineChange(const PowerSuplyType &lastPowerType, const PowerSuplyType &currentPowerType, const bool &powerTypeChange)
{
    static int batteryCountDown = 0;    //90s至少刷新一次电量图标.
    static int CountDown = 0;    //90s至少刷新一次电量图标.
    static bool ACtoBattery = false;
    static bool BatterytoAC = false;
    int batteryVoltage = batteryMessage.getVoltage();
    int batteryCapacity = batteryMessage.getRemainingCapacity();

    static unsigned char count = 0;
    //电池刚接入或者电池接入超过0s但不足10s,执行此条件
    if((lastPowerType != POWER_SUPLY_BAT && lastPowerType != POWER_SUPLY_AC_BAT
        && (currentPowerType == POWER_SUPLY_AC_BAT || currentPowerType == POWER_SUPLY_BAT))
            || count != 0)
    {
        //电池接入到达10s,则不再满足执行条件
        count = count >= 10 ? 0 : (count+1);
    }

    if(powerTypeChange == true)
    {
        //电池接入10s内不应执行90s延迟.
        if(count == 0)
        {
            if(lastPowerType == POWER_SUPLY_BAT && _powerType == POWER_SUPLY_AC_BAT)
            {
                BatterytoAC = true;
            }
            else if(lastPowerType == POWER_SUPLY_AC_BAT && _powerType == POWER_SUPLY_BAT)
            {
                ACtoBattery = true;
            }
            else
            {
                ACtoBattery = false;
                BatterytoAC = false;
            }
        }
        else
        {
            ACtoBattery = false;
            BatterytoAC = false;
        }
    }
    //无变化
    if(ACtoBattery == false && BatterytoAC == false)
    {
        // 使用BAT充电
        if (currentPowerType == POWER_SUPLY_BAT)
        {
            // PD充电后一段时间不显示
            if (!_isChargePeriod)
            {
                if (batteryVoltage != 0 && batteryCapacity!= 65535)
                {
                    _timeValue = batteryMessage.getAverageTimeToEmpty() / 30;
                }
            }
        }
        // 不使用bat充电
        else
        {
            if (batteryCapacity != 65535)
            {
                _timeValue = (batteryCapacity / CFG_CURRENT_DRAW) * 2;
            }
        }
    }
    //AC拔掉
    else if(ACtoBattery == true && BatterytoAC == false)
    {
        ++batteryCountDown;
        //BAT状态倒计时到了,或者不再是BAT状态,则退出AC的状态
        if(batteryCountDown >= TIME_CHANGE && currentPowerType == POWER_SUPLY_BAT)
        {
            // PD充电后一段时间不显示
            if(!_isChargePeriod)
            {
                ACtoBattery = false;
                batteryCountDown = 0;
            }
        }
        else if(currentPowerType != POWER_SUPLY_BAT)
        {
            ACtoBattery = false;
            batteryCountDown = 0;
        }
    }
    //AC插上
    else if(ACtoBattery == false && BatterytoAC == true)
    {
        ++CountDown;
        //AC_BAT状态倒计时到了,或者不再是AC_BAT状态,则退出AC插上的状态
        if((CountDown >= TIME_CHANGE && currentPowerType == POWER_SUPLY_AC_BAT) || currentPowerType != POWER_SUPLY_AC_BAT)
        {
            BatterytoAC = false;
            CountDown = 0;
        }
    }
    //AC多次插拔
    else if(ACtoBattery == true && BatterytoAC == true)
    {
        ++CountDown;
        //AC接入 或者battery接入时,电量CountDown固定刷新90s
        if(currentPowerType == POWER_SUPLY_AC_BAT || currentPowerType == POWER_SUPLY_BAT)
        {
            //battery未供电时,battery的90s计时清零.
            if(currentPowerType != POWER_SUPLY_BAT && batteryCountDown != 0)
            {
                batteryCountDown = 0;
            }
            //公共倒计时到叻,且电池供电不稳定,就用电量刷新界面.
            if(CountDown >= TIME_CHANGE && batteryCountDown < TIME_CHANGE)
            {
                BatterytoAC = false;
                CountDown = 0;
            }
            //battery接入时,battery90s后电池供电稳定,采用电池信息刷新界面.
            if(currentPowerType == POWER_SUPLY_BAT)
            {
                ++batteryCountDown;
                if(batteryCountDown >= TIME_CHANGE)
                {
                    ACtoBattery = false;
                    BatterytoAC = false;
                    batteryCountDown = 0;
                    CountDown = 0;
                }
            }
        }
        else
        {
            ACtoBattery = false;
            BatterytoAC = false;
            batteryCountDown = 0;
            CountDown = 0;
        }
    }


}

void PowerManager::_batteryChangeByAlarm()
{
    static bool lastBatteryLow = false;
    if(_lowBattery || _replaceBattery || _shutBattery)
    {
        if (systemBoardProvider.getErrorWaringCode() == ERR_CODE_NONE) // 电池通信中断.
        {
            lastBatteryLow = true;
            _batteryIconNormal();
        }
        else
        {
            lastBatteryLow = false;
        }
    }
    else if(lastBatteryLow == true && (_powerType == POWER_SUPLY_BAT || POWER_SUPLY_AC_BAT))
    {
        lastBatteryLow = false;
        // 使用BAT充电
        if (_powerType == POWER_SUPLY_BAT)
        {
            // PD充电后一段时间不显示
            if (!_isChargePeriod)
            {
                if (batteryMessage.getVoltage() != 0 && batteryMessage.getRemainingCapacity()!= 65535)
                {
                    _timeValue = batteryMessage.getAverageTimeToEmpty() / 30;
                }
            }
        }
        // 不使用bat充电
        else
        {
            if (batteryMessage.getRemainingCapacity() != 65535)
            {
                _timeValue = (batteryMessage.getRemainingCapacity() / CFG_CURRENT_DRAW) * 2;
            }
        }
    }
}

/***************************************************************************************************
 *    返回是否有电池接入。
 **************************************************************************************************/
bool PowerManager::hasBattery(void)
{
    if ((systemBoardProvider.getPowerSuplyType() == POWER_SUPLY_BAT) ||
            (systemBoardProvider.getPowerSuplyType() == POWER_SUPLY_AC_BAT))
    {
        return true;
    }

    return false;
}

/***************************************************************************************************
 * 函数说明:
 *      更新电池报警
 * 函数参数:
 *      isOccure:是否发生，true:发生，false:移除
 **************************************************************************************************/
void PowerManager::_updateBatAlarm(BatOneShotType type, bool isOccure)
{
    batteryOneShotAlarm.setOneShotAlarm(type, isOccure);
}

/***************************************************************************************************
 * 关机弹框。
 **************************************************************************************************/
void PowerManager::_shutdownWarn(void)
{
    _timer->start();
    _shutBattery = true;
    //关闭储存功能；
    systemManager.turnOff(true);

    // 清除界面弹出框
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }
    _shutdownMsgBox->show();
}

/***************************************************************************************************
 * 电池数据初始化。
 **************************************************************************************************/
void PowerManager::_initBatteryData(void)
{
    _replaceBattery = false;
    _lowBattery = false;
    _shutBattery = false;
    _defibDisable = false;
    _isChargePeriod = false;
    _chargrdDelayTime = 0;
    _timeValue = -2;
}

/***************************************************************************************************
 * 关机定时器时间到。
 **************************************************************************************************/
void PowerManager::_timerOut()
{
    if (systemBoardProvider.getPowerSuplyType() == POWER_SUPLY_BAT)
    {
        QTimer::singleShot(100, this, SLOT(_powerOff()));
    }
}

/***************************************************************************************************
 * power off。
 **************************************************************************************************/
void PowerManager::_powerOff()
{
    systemBoardProvider.requestShutdown();
}
