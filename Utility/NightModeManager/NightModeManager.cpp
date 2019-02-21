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
#include "RunningStatusBar.h"
#include "NIBPParam.h"

NightModeManager *NightModeManager::_selfObj = NULL;

class NightModeManagerPrivate
{
public:
    NightModeManagerPrivate();
    ~NightModeManagerPrivate() {}

    // 显示夜间模式图标
    void setSystemModeBar();
    bool isNightMode;
    BrightnessLevel normalScreenBrightness;
    SoundManager::VolumeLevel normalAlarmVolume;
    SoundManager::VolumeLevel normalHeartBeatVolume;
    SoundManager::VolumeLevel normalKeyVolume;

    /**
     * @brief loadOption    保存正常模式下的值
     */
    void loadOption();
};

NightModeManager::NightModeManager()
    : d_ptr(new NightModeManagerPrivate)
{
}

NightModeManager::~NightModeManager()
{
    delete d_ptr;
}

void NightModeManager::setNightMode(bool nightMode)
{
    int screenBrightness = 0;
    int alarmVolume = 0;
    int heartBeatVolume = 0;
    int keyVolume = 0;
    systemConfig.setNumValue("NightMode|EnterNightMode", static_cast<int>(nightMode));
    d_ptr->isNightMode = nightMode;
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
        int b = 0;

        screenBrightness = systemManager.getBrightness();

        systemConfig.getNumValue("Alarms|DefaultAlarmVolume", b);
        alarmVolume = b;

        currentConfig.getNumValue("ECG|QRSVolume", b);
        heartBeatVolume = b;

        systemConfig.getNumValue("General|KeyPressVolume", b);
        keyVolume = b;

        runningStatus.setNightModeStatus(false);
    }

    systemManager.enableBrightness(static_cast<BrightnessLevel>(screenBrightness));
    soundManager.setVolume(SoundManager::SOUND_TYPE_ALARM,
                           static_cast<SoundManager::VolumeLevel>(alarmVolume));
    soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT,
                           static_cast<SoundManager::VolumeLevel>(heartBeatVolume));
    soundManager.setVolume(SoundManager::SOUND_TYPE_NOTIFICATION,
                           static_cast<SoundManager::VolumeLevel>(keyVolume));
}

bool NightModeManager::nightMode()
{
    return d_ptr->isNightMode;
}

NightModeManagerPrivate::NightModeManagerPrivate()
    : isNightMode(0),
      normalScreenBrightness(BRT_LEVEL_NR),
      normalAlarmVolume(SoundManager::VOLUME_LEV_NR),
      normalHeartBeatVolume(SoundManager::VOLUME_LEV_NR),
      normalKeyVolume(SoundManager::VOLUME_LEV_NR)

{
    int index = 0;
    systemConfig.getNumValue("NightMode|EnterNightMode", index);
    isNightMode = index;
    loadOption();
}

void NightModeManagerPrivate::loadOption()
{
    normalScreenBrightness = systemManager.getBrightness();
    normalAlarmVolume =  soundManager.getVolume(SoundManager::SOUND_TYPE_ALARM);
    normalHeartBeatVolume = soundManager.getVolume(SoundManager::SOUND_TYPE_HEARTBEAT);
    normalKeyVolume = soundManager.getVolume(SoundManager::SOUND_TYPE_NOTIFICATION);
}
