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
#include "LightManagerInterface.h"

class LightProviderIFace;
class LightManager : public LightManagerInterface
{
public:
    ~LightManager();

    static LightManager &getInstance(void);

public:
    void setProvider(LightProviderIFace *iface);
    void updateAlarm(bool hasAlarm, AlarmPriority priority);
    /**
     * @brief enableKeyboardBacklight  ebable the backlight of the Keyboard
     * @param enable
     */
    void enableKeyboardBacklight(bool enable);

    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    /**
     * @brief stopHandlingLight  是否停止处理灯光
     * @param enable  true--不允许开启灯光  false--允许开启灯光
     */
    void stopHandlingLight(bool enable);

private:
    LightManager();
    LightProviderIFace *_provider;
    bool _stopHandlingLight;
};
#define lightManager (LightManager::getInstance())
