/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/19
 **/

#include "NightModeManager.h"
#include "SystemManagerInterface.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "RunningStatusBarInterface.h"
#include "NIBPParamInterface.h"
#include "ConfigManagerInterface.h"

class NightModeManagerPrivate
{
public:
    NightModeManagerPrivate();
    ~NightModeManagerPrivate() {}

    // 显示夜间模式图标
    void setSystemModeBar();
    bool isNightMode;
};

NightModeManager::NightModeManager()
    : d_ptr(new NightModeManagerPrivate)
{
}

NightModeManager &NightModeManager::getInstance()
{
    static NightModeManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new NightModeManager();
    }
    return *instance;
}

NightModeManager::~NightModeManager()
{
    delete d_ptr;
}

void NightModeManager::setNightMode(bool nightMode)
{
    int screenBrightness = 0;
    int heartBeatVolume = 0;
    int notificationVolume = 0;
    int nibpCompleteTone = 0;
    bool isRepetSet = false;
    if (nightMode == d_ptr->isNightMode)
    {
        isRepetSet = true;
    }

    RunningStatusBarInterface *runningStatus = RunningStatusBarInterface::getRunningStatusBar();
    systemConfig.setNumValue("NightMode|EnterNightMode", static_cast<int>(nightMode));
    d_ptr->isNightMode = nightMode;
    if (d_ptr->isNightMode)    //　夜间模式
    {
        // 屏幕亮度(读取夜间模式)
        systemConfig.getNumValue("NightMode|ScreenBrightness", screenBrightness);

        // 心跳音量
        systemConfig.getNumValue("NightMode|HeartBeatVolume", heartBeatVolume);

        // 按键音量
        systemConfig.getNumValue("NightMode|KeyPressVolume", notificationVolume);

        // NIBP完成音
        int index = 0;
        systemConfig.getNumValue("NightMode|NIBPCompletedTips", index);
        if (index)
        {
            nibpCompleteTone = notificationVolume;
        }
        else
        {
            nibpCompleteTone = 0;
        }


        if (runningStatus)
        {
            runningStatus->setNightModeStatus(true);
        }

        // NIBP停止测量
        int stopNibpMeasure = 0;
        systemConfig.getNumValue("NightMode|StopNIBPMeasure", stopNibpMeasure);
        if (stopNibpMeasure && !isRepetSet)
        {
            NIBPParamInterface *nibpParam = NIBPParamInterface::getNIBPParam();
            nibpParam->stopMeasure();
            if (nibpParam->isSTATMeasure())
            {
                nibpParam->setSTATMeasure(false);
            }
        }
    }
    else
    {
        int b = 0;
        SystemManagerInterface *systemManager = SystemManagerInterface::getSystemManager();
        screenBrightness = systemManager->getBrightness();

        ConfigManagerInterface *configManagers = ConfigManagerInterface::getConfigManager();
        configManagers->getCurConfig().getNumValue("ECG|QRSVolume", b);
        heartBeatVolume = b;

        systemConfig.getNumValue("General|KeyPressVolume", b);
        notificationVolume = b;

        systemConfig.getNumValue("PrimaryCfg|NIBP|CompleteTone", b);
        if (b)
        {
            nibpCompleteTone = notificationVolume;
        }
        else
        {
            nibpCompleteTone = 0;
        }

        if (runningStatus)
        {
            runningStatus->setNightModeStatus(false);
        }
    }

    BrightnessLevel brightness = static_cast<BrightnessLevel>(screenBrightness);
    SystemManagerInterface *systemManager = SystemManagerInterface::getSystemManager();
    systemManager->enableBrightness(brightness);
    SoundManagerInterface *soundManager = SoundManagerInterface::getSoundManager();
    soundManager->setVolume(SoundManagerInterface::SOUND_TYPE_HEARTBEAT,
                           static_cast<SoundManagerInterface::VolumeLevel>(heartBeatVolume));
    soundManager->setVolume(SoundManagerInterface::SOUND_TYPE_NOTIFICATION,
                           static_cast<SoundManagerInterface::VolumeLevel>(notificationVolume));
    soundManager->setVolume(SoundManagerInterface::SOUND_TYPE_NIBP_COMPLETE,
                           static_cast<SoundManagerInterface::VolumeLevel>(nibpCompleteTone));

    emit nightModeChanged(d_ptr->isNightMode);
}

bool NightModeManager::nightMode()
{
    return d_ptr->isNightMode;
}

void NightModeManager::setBrightness(BrightnessLevel level)
{
    if (d_ptr->isNightMode)
    {
        SystemManagerInterface *systemManager = SystemManagerInterface::getSystemManager();
        systemManager->enableBrightness(level);
    }
}

void NightModeManager::setSoundVolume(SoundManagerInterface::SoundType soundType,
                                      SoundManagerInterface::VolumeLevel volumeLevel)
{
    if (d_ptr->isNightMode)
    {
        SoundManagerInterface *soundManager = SoundManagerInterface::getSoundManager();
        soundManager->setVolume(soundType, static_cast<SoundManagerInterface::VolumeLevel>(volumeLevel));
    }
}

void NightModeManager::setNibpStopMeasure(bool stopNibpMeasure)
{
    if (d_ptr->isNightMode)
    {
        if (stopNibpMeasure)
        {
            NIBPParamInterface *nibpParam = NIBPParamInterface::getNIBPParam();
            nibpParam->stopMeasure();
            if (nibpParam->isSTATMeasure())
            {
                nibpParam->setSTATMeasure(false);
            }
        }
    }
}

NightModeManagerPrivate::NightModeManagerPrivate()
    : isNightMode(0)
{
    // By default, it does not enter night mode.
//    int index = 0;
//    systemConfig.getNumValue("NightMode|EnterNightMode", index);
//    isNightMode = index;
}
