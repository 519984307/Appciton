/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/7/25
 **/

#include "NIBPServiceStateMachine.h"
#include "NIBPParam.h"
#include "NIBPState.h"
#include "NIBPServiceCalibrateState.h"
#include "NIBPServiceErrorState.h"
#include "NIBPServiceManometerState.h"
#include "NIBPServicePressureControlState.h"
#include "NIBPServiceStandbyState.h"
#include "NIBPServiceZeroPointState.h"
#include "IConfig.h"
#include "Debug.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceStateMachine::NIBPServiceStateMachine() : NIBPStateMachine(NIBP_STATE_MACHINE_SERVICE)
{
    // 构造状态对象，并关联状态机和状态。
    NIBPState *state = new NIBPServiceCalibrateState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPServiceManometerState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPServicePressureControlState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPServiceStandbyState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPServiceZeroPointState();
    state->setStateMachine(this);
    registerState(state);

    state = new NIBPServiceErrorState();
    state->setStateMachine(this);
    registerState(state);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceStateMachine::~NIBPServiceStateMachine()
{
}

/**************************************************************************************************
 * 状态机进入
 *************************************************************************************************/
void NIBPServiceStateMachine::enter()
{
    NIBPStateMachine::enter();

    if (nibpParam.getConnectedState())
    {
        switchToState(NIBP_SERVICE_STANDBY_STATE);
    }
    else
    {
        switchToState(NIBP_SERVICE_ERROR_STATE);
    }
}

void NIBPServiceStateMachine::exit()
{
}

/**************************************************************************************************
 * 当前状态类型。
 *************************************************************************************************/
int NIBPServiceStateMachine::curStatusType()
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


