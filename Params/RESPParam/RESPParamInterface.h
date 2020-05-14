/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/28
 **/

#pragma once
#include "RESPProviderIFace.h"

class RESPParamInterface
{
public:
    ~RESPParamInterface() {}

    /**
     * @brief setProvider set the provider
     * @param provider
     */
    virtual void setProvider(RESPProviderIFace *provider) = 0;

    /**
     * @brief addWaveformData add waveform data
     * @param wave the wave value
     * @param flag the wave flag
     */
    virtual void addWaveformData(int wave, int flag) = 0;

    /**
     * @brief setRR set the RR value
     * @param rrValue the new rr value
     */
    virtual void setRR(short rrValue) = 0;

    /**
     * @brief setLeadoff set the leadOff alarm
     * @param flag lead status
     */
    virtual void setLeadoff(bool flag, bool isFirsConnect) = 0;

    /**
     * @brief setOneShotAlarm set the oneshot alarm
     * @param type one shot alarm type
     * @param status the alarm status
     */
    virtual void setOneShotAlarm(RESPOneShotType type, bool status) = 0;

    /**
     * @brief reset reset the module
     */
    virtual void reset() = 0;
};
