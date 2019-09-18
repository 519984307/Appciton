/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/4/2
 **/

#pragma once
#include "AlarmDefine.h"

class NurseCallManagerInterface
{
public:
    NurseCallManagerInterface(){}
    virtual ~NurseCallManagerInterface(){}

    static NurseCallManagerInterface *registerNurseCallManagerInterface(NurseCallManagerInterface *interface);

    static NurseCallManagerInterface *getNurseCallManagerInterface();

    /**
     * @brief nurseCall 呼叫护士连续信号
     */
    virtual void callNurse(AlarmType type, AlarmPriority prio, bool alarmSta) = 0;

    /**
     * @brief callNurse 呼叫护士脉冲信号
     */
    virtual void callNurse(AlarmType type, AlarmPriority prio) = 0;

    /**
     * @brief upDateCallSta 刷新当前呼叫状态
     */
    virtual void upDateCallSta() = 0;
};
