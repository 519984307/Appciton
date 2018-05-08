#pragma once
#include "NIBPState.h"
#include "NIBPMonitorStateDefine.h"

class NIBPMonitorMeasureState : public NIBPState
{
public:
    virtual void run(void);
    virtual void enter(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPMonitorMeasureState();
    ~NIBPMonitorMeasureState();
};
