#include "IConfig.h"
#include <QString>

SystemConfig *SystemConfig::_selfObj = NULL;
MachineConfig *MachineConfig::_selfObj = NULL;
SupervisorConfig *SupervisorConfig::_selfObj = NULL;
SupervisorRunConfig *SupervisorRunConfig::_selfObj = NULL;

//QString SUPERVISOR_RUN_CFG_FILE = CFG_PATH"AdultConfig.xml";

//QString SUPERVISOR_CFG_FILE = CFG_PATH"AdultConfig.xml";

//QString ORGINAL_SUPERVISOR_CFG_FILE = CFG_PATH"AdultConfig.xml";

void SystemConfig::updateCurConfigName()
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
