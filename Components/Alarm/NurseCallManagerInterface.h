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

class NurseCallManagerInterface
{
public:
    NurseCallManagerInterface(){}
    virtual ~NurseCallManagerInterface(){}

    static NurseCallManagerInterface *registerNurseCallManagerInterface(NurseCallManagerInterface *interface);

    static NurseCallManagerInterface *getNurseCallManagerInterface();

    /**
     * @brief getAlarmLevelHigh 高级报警触发呼叫状态
     * @return
     */
    virtual bool getAlarmLevelHigh() = 0;

    /**
     * @brief getAlarmLevelMed 中级报警触发呼叫状态
     * @return
     */
    virtual bool getAlarmLevelMed() = 0;

    /**
     * @brief getAlarmLevelLow 低级报警触发呼叫状态
     * @return
     */
    virtual bool getAlarmLevelLow() = 0;

    /**
     * @brief getAlarmTypeTech 技术报警触发呼叫状态
     * @return
     */
    virtual bool getAlarmTypeTech() = 0;

    /**
     * @brief getAlarmTypePhy 生理报警触发呼叫状态
     * @return
     */
    virtual bool getAlarmTypePhy() = 0;

    /**
     * @brief nurseCall 呼叫护士
     */
    virtual void callNurse(bool signalSta) = 0;
};
