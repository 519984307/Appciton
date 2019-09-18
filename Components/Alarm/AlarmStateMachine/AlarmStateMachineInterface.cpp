/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/2/28
 **/

#include "AlarmStateMachineInterface.h"
#include "stddef.h"

static AlarmStateMachineInterface *curStateMachine = NULL;

AlarmStateMachineInterface *AlarmStateMachineInterface::registerAlarmStateMachine(AlarmStateMachineInterface *stateMachine)
{
    AlarmStateMachineInterface *oldStateMachine = curStateMachine;
    curStateMachine = stateMachine;
    return oldStateMachine;
}

AlarmStateMachineInterface *AlarmStateMachineInterface::getAlarmStateMachine()
{
    return curStateMachine;
}
