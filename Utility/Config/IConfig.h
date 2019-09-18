/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/18
 **/

#pragma once
#include "Config.h"
#include "ConfigDefine.h"

class SystemConfig : public Config
{

public:
    static SystemConfig &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SystemConfig();
        }
        return *_selfObj;
    }
    static SystemConfig *_selfObj;

    ~SystemConfig()
    {
    }

    const QString &getCurConfigName(){return curConfigName;}

    void updateCurConfigName();

private:
    SystemConfig() : Config(SYSTEM_CFG_FILE)
    {
        updateCurConfigName();
    }
    QString curConfigName;
};
#define systemConfig (SystemConfig::construction())
#define deleteSystemConfig() (delete SystemConfig::_selfObj)


///////////////////////////////////
class MachineConfigPrivate;
class MachineConfig : public Config
{
public:
    static MachineConfig &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new MachineConfig();
        }
        return *_selfObj;
    }
    static MachineConfig *_selfObj;

    ~MachineConfig()
    {
    }

    /**
     * @brief getModuleInitialStatus - get the module initial status
     * @param moduleStr: the module index string
     * @param enable: whether it is enable
     */
    void getModuleInitialStatus(const QString &moduleStr, bool *enable);

private:
    MachineConfig();
    MachineConfigPrivate *const d_ptr;
};
#define machineConfig (MachineConfig::construction())
#define deleteMachineConfig() (delete MachineConfig::_selfObj)

//////////////////////////////////
class SupervisorConfig : public Config
{
public:
    static SupervisorConfig &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SupervisorConfig();
        }
        return *_selfObj;
    }
    static SupervisorConfig *_selfObj;

    ~SupervisorConfig()
    {
    }

private:
    SupervisorConfig() : Config(systemConfig.getCurConfigName())
    {
    }
};
#define superConfig (SupervisorConfig::construction())
#define deleteSuperConfig() (delete SupervisorConfig::_selfObj)

//////////////////////////////////
class SupervisorRunConfig : public Config
{
public:
    static SupervisorRunConfig &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SupervisorRunConfig();
        }
        return *_selfObj;
    }
    static SupervisorRunConfig *_selfObj;

    ~SupervisorRunConfig()
    {
    }

private:
    SupervisorRunConfig() : Config(systemConfig.getCurConfigName())
    {
    }
};
#define superRunConfig (SupervisorRunConfig::construction())
#define deleteSuperRunConfig() (delete SupervisorRunConfig::_selfObj)

