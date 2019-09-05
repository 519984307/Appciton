/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/9/5
 **/

#include "IConfig.h"
#include <QString>
#include <QMap>

SystemConfig *SystemConfig::_selfObj = NULL;
MachineConfig *MachineConfig::_selfObj = NULL;
SupervisorConfig *SupervisorConfig::_selfObj = NULL;
SupervisorRunConfig *SupervisorRunConfig::_selfObj = NULL;

// QString SUPERVISOR_RUN_CFG_FILE = CFG_PATH"AdultConfig.xml";

// QString SUPERVISOR_CFG_FILE = CFG_PATH"AdultConfig.xml";

// QString ORGINAL_SUPERVISOR_CFG_FILE = CFG_PATH"AdultConfig.xml";

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
    QString modulesFuncStr;
    getStrValue("ConfiguredFunctions", modulesFuncStr);
    QStringList pathList = modulesFuncStr.split(',');
    QString path;

    foreach(path, pathList)
    {
        bool enable = false;
        path += QString("Enable");
        getNumValue(path, enable);
        d_ptr->modulesStatusMap[path] = enable;
    }
}

void SystemConfig::updateCurConfigName()
{
    int numValue = 255;
    getNumValue("General|PatientType", numValue);
    switch (numValue)
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
    if (numValue > 2)
    {
        curConfigName = CFG_PATH"AdultConfig.xml";
        numValue = 255;
    }
}
