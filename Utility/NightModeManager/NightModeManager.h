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

class NightModeManagerPrivate;
class NightModeManager
{
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

private:
    NightModeManager();
    NightModeManager(const NightModeManager &handle);  // stop the cppcheck complain
    NightModeManagerPrivate *d_ptr;
};
#define nightModeManager (NightModeManager::getInstance())
