/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/4
 **/


#pragma once
#include <QString>

// 存放配置文件的位置。
#define CFG_PATH                  "/usr/local/nPM/etc/"

// 机器配置。
#define MACHINE_CFG_FILE          CFG_PATH"Machine.xml"

// 系统配置文件。
#define SYSTEM_CFG_FILE           CFG_PATH"System.xml"

// 默认系统配置文件。
#define ORGINAL_SYSTEM_CFG_FILE      CFG_PATH"System.Original.xml"

// 工厂配置
#define FACTORY_SUPERVISOR_CFG_FILE       CFG_PATH"FactorySupervision.xml"
