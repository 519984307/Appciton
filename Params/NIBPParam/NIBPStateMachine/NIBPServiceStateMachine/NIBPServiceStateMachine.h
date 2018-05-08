#pragma once
#include "NIBPStateMachine.h"
#include "NIBPRepairMenuManager.h"

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
