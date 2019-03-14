/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/


#include "NIBPMonitorStateMachine.h"
#include "NIBPParam.h"
#include "NIBPState.h"
#include "NIBPMonitorStandbyState.h"
#include "NIBPMonitorStartingState.h"
#include "NIBPMonitorMeasureState.h"
#include "NIBPMonitorGetResultState.h"
#include "NIBPMonitorSafeWaitTimeState.h"
#include "NIBPMonitorStopState.h"
#include "NIBPMonitorErrorState.h"
#include "Debug.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMonitorStateMachine::NIBPMonitorStateMachine() : NIBPStateMachine(NIBP_STATE_MACHINE_MONITOR)
{
    // 构造状态对象，并关联状态机和状态。
    NIBPState *state = new NIBPMonitorStandbyState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPMonitorStartingState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPMonitorMeasureState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPMonitorGetResultState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPMonitorSafeWaitTimeState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPMonitorStopState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPMonitorErrorState();
    state->setStateMachine(this);
    registerState(state);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorStateMachine::~NIBPMonitorStateMachine()
{
}

/**************************************************************************************************
 * 状态机进入
 *************************************************************************************************/
void NIBPMonitorStateMachine::enter()
{
    NIBPStateMachine::enter();
    switchToState(NIBP_MONITOR_STANDBY_STATE);
}

void NIBPMonitorStateMachine::exit()
{
    NIBPStateMachine::exit();
}

/**************************************************************************************************
 * 当前状态类型。
 *************************************************************************************************/
int NIBPMonitorStateMachine::curStatusType()
{
    if (NULL != activateState)
    {
        return activateState->getID();
    }
    else
    {
        return NIBPStateMachine::curStatusType();
    }
}
