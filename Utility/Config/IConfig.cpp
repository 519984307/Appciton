#include "IConfig.h"
#include <QString>

SystemConfig *SystemConfig::_selfObj = NULL;
MachineConfig *MachineConfig::_selfObj = NULL;
SupervisorConfig *SupervisorConfig::_selfObj = NULL;
SupervisorRunConfig *SupervisorRunConfig::_selfObj = NULL;

//QString SUPERVISOR_RUN_CFG_FILE = CFG_PATH"AdultConfig.xml";

//QString SUPERVISOR_CFG_FILE = CFG_PATH"AdultConfig.xml";

//QString ORGINAL_SUPERVISOR_CFG_FILE = CFG_PATH"AdultConfig.xml";

QString curConfigName = CFG_PATH"AdultConfig.xml";
