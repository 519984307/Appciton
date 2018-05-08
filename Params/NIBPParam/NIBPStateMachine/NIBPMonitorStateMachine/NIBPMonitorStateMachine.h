#pragma once
#include "NIBPStateMachine.h"

class NIBPMonitorStateMachine : public NIBPStateMachine
{
    Q_OBJECT
public:
    // 构造与析构。
    NIBPMonitorStateMachine();
    virtual ~NIBPMonitorStateMachine();

    // 虚拟机进入与退出。
    virtual void enter(void);
    virtual void exit(void);

    // name
    const QString name() {return ("NIBP Monitor State Machine");}

    virtual int curStatusType();
};
