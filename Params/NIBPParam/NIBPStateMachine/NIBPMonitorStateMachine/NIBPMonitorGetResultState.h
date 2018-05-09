#pragma once
#include "NIBPState.h"
#include "NIBPMonitorStateDefine.h"

class NIBPMonitorGetResultState : public NIBPState
{
public:
    virtual void enter(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPMonitorGetResultState();
    ~NIBPMonitorGetResultState();

private:
    bool same_time_flag;                                   //同一状态标志
};
