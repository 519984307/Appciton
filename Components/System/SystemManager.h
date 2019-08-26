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

#ifdef Q_WS_X11
#include <QTcpSocket>
#include <QQueue>
#endif

#define IDM_SOFTWARE_VERSION ("1.0.3")

// 前面板按键状态,与系统板获取按键状态命令回复一致
enum PanelKeyStatus
{
    PANEL_KEY_0 = 0,
    PANEL_KEY_1,
    PANEL_KEY_2,
    PANEL_KEY_3,
    PANEL_KEY_4,
    PANEL_KEY_ALARM,
    PANEL_KEY_MENU,
    PANEL_KEY_NIBP,
    PANEL_KEY_RECORDER,
    PANEL_KEY_ENERGY_DN,
    PANEL_KEY_ENERGY_UP,
    PANEL_KEY_CHARGING,
    PANEL_KEY_SHOCK,
    PANEL_KEY_ENTER,
    PANEL_KEY_0_4,
    PANEL_KEY_1_4,
    PANEL_KEY_0_3,
    PANEL_KEY_NR = 24
};

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

class SystemSelftestMenu;
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

    // 获取屏幕尺寸
    float getScreenSize(void);

    // 获取屏幕分辨率
    QString getScreenResolution(void);

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
    void setAutoBrightness(int br);
    void enableBrightness(int br);
    BrightnessLevel getBrightness(void);

    // 加载初始底层模式。
    void loadInitBMode(void);

    // 是否确认了自检结果
    bool isAcknownledgeSystemTestResult();

    // system self-test is over
    bool isSystemSelftestOver() const;
    void systemSelftestOver();

    // hide system test diag
    void closeSystemTestDialog();

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

#ifdef Q_WS_X11
public:
    bool sendCommand(const QByteArray &cmd);

public slots:
    void onCtrlSocketReadReady();

signals:
    void metronomeReceived();
#endif

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

private slots:
    void publishTestResult();

private:
    SystemManager();

private:
    SystemManagerPrivate * d_ptr;
};

#define systemManager (SystemManager::getInstance())
