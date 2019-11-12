/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#pragma once
#include <QObject>
#include <QBitArray>
#include "Param.h"
#include "TimeDefine.h"
#include "SystemManagerInterface.h"

// 模块开机检查结果类型
enum ModulePoweronTestResult
{
    TE3_MODULE_SELFTEST_RESULT,
    E5_MODULE_SELFTEST_RESULT,
    N5_MODULE_SELFTEST_RESULT,
    TS3_MODULE_SELFTEST_RESULT,
    S5_MODULE_SELFTEST_RESULT,
    TT3_MODULE_SELFTEST_RESULT,
    T5_MODULE_SELFTEST_RESULT,
    CO2_MODULE_SELFTEST_RESULT,
    O2_MODULE_SELFTEST_RESULT,
    PRINTER72_SELFTEST_RESULT,
    PRINTER48_SELFTEST_RESULT,
    PANEL_KEY_POWERON_TEST_RESULT,
    MODULE_POWERON_TEST_RESULT_NR
};

//模块开机测试状态
enum ModulePoweronTestStatus
{
    SELFTEST_UNKNOWN,           //未开始
    SELFTEST_FAILED_AND_RETRY,  //自检失败并再次尝试
    SELFTEST_MODULE_RESET,      //模块复位
    SELFTEST_FAILED,            //自检失败
    SELFTEST_SUCCESS,           //自检成功
    SELFTEST_NOT_SUPPORT,       //模块不支持
    SELFTEST_STATUS_NR
};

// 触摸屏状态
enum TouchscreenStatus
{
    TOUCHSCREEN_DISENABLE = 0,              // 关闭
    TOUCHSCREEN_RESISTIVE = 1,              // 电阻
    TOUCHSCREEN_CAPACITIVE = 2,             // 电容
};

class SystemManagerPrivate;
class SystemManager : public QObject, public SystemManagerInterface
{
    Q_OBJECT
public:
    static SystemManager &getInstance();
    ~SystemManager();

public:
    // 获取屏幕像素点之间的距离。
    float getScreenPixelWPitch(void);
    float getScreenPixelHPitch(void);

    //  查询是否支持该功能。
    bool isSupport(ConfiguredFuncs funcs) const;

    // check whether param is support
    bool isSupport(ParamID paramID) const;

#ifdef Q_WS_QWS
    /**
     * @brief isTouchScreenOn check whether current touch screen is on or off
     * @return
     */
    bool isTouchScreenOn() const;

    /**
     * @brief setTouchScreenOnOff set the touch screen on and off status
     * @param sta
     */
    void setTouchScreenOnOff(int sta);

    /**
     * @brief configTouchScreen 配置触摸屏状态为电容,电阻或普通
     * @param sta
     */
    void configTouchScreen(int sta);
#endif

    // get the module config status
    unsigned int getModuleConfig() const;

    // set module power on test result
    void setPoweronTestResult(ModulePoweronTestResult module, ModulePoweronTestStatus result);

public:
    // 按键协议解析。
    void parseKeyValue(const unsigned char *data, unsigned int len);

    // 背光亮度设置/获取。
    void setBrightness(BrightnessLevel br);
    void enableBrightness(BrightnessLevel br);
    BrightnessLevel getBrightness(void);

    // check whether the system is going to turn off
    bool isGoingToTrunOff() const;
    void turnOff(bool flag);

    /**
     * @brief getCurWorkMode get the current work mode
     * @return the current work mode
     */
    virtual WorkMode getCurWorkMode() const;

    /**
     * @brief setWorkMode set the work mode
     * @param workmode
     */
    void setWorkMode(WorkMode workmode);

    /**
     * @brief setSystemTimeFormat set the system time format
     * @param format the new format
     */
    void setSystemTimeFormat(const TimeFormat &format);

    /**
     * @brief getSystemTimeFormat get the current system time format
     * @return the current system time format
     */
    TimeFormat getSystemTimeFormat() const;

    /**
     * @brief setSystemDateFormat set the system date format
     * @param format the new date  fomate
     */
    void setSystemDateFormat(const DateFormat &format);
    /**
     * @brief getSystemDateFormat get the current system date format
     * @return  the current system date format
     */
    DateFormat getSystemDateFormat() const;

signals:
    void workModeChanged(WorkMode mode);

    /**
     * @brief systemTimeFormatUpdated  系统时间格式更新
     * @param format  时间格式--12小时制、24小时制
     */
    void systemTimeFormatUpdated(TimeFormat format);

    /**
     * @brief standbySignal 待机信号
     * @param flag 待机标志
     */
    void standbySignal(bool flag);

private:
    SystemManager();

private:
    SystemManagerPrivate * d_ptr;
};

#define systemManager (SystemManager::getInstance())
