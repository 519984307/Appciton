#pragma once
#include "NIBPState.h"

class NIBPStandbyState : public NIBPState
{
public:
    virtual void enter(void);
    virtual void run(void);

    NIBPStandbyState();
    ~NIBPStandbyState();
};
