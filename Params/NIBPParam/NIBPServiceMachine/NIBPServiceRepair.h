#pragma once
#include "NIBPServiceState.h"

class NIBPServiceRepair : public NIBPServiceState
{
public:
    virtual void triggerEnter(void);
    virtual void triggerReturn(void);
    // 进入该状态。
    virtual void enter(void) { }

    virtual void run(void);
    virtual void connectError(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPServiceRepair();
    ~NIBPServiceRepair();
};
