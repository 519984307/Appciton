#pragma once
#include "NIBPState.h"
#include "NIBPMonitorStateDefine.h"

class NIBPMonitorStandbyState : public NIBPState
{
public:
    virtual void enter(void);
    virtual void run(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPMonitorStandbyState();
    ~NIBPMonitorStandbyState();
};
