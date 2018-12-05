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
#include "SystemManager.h"
#include "IConfig.h"
#include "SoundManager.h"
#include "ECGParam.h"
#include "RunningStatusBar.h"
#include "NIBPParam.h"

NightModeManager *NightModeManager::_selfObj = NULL;

class NightModeManagerPrivate
{
public:
    NightModeManagerPrivate();
    ~NightModeManagerPrivate(){}

    // 显示夜间模式图标
    void setSystemModeBar();
    bool isNightMode;
    int normalScreenBrightness;
    int normalalarmVolume;
    int normalheartBeatVolume;
    int normalkeyVolume;

    /**
     * @brief loadOption    保存正常模式下的值
     */
    void loadOption();
};

NightModeManager::NightModeManager()
    : d_ptr(new NightModeManagerPrivate)
{
}

NightModeManager::NightModeManager(const NightModeManager &handle)
{
    d_ptr->isNightMode = handle.d_ptr->isNightMode;
    d_ptr->normalScreenBrightness = handle.d_ptr->normalScreenBrightness;
    d_ptr->normalalarmVolume = handle.d_ptr->normalalarmVolume;
    d_ptr->normalheartBeatVolume = handle.d_ptr->normalheartBeatVolume;
    d_ptr->normalkeyVolume = handle.d_ptr->normalkeyVolume;
}

NightModeManager::~NightModeManager()
{
    delete d_ptr;
}

void NightModeManager::setNightMode()
{
    int screenBrightness = 0;
    int alarmVolume = 0;
    int heartBeatVolume = 0;
    int keyVolume = 0;
    int enterNightMode = 0;
    systemConfig.getNumValue("NightMode|EnterNightMode", enterNightMode);
    systemConfig.setNumValue("NightMode|EnterNightMode",
                             static_cast<int>(!enterNightMode));
    d_ptr->isNightMode = !enterNightMode;
    if (d_ptr->isNightMode)    //　夜间模式
    {
        // 屏幕亮度(读取夜间模式)
        systemConfig.getNumValue("NightMode|ScreenBrightness", screenBrightness);

        // 报警音量
        systemConfig.getNumValue("NightMode|AlarmVolume", alarmVolume);

        // 心跳音量
        systemConfig.getNumValue("NightMode|HeartBeatVolume", heartBeatVolume);

        // 按键音量
        systemConfig.getNumValue("NightMode|KeyPressVolume", keyVolume);

        d_ptr->loadOption();

        runningStatus.setNightModeStatus(true);

        // NIBP停止测量
        int stopNibpMeasure = 0;
        systemConfig.getNumValue("NightMode|StopNIBPMeasure", stopNibpMeasure);
        if (stopNibpMeasure)
        {
            nibpParam.stopMeasure();
        }
    }
    else
    {
        screenBrightness = d_ptr->normalScreenBrightness;

        alarmVolume = d_ptr->normalalarmVolume;

        heartBeatVolume = d_ptr->normalalarmVolume;

        keyVolume = d_ptr->normalkeyVolume;

        runningStatus.setNightModeStatus(false);
    }

    systemManager.setBrightness(static_cast<BrightnessLevel>(screenBrightness));
    soundManager.setVolume(SoundManager::SOUND_TYPE_ALARM, (SoundManager::VolumeLevel) alarmVolume);
    ecgParam.setQRSToneVolume(static_cast<SoundManager::VolumeLevel>(heartBeatVolume));
    soundManager.setVolume(SoundManager::SOUND_TYPE_KEY_PRESS , (SoundManager::VolumeLevel)keyVolume);
}

bool NightModeManager::isNightMode()
{
    return d_ptr->isNightMode;
}

NightModeManagerPrivate::NightModeManagerPrivate()
{
    int index = 0;
    systemConfig.getNumValue("NightMode|EnterNightMode", index);
    isNightMode = index;
    loadOption();
}

void NightModeManagerPrivate::loadOption()
{
    BrightnessLevel brightnessLevel = systemManager.getBrightness();
    normalScreenBrightness =  static_cast<int>(brightnessLevel);

    SoundManager::VolumeLevel volume =  soundManager.getVolume(SoundManager::SOUND_TYPE_ALARM);
    normalalarmVolume = static_cast<int>(volume);

    volume = soundManager.getVolume(SoundManager::SOUND_TYPE_HEARTBEAT);
    normalheartBeatVolume = static_cast<int>(volume);

    volume = soundManager.getVolume(SoundManager::SOUND_TYPE_KEY_PRESS);
    normalkeyVolume = static_cast<int>(volume);
}
