#include "IConfig.h"
#include <QString>
#include <QMap>
#include "SystemManagerInterface.h"

SystemConfig *SystemConfig::_selfObj = NULL;
MachineConfig *MachineConfig::_selfObj = NULL;
SupervisorConfig *SupervisorConfig::_selfObj = NULL;
SupervisorRunConfig *SupervisorRunConfig::_selfObj = NULL;

//QString SUPERVISOR_RUN_CFG_FILE = CFG_PATH"AdultConfig.xml";

//QString SUPERVISOR_CFG_FILE = CFG_PATH"AdultConfig.xml";

//QString ORGINAL_SUPERVISOR_CFG_FILE = CFG_PATH"AdultConfig.xml";

class MachineConfigPrivate
{
public:
    QMap <QString, bool> modulesStatusMap;
};

void MachineConfig::getModuleInitialStatus(const QString &moduleStr, bool *enable)
{
    if (enable)
    {
        *enable = d_ptr->modulesStatusMap[moduleStr];
    }
}

MachineConfig::MachineConfig()
             : Config(MACHINE_CFG_FILE)
             , d_ptr(new MachineConfigPrivate)
{
    QStringList pathList = QStringList()
            << "RESPEnable"
            << "ECG12LEADEnable"
            << "SPO2Enable"
            << "NIBPEnable"
            << "CO2Enable"
            << "COEnable"
            << "IBPEnable"
            << "TEMPEnable"
            << "WIFIEnable"
            << "TouchEnable"
            << "PrinterEnable"
            << "NIBPNEOMeasureEnable"
            << "O2Enable";

    bool enable;
    QString path;
    foreach(path, pathList)
    {
        enable = false;
        getNumValue(path, enable);
        d_ptr->modulesStatusMap[path] = enable;
    }
}

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
