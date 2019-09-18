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
#include <AlarmDefine.h>

class SoundManagerInterface
{
public:
    enum SoundType
    {
        SOUND_TYPE_NONE,        /* no sound playing */
        SOUND_TYPE_NOTIFICATION,
        SOUND_TYPE_ERROR,
        SOUND_TYPE_PULSE,
        SOUND_TYPE_HEARTBEAT,
        SOUND_TYPE_NIBP_COMPLETE,
        SOUND_TYPE_ALARM,
        SOUND_TYPE_NR,
    };

    enum VolumeLevel
    {
        VOLUME_LEV_0,   /* muted */
        VOLUME_LEV_1,
        VOLUME_LEV_2,
        VOLUME_LEV_3,
        VOLUME_LEV_4,
        VOLUME_LEV_5,
        VOLUME_LEV_MAX = VOLUME_LEV_5,
        VOLUME_LEV_NR,
    };

    virtual ~SoundManagerInterface(){}

    static SoundManagerInterface *registerSoundManager(SoundManagerInterface *);

    static SoundManagerInterface *getSoundManager(void);

    /**
     * @brief setVolume set the volumn level of specific sound type
     * @param type sound type
     * @param lev volumn level
     */
    virtual void setVolume(SoundType type, VolumeLevel lev) = 0;

    /**
     * @brief getVolume get the volumn of specific sound type
     * @param type sound type
     * @return  the volumn level
     */
    virtual VolumeLevel getVolume(SoundType type) = 0;

    virtual void updateAlarm(bool hasAlarm, AlarmPriority curHighestPriority = ALARM_PRIO_PROMPT) = 0;
};
