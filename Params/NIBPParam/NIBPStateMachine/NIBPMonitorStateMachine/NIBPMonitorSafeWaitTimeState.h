#pragma once
#include "NIBPState.h"
#include "NIBPMonitorStateDefine.h"

class NIBPMonitorSafeWaitTimeState : public NIBPState
{
public:
    virtual void run(void);
    virtual void enter(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPMonitorSafeWaitTimeState();
    ~NIBPMonitorSafeWaitTimeState();

private:
    int _safeWaitTiming;                    //安全间隔倒计时时间
    int _autoCountdownTiming;               //自动倒计时时间
    bool _safeFlag;                         //低于15mmHg标志
    int _safeTime;                          //安全间隔时间

    int _safeWaitTime();                    //获取安全间隔的时间
};
