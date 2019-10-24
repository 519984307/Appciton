/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/6
 **/

#pragma once
#include "SystemDefine.h"
#include "ParamDefine.h"

enum WorkMode
{
    WORK_MODE_NORMAL = 0,
    WORK_MODE_DEMO,
    WORK_MODE_STANDBY,
    WORK_MODE_NR,
};

enum ConfiguredFuncs
{
    CONFIG_ST = 0x01,
    CONFIG_RESP = 0x02,
    CONFIG_ECG12LEADS = 0x04,
    CONFIG_SPO2 = 0x08,
    CONFIG_NIBP = 0x10,
    CONFIG_CO2 = 0x20,
    CONFIG_TEMP = 0x40,
    CONFIG_AG = 0x80,
    CONFIG_CO = 0x100,
    CONFIG_IBP = 0x200,
    CONFIG_WIFI = 0x400,
    CONFIG_TOUCH = 0x800,
    CONFIG_O2 = 0x1000,
    CONFIG_PRINTER = 0x2000,
    CONFIG_HDMI = 0x4000,
    CONFIG_NURSE_CALL = 0x8000,
    CONFIG_ANALOG_OUTPUT = 0x10000,
    CONFIG_SYNC_DEFIBRILLATION = 0x20000
};

class SystemManagerInterface
{
public:
    virtual ~SystemManagerInterface(){}

    /**
     * @brief registerSystemManager register system manager handle
     * @param instance
     * @return
     */
    static SystemManagerInterface *registerSystemManager(SystemManagerInterface *instance);

    /**
     * @brief getSystemManager get system manager handle
     * @return
     */
    static SystemManagerInterface *getSystemManager(void);

    // 背光亮度设置/获取。
    virtual void setBrightness(BrightnessLevel br) = 0;
    virtual void enableBrightness(BrightnessLevel br) = 0;
    virtual BrightnessLevel getBrightness(void) = 0;

    /**
     * @brief getCurWorkMode get the current work mode
     * @return the current work mode
     */
    virtual WorkMode getCurWorkMode() const = 0;

    //  查询是否支持该功能。
    virtual bool isSupport(ConfiguredFuncs funcs) const = 0;

    // check whether param is support
    virtual bool isSupport(ParamID paramID) const = 0;
};
