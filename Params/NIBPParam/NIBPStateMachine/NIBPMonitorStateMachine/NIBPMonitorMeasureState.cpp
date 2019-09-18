/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/


#include "NIBPMonitorMeasureState.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "NIBPCountdownTime.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPMonitorMeasureState::enter(void)
{
    setTimeOut();
    nibpParam.setSnapshotFlag(true);
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPMonitorMeasureState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
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
                nibpCountdownTime.STATMeasureStart(); // 只测量5分钟。
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

    case NIBP_EVENT_CURRENT_PRESSURE:
    {
        setTimeOut();
        short pressure = 0;
        pressure = (args[1] << 8)+args[0];
        if (pressure != -1)
        {
            nibpParam.setCuffPressure(pressure);
            return;
        }
    }
        break;

    case NIBP_EVENT_MONITOR_STOP:
        switchState(NIBP_MONITOR_STOPE_STATE);
        break;

    case NIBP_EVENT_MONITOR_MEASURE_DONE:
        switchState(NIBP_MONITOR_GET_RESULT_STATE);
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMonitorMeasureState::NIBPMonitorMeasureState() : NIBPState(NIBP_MONITOR_MEASURE_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorMeasureState::~NIBPMonitorMeasureState()
{
}
