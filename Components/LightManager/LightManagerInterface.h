/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/5
 **/

#pragma once
#include "AlarmDefine.h"

class LightProviderIFace;
class LightManagerInterface
{
public:
    LightManagerInterface() {}
    virtual ~LightManagerInterface(){}

    /**
     * @brief registerLightManager register light manager
     * @return
     */
    static LightManagerInterface *registerLightManager(LightManagerInterface *);

    /**
     * @brief getLightManager get light manager handle
     * @return
     */
    static LightManagerInterface *getLightManager(void);

    /**
     * @brief setProvider 设置灯光接口
     * @param iface
     */
    virtual void setProvider(LightProviderIFace *iface) = 0;

    /**
     * @brief updateAlarm 更新报警
     * @param hasAlarm
     * @param priority
     */
    virtual void updateAlarm(bool hasAlarm, AlarmPriority priority) = 0;

    /**
     * @brief enableAlarmAudioMute 使能报警音
     * @param enable
     */
    virtual void enableAlarmAudioMute(bool enable) = 0;

    /**
     * @brief sendCmdData 发送协议命令
     * @param cmdId
     * @param data
     * @param len
     */
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len) = 0;

    /**
     * @brief stopHandlingLight  是否停止处理灯光
     * @param enable  true--不允许开启灯光  false--允许开启灯光
     */
    virtual void stopHandlingLight(bool enable) = 0;
};
