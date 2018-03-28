#pragma once
#include "NIBPState.h"

class NIBPStopState : public NIBPState
{
public:
    virtual void run(void);
    virtual void enter(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPStopState();
    ~NIBPStopState();
};
