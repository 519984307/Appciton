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

    QString getCurConfigName(){return curConfigName;}

private:
    SystemConfig() : Config(SYSTEM_CFG_FILE)
    {
        int numValue = 255;
        getNumValue("General|PatientType", numValue);
        switch(numValue)
        {
        case 0:
            curConfigName = CFG_PATH"AdultConfig.xml";
            break;
        case 1:
            curConfigName = CFG_PATH"PedConfig.xml";
            break;
        case 2:
            curConfigName = CFG_PATH"NeoConfig.xml";
            break;
        }
        if(numValue>2)
        {
            curConfigName = CFG_PATH"AdultConfig.xml";
            numValue = 255;
        }
    }
    QString curConfigName;
};
#define systemConfig (SystemConfig::construction())
#define deleteSystemConfig() (delete SystemConfig::_selfObj)


///////////////////////////////////
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

private:
    MachineConfig() : Config(MACHINE_CFG_FILE)
    {
    }
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

