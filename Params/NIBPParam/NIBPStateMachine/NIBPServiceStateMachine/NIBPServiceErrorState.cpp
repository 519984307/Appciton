/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/8/15
 **/


#include "NIBPServiceErrorState.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "NIBPRepairMenuManager.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceErrorState::enter(void)
{
    nibpRepairMenuManager.setMonitorState(NIBP_MONITOR_ERROR_STATE);
    nibpRepairMenuManager.warnShow(true);
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceErrorState::handleNIBPEvent(NIBPEvent /*event*/, const unsigned char */*args*/, int /*argLen*/)
{
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
