/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/


#pragma once
#include "AlarmDefine.h"

class LightProviderIFace;
class LightManager
{
public:
    static LightManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new LightManager();
        }
        return *_selfObj;
    }
    static LightManager *_selfObj;
    ~LightManager();

public:
    void setProvider(LightProviderIFace *iface);
    void updateAlarm(bool hasAlarm, AlarmPriority priority);
    void enableAlarmAudioMute(bool enable);

    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    /**
     * @brief stopLightOn  是否停止开启灯光
     * @param isClosed  true--不允许开启灯光  false--允许开启灯光
     */
    void stopLightOn(bool isClosed);

private:
    LightManager();
    LightProviderIFace *_provider;
    bool _isClosed;
};
#define lightManager (LightManager::construction())
#define deleteLightManager() (delete LightManager::_selfObj)
