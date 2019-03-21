/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/12
 **/



#include "NIBPStateMachine.h"
#include "NIBPMonitorStateDefine.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPState.h"
#include "NIBPParamInterface.h"

/**************************************************************************************************
 * 启动定时器。
 *************************************************************************************************/
void NIBPStateMachine::beginTimer(int interval)
{
    // 保证只有一个timer。
    if (_timerID == 0)
    {
        _timerID = startTimer(interval);
    }
}

/**************************************************************************************************
 * 停止定时器。
 *************************************************************************************************/
void NIBPStateMachine::endTimer(void)
{
    if (0 != _timerID)
    {
        killTimer(_timerID);
        _timerID = 0;
    }
}

/**************************************************************************************************
 * 注册状态对象。
 *************************************************************************************************/
void NIBPStateMachine::registerState(NIBPState *state)
{
    _nibpStates.insert(state->getID(), state);
}

/**************************************************************************************************
 * 获取一个状态。
 *************************************************************************************************/
NIBPState *NIBPStateMachine::getState(unsigned char stateID)
{
    QMap<unsigned char, NIBPState *>::ConstIterator it = _nibpStates.find(stateID);
    if (it == _nibpStates.end())
    {
        return NULL;
    }

    return it.value();
}

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPStateMachine::run(void)
{
}

/**************************************************************************************************
 * 重启复位。
 *************************************************************************************************/
void NIBPStateMachine::reset(void)
{
    exit();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPStateMachine::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen)
{
    NIBPParamInterface* nibpParam = NIBPParamInterface::getNIBPParam();
    if (nibpParam)
    {
        switch (event)
        {
        case NIBP_EVENT_MODULE_RESET:
            nibpParam->setText(InvStr());
            break;

        case NIBP_EVENT_TRIGGER_PATIENT_TYPE:
            // 每个状态需要setText
            // 切换病人类型就清除结果
            nibpParam->clearResult();
            nibpParam->setText(InvStr());
            break;

        default:
            break;
        }
    }
    if (activateState)
    {
        activateState->handleNIBPEvent(event, args, argLen);
    }
}

/**************************************************************************************************
 * 进入状态机。
 *************************************************************************************************/
void NIBPStateMachine::enter(void)
{
    _isExited = false;
}

/**************************************************************************************************
 * 退出状态机。
 *************************************************************************************************/
void NIBPStateMachine::exit(void)
{
    _isExited = true;
    if (activateState)
    {
        activateState->exit();
        activateState->stateMachineExit();
        if (_isExited)  // the state might be reentery while waiting in stateMachineExit eventloop
        {
            activateState = NULL;
        }
    }
}

/**************************************************************************************************
 * 切换状态。
 *************************************************************************************************/
void NIBPStateMachine::switchToState(unsigned char newStateID)
{
    NIBPState *state = getState(newStateID);
    if (state != NULL)
    {
        if (NULL != activateState)
        {
            activateState->exit();
        }

        activateState = state;
        state->enter();
    }
}

/**************************************************************************************************
 * 切换状态。
 *************************************************************************************************/
void NIBPStateMachine::switchToState(unsigned char newStateID, void *arg)
{
    NIBPState *state = getState(newStateID);
    if (state != NULL)
    {
        if (NULL != activateState)
        {
            activateState->exit();
        }

        activateState = state;
        state->enter(arg);
    }
}

/**************************************************************************************************
 * 状态对象退出。
 *************************************************************************************************/
void NIBPStateMachine::stateExit(unsigned char /*stateID*/, void */*arg*/)
{
}

/**************************************************************************************************
 * 状态对象退出。
 *************************************************************************************************/
void NIBPStateMachine::stateExit(unsigned char /*stateID*/)
{
}

/**************************************************************************************************
 * 状态对象退出,退出当前状态。
 *************************************************************************************************/
void NIBPStateMachine::stateExit(void)
{
    if (NULL != activateState)
    {
        activateState->exit();
        activateState = NULL;
    }
}

bool NIBPStateMachine::isExit() const
{
    return _isExited;
}

/**************************************************************************************************
 * 获取状态机的类型。
 *************************************************************************************************/
NIBPStateMachineType NIBPStateMachine::type(void) const
{
    return _type;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPStateMachine::NIBPStateMachine(NIBPStateMachineType type)
{
    _type = type;
    activateState = NULL;
    _timerID = 0;
    _isExited = true;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPStateMachine::~NIBPStateMachine()
{
    // 删除注册的对象。
    QMap<unsigned char, NIBPState *>::ConstIterator it = _nibpStates.begin();
    for (; it != _nibpStates.end(); ++it)
    {
        delete it.value();
    }
}
