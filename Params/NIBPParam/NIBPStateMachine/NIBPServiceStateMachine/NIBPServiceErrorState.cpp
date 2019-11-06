/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/10/16
 **/


#include "NIBPServiceErrorState.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "NIBPMaintainMgrInterface.h"
#include "Framework/ErrorLog/ErrorLog.h"
#include "Framework/ErrorLog/ErrorLogItem.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceErrorState::enter(void)
{
    NIBPMaintainMgrInterface *nibpMaintainMgr;
    nibpMaintainMgr = NIBPMaintainMgrInterface::getNIBPMaintainMgr();
    if (nibpMaintainMgr)
    {
        nibpMaintainMgr->setMonitorState(NIBP_MONITOR_ERROR_STATE);
        nibpMaintainMgr->warnShow(true);
    }
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceErrorState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*arglen*/)
{
    switch (event)
    {
    case NIBP_EVENT_CURRENT_PRESSURE:
    {
        int pressure;
        pressure = (args[1] << 8) + args[0];
        if (pressure != -1)
        {
            nibpParam.setManometerPressure(pressure);
            return;
        }
    }
        break;
    case NIBP_EVENT_CONNECTION_NORMAL:
        switchState(NIBP_SERVICE_STANDBY_STATE);
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceErrorState::NIBPServiceErrorState() : NIBPState(NIBP_SERVICE_ERROR_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceErrorState::~NIBPServiceErrorState()
{
}
