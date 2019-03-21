/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by tongzhou fang  fangtongzhou@blmed.cn, 2019/3/20
 **/
#ifndef VIRTUALNIBPSTATEMACHINE_H
#define VIRTUALNIBPSTATEMACHINE_H

#include "NIBPStateMachine.h"
#include "gmock/gmock.h"

class MockNIBPMonitorStateMachine : public NIBPStateMachine
{
public:
    MockNIBPMonitorStateMachine() : NIBPStateMachine(NIBP_STATE_MACHINE_MONITOR){}
    MOCK_METHOD0(name, const QString(void));
};

class MockNIBPServiceStateMachine :public NIBPStateMachine
{
public:
    MockNIBPServiceStateMachine() : NIBPStateMachine(NIBP_STATE_MACHINE_SERVICE) {}
    MOCK_METHOD0(name, const QString(void));
};
#endif
