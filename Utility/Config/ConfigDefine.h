#pragma once
#include <QString>
#include "ConfigManager.h"
// 存放配置文件的位置。
#define CFG_PATH                  "/usr/local/nPM/etc/"

// 机器配置。
#define MACHINE_CFG_FILE          CFG_PATH"Machine.xml"

// 系统配置文件。
#define SYSTEM_CFG_FILE           CFG_PATH"System.xml"

// 默认系统配置文件。
#define ORGINAL_SYSTEM_CFG_FILE      CFG_PATH"System.Original.xml"

extern    QString      curConfigName;

// 工厂配置
#define FACTORY_SUPERVISOR_CFG_FILE       CFG_PATH"FactorySupervision.xml"
