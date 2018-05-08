#pragma once
#include "NIBPState.h"
#include "NIBPServiceStateDefine.h"

class NIBPServiceStandbyState : public NIBPState
{
public:
    // 进入该状态。
    virtual void enter(void);

    virtual void run(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPServiceStandbyState();
    ~NIBPServiceStandbyState();
};
