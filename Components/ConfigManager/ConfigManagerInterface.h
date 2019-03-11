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
#include "PatientDefine.h"
#include "Config.h"

class ConfigManagerInterface
{
public:
    virtual ~ConfigManagerInterface(){}

    /**
     * @brief registerConfigManager register config manger
     * @return
     */
    static ConfigManagerInterface *registerConfigManager(ConfigManagerInterface *);

    /**
     * @brief getConfigManager get current config manager handle
     * @return
     */
    static ConfigManagerInterface* getConfigManager(void);

    /**
     * @brief getConfig get a config object of the specific patient type
     * @param patType the paient type, adult, ped or neo
     * @return pointer to the specific patient type config object
     */
    virtual Config *getConfig(PatientType patType) = 0;

    /**
     * @brief getCurConfig get the reference of the current running config
     * @return  reference to the current running config
     */
    virtual Config &getCurConfig() = 0;
};
