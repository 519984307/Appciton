#pragma once
#include "NIBPState.h"
#include "NIBPServiceStateDefine.h"

class NIBPServiceErrorState : public NIBPState
{
public:
    virtual void enter(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPServiceErrorState();
    ~NIBPServiceErrorState();
};
