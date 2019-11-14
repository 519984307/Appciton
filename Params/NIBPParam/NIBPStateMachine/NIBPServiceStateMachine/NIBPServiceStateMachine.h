/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/10/30
 **/

#pragma once
#include "NIBPStateMachine.h"

class NIBPServiceStateMachine : public NIBPStateMachine
{
    Q_OBJECT
public:
    // 构造与析构。
    NIBPServiceStateMachine();
    virtual ~NIBPServiceStateMachine();

    virtual void enter();
    virtual void exit();
    // name
    const QString name() {return ("NIBP Service State Machine");}

    virtual int curStatusType();

protected:
    // 当前活跃的状态。
    NIBPState *activateState;
};
