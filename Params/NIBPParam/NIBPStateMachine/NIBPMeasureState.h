#pragma once
#include "NIBPState.h"

class NIBPMeasureState : public NIBPState
{
public:
    virtual void run(void);
    virtual void enter(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPMeasureState();
    ~NIBPMeasureState();
};
