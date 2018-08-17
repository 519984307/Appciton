/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/


#include "LightManager.h"
#include "LightProviderIFace.h"

LightManager *LightManager::_selfObj = NULL;
/**************************************************************************************************
 * 设置提供者。
 *************************************************************************************************/
void LightManager::setProvider(LightProviderIFace *iface)
{
    _provider = iface;
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void LightManager::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    if (NULL != _provider)
    {
        _provider->sendCmdData(cmdId, data, len);
    }
}

/**************************************************************************************************
 * 报警灯控制。
 *************************************************************************************************/
void LightManager::updateAlarm(bool hasAlarmed, AlarmPriority priority)
{
    if (_provider == NULL)
    {
        return;
    }

    _provider->updateAlarm(hasAlarmed, priority);
}

/**************************************************************************************************
 * 报警静音灯控制。
 *************************************************************************************************/
void LightManager::enableAlarmAudioMute(bool enable)
{
    if (_provider == NULL)
    {
        return;
    }

    _provider->enableAlarmAudioMute(enable);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
LightManager::LightManager()
{
    _provider = NULL;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
LightManager::~LightManager()
{
}
