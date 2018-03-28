#pragma once
#include "NIBPState.h"

class NIBPGetResultState : public NIBPState
{
public:
    virtual void run(void);
    virtual void enter(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPGetResultState();
    ~NIBPGetResultState();

private:
    bool same_time_flag;                                   //同一状态标志
};
