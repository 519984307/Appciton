/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/


#include "NIBPMonitorStartingState.h"
#include "NIBPParam.h"
#include "PatientManager.h"
#include "NIBPCountdownTime.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPMonitorStartingState::enter(void)
{
    // 启动定时器，不能无限期地等待下去。
    // 将定时器上移，相当于在每次测量时初始化定时器；
    setTimeOut();

    //清除显示的数据。
    nibpParam.invResultData();
    nibpParam.setCuffPressure(0);

    //清除标志位
    nibpParam.setSwitchFlagTime(false);
    nibpParam.setSwitchFlagType(false);

//    provider->setPatientType((unsigned char)patientManager.getType());
//    provider->setInitPressure(nibpParam.getInitPressure(patientManager.getType()));

    nibpParam.provider().startMeasure(patientManager.getType());

    // 如果不需要启动应答则立即切换到测量状态。
    if (!nibpParam.provider().needStartACK())
    {
        switchState(NIBP_MONITOR_MEASURE_STATE);
        return;
    }
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPMonitorStartingState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
        switchState(NIBP_MONITOR_STOPE_STATE);
        break;

    case NIBP_EVENT_MODULE_ERROR:
    case NIBP_EVENT_TIMEOUT:
        switchState(NIBP_MONITOR_ERROR_STATE);
        break;

    case NIBP_EVENT_TRIGGER_BUTTON:
        switchState(NIBP_MONITOR_STOPE_STATE);
        break;

    case NIBP_EVENT_TRIGGER_PATIENT_TYPE:
        switchState(NIBP_MONITOR_STOPE_STATE);
        break;

    case NIBP_EVENT_TRIGGER_MODEL:
        if (args[0] == 0x01)
        {
            if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
            {
                switchState(NIBP_MONITOR_STOPE_STATE);
            }
            else
            {
                nibpParam.setSTATMeasure(true);
                nibpCountdownTime.STATMeasureStart();  // 只测量5分钟。
            }
            break;
        }
        if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            nibpParam.switchToAuto();
        }
        else
        {
            nibpParam.switchToManual();
        }
        break;

    case NIBP_EVENT_MONITOR_STOP:
        switchState(NIBP_MONITOR_STOPE_STATE);
        break;

    case NIBP_EVENT_MONITOR_START_MEASURE:
        // 获得应答，切换到测量状态。
        switchState(NIBP_MONITOR_MEASURE_STATE);
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMonitorStartingState::NIBPMonitorStartingState() : NIBPState(NIBP_MONITOR_STARTING_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorStartingState::~NIBPMonitorStartingState()
{
}
