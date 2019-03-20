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

class virtualNIBPMonitorStateMachine : public NIBPStateMachine
{
public:
    virtualNIBPMonitorStateMachine();
    const QString name() {return ("NIBP Monitor State Machine");}
};

class virtualNIBPServiceStateMachine :public NIBPStateMachine
{
public:
    virtualNIBPServiceStateMachine();
    const QString name() {return ("NIBP Service State Machine");}
};
#endif  // VIRTUALNIBPSTATEMACHINE_H
