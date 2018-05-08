#pragma once
#include "NIBPState.h"
#include "NIBPMonitorStateDefine.h"

class NIBPMonitorStopState : public NIBPState
{
public:
    virtual void run(void);
    virtual void enter(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPMonitorStopState();
    ~NIBPMonitorStopState();
};
