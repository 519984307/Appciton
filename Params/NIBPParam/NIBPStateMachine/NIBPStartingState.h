#pragma once
#include "NIBPState.h"

class NIBPStartingState : public NIBPState
{
public:
    virtual void run(void);
    virtual void enter(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPStartingState();
    ~NIBPStartingState();

private:
    bool _isWaiting;
};
