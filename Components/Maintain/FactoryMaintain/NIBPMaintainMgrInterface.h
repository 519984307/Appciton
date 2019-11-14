/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/10/30
 **/

#pragma once
#include "NIBPMonitorStateDefine.h"

class NIBPMaintainMgrInterface
{
public:
    /* destructor */
    virtual ~NIBPMaintainMgrInterface(){}

    /**
     * @brief getNIBPMaintainMgr get the NIBP maintain manager
     * @return  pointer to the NIBP Maintain Manager
     */
    static NIBPMaintainMgrInterface *getNIBPMaintainMgr();

    /**
     * @brief unPacket  进入模式的应答
     * @param flag
     */
    virtual void unPacket(bool flag) = 0;

    /**
     * @brief getRepairError 获取维护模式是否错误
     * @return
     */
    virtual bool getRepairError(void) = 0;

    /**
     * @brief setMonitorState record the state before enter maintaince
     * @param id
     */
    virtual void setMonitorState(NIBPMonitorStateID id) = 0;

    /**
     * @brief getMonitorState get the state before enter maintaince
     * @return
     */
    virtual NIBPMonitorStateID getMonitorState(void) = 0;

    /**
     * @brief messageBox message box
     */
    virtual void messageBox(void) = 0;

    /**
     * @brief warnShow warn message
     * @param enable
     */
    virtual void warnShow(bool enable) = 0;
};
