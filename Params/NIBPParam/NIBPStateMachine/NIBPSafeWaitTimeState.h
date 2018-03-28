#pragma once
#include "NIBPState.h"

class NIBPSafeWaitTime : public NIBPState
{
public:
    virtual void run(void);
    virtual void enter(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPSafeWaitTime();
    ~NIBPSafeWaitTime();

private:
    int _safeWaitTiming;                    //安全间隔倒计时时间
    int _autoCountdownTiming;               //自动倒计时时间
    bool _safeFlag;                         //低于15mmHg标志
    int _safeTime;                          //安全间隔时间
};
