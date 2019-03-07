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
#include "RunningStatusBar.h"
#include "NIBPParamInterface.h"
#include "SoundManagerInterface.h"
#include "ConfigManagerInterface.h"

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
    SoundManagerInterface::VolumeLevel normalAlarmVolume;
    SoundManagerInterface::VolumeLevel normalHeartBeatVolume;
    SoundManagerInterface::VolumeLevel normalKeyVolume;
    SoundManagerInterface::VolumeLevel normalNibpCompleteVolume;

    /**
     * @brief loadOption    保存正常模式下的值
     */
    void loadOption();

    SoundManagerInterface *soundManager;
    SystemManagerInterface *systemManager;
    NIBPParamInterface *nibpParam;
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
    int notificationVolume = 0;
    int nibpCompleteTone = 0;
    bool isRepetSet = false;
    if (nightMode == d_ptr->isNightMode)
    {
        isRepetSet = true;
    }

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

        d_ptr->loadOption();

        runningStatus.setNightModeStatus(true);

        // NIBP停止测量
        int stopNibpMeasure = 0;
        systemConfig.getNumValue("NightMode|StopNIBPMeasure", stopNibpMeasure);
        if (stopNibpMeasure && !isRepetSet)
        {
            d_ptr->nibpParam->stopMeasure();
        }
    }
    else
    {
        int b = 0;

        screenBrightness = d_ptr->systemManager->getBrightness();

        systemConfig.getNumValue("Alarms|DefaultAlarmVolume", b);
        alarmVolume = b;

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

        runningStatus.setNightModeStatus(false);
    }

    d_ptr->systemManager->enableBrightness(static_cast<BrightnessLevel>(screenBrightness));
    d_ptr->soundManager->setVolume(SoundManagerInterface::SOUND_TYPE_ALARM,
                           static_cast<SoundManagerInterface::VolumeLevel>(alarmVolume));
    d_ptr->soundManager->setVolume(SoundManagerInterface::SOUND_TYPE_HEARTBEAT,
                           static_cast<SoundManagerInterface::VolumeLevel>(heartBeatVolume));
    d_ptr->soundManager->setVolume(SoundManagerInterface::SOUND_TYPE_NOTIFICATION,
                           static_cast<SoundManagerInterface::VolumeLevel>(notificationVolume));
    d_ptr->soundManager->setVolume(SoundManagerInterface::SOUND_TYPE_NIBP_COMPLETE,
                           static_cast<SoundManagerInterface::VolumeLevel>(nibpCompleteTone));
}

bool NightModeManager::nightMode()
{
    return d_ptr->isNightMode;
}

NightModeManagerPrivate::NightModeManagerPrivate()
    : isNightMode(0),
      normalScreenBrightness(BRT_LEVEL_NR),
      normalAlarmVolume(SoundManagerInterface::VOLUME_LEV_NR),
      normalHeartBeatVolume(SoundManagerInterface::VOLUME_LEV_NR),
      normalKeyVolume(SoundManagerInterface::VOLUME_LEV_NR),
      normalNibpCompleteVolume(SoundManagerInterface::VOLUME_LEV_NR),
      soundManager(SoundManagerInterface::getSoundManager()),
      systemManager(SystemManagerInterface::getSystemManager()),
      nibpParam(NIBPParamInterface::getNIBPParam())
{
    int index = 0;
    systemConfig.getNumValue("NightMode|EnterNightMode", index);
    isNightMode = index;
    loadOption();
}

void NightModeManagerPrivate::loadOption()
{
    normalScreenBrightness = systemManager->getBrightness();
    normalAlarmVolume =  soundManager->getVolume(SoundManagerInterface::SOUND_TYPE_ALARM);
    normalHeartBeatVolume = soundManager->getVolume(SoundManagerInterface::SOUND_TYPE_HEARTBEAT);
    normalKeyVolume = soundManager->getVolume(SoundManagerInterface::SOUND_TYPE_NOTIFICATION);
    normalNibpCompleteVolume = soundManager->getVolume(SoundManagerInterface::SOUND_TYPE_NIBP_COMPLETE);
}
