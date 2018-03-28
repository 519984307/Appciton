#pragma once
#include "NIBPState.h"

class NIBPErrorState : public NIBPState
{
public:
    virtual void run(void);
    virtual void enter(void);

    NIBPErrorState();
    ~NIBPErrorState();
};
