#pragma once
#include "NIBPState.h"
#include "NIBPMonitorStateDefine.h"

enum NIBPZeroTestState
{
    NIBP_ZERO_ONGOING_STATE = 0x01,
    NIBP_ZERO_SUCCESS_STATE = 0x02,
    NIBP_ZERO_FAIL_STATE = 0x03
};

class NIBPMonitorStandbyState : public NIBPState
{
public:
    virtual void enter(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPMonitorStandbyState();
    ~NIBPMonitorStandbyState();
protected:
    void timerEvent(QTimerEvent *ev);

private:
    int _timerID;
};
