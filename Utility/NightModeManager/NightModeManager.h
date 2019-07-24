/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/19
 **/

#pragma once
#include <QObject>
#include "SystemDefine.h"
#include "SoundManagerInterface.h"

class NightModeManagerPrivate;
class NightModeManager : public QObject
{
    Q_OBJECT
public:
    static NightModeManager &getInstance(void);

    // 复制构造函数
    ~NightModeManager();

    /**
     * @brief setNightMode 切换夜间模式后进行相应参数设置
     * @param nightMode    ０：正常模式　　１：夜间模式
     */
    void setNightMode(bool nightMode);

    /**
     * @brief isNightMode   返回夜间模式状态
     * @return              ０：正常模式　　１：夜间模式
     */
    bool nightMode();

    /**
     * @brief setBrightness 设置夜间模式的背光亮度
     * @param level
     */
    void setBrightness(BrightnessLevel level);

    /**
     * @brief setSoundVolume　设置夜间模式的音量
     * @param soundType
     * @param volumeLevel
     */
    void setSoundVolume(SoundManagerInterface::SoundType soundType, SoundManagerInterface::VolumeLevel volumeLevel);

    /**
     * @brief setNibpStopMeasure 设置夜间模式的NIBP停止测量状态
     * @param stopNibpMeasure
     */
    void setNibpStopMeasure(bool stopNibpMeasure);
signals:
    void nightModeChanged(bool);

private:
    NightModeManager();
    NightModeManager(const NightModeManager &handle);  // stop the cppcheck complain
    NightModeManagerPrivate *d_ptr;
};
#define nightModeManager (NightModeManager::getInstance())
