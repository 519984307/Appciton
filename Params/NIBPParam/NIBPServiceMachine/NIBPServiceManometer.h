#pragma once
#include "NIBPServiceState.h"

class NIBPServiceManometer : public NIBPServiceState
{

    #ifdef CONFIG_UNIT_TEST
        friend class TestNIBPParam;
    #endif

public:
    virtual void triggerReturn(void);

    virtual void run(void);
    virtual void enter(void);
    virtual void connectError(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPServiceManometer();
    ~NIBPServiceManometer();

private:
    bool _isReturn;
    bool _isEnterSuccess;
};
