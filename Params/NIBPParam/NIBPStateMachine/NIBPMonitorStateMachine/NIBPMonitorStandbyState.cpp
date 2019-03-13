/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/


#include "NIBPMonitorStandbyState.h"
#include "NIBPParam.h"
#include "NIBPCountdownTime.h"

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPMonitorStandbyState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_ERROR:
        switchState(NIBP_MONITOR_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
        switchState(NIBP_MONITOR_ERROR_STATE);
        break;

    case NIBP_EVENT_TRIGGER_BUTTON:
        if (nibpParam.getMeasurMode() == NIBP_MODE_AUTO)
        {
            //自动测量模式的手动触发标志
            nibpParam.setAutoMeasure(true);
        }
        // 转换到测量状态。
        switchState(NIBP_MONITOR_STARTING_STATE);
        break;

    case NIBP_EVENT_TRIGGER_MODEL:
        if (args[0] == 0x01)
        {
            nibpParam.setSTATMeasure(true);
            nibpCountdownTime.STATMeasureStart();// 只测量5分钟。
            switchState(NIBP_MONITOR_STARTING_STATE);
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

    default:
        break;
    }
}

/**************************************************************************************************
 * 状态进入
 *************************************************************************************************/
void NIBPMonitorStandbyState::enter()
{
    if (nibpParam.getMeasurMode() == NIBP_MODE_AUTO)
    {
        nibpParam.setAutoMeasure(false);
        nibpParam.setModelText(trs("NIBPAUTO") + ":" +
                               trs(NIBPSymbol::convert((NIBPAutoInterval)nibpParam.getAutoInterval())));
    }
    else if (nibpParam.getMeasurMode() == NIBP_MODE_MANUAL)
    {
        nibpParam.setModelText(trs("NIBPManual"));
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMonitorStandbyState::NIBPMonitorStandbyState() : NIBPState(NIBP_MONITOR_STANDBY_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorStandbyState::~NIBPMonitorStandbyState()
{
}
