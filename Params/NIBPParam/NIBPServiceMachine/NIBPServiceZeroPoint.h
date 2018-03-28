#pragma once
#include "NIBPServiceState.h"

class NIBPServiceZeroPoint : public NIBPServiceState
{
    #ifdef CONFIG_UNIT_TEST
        friend class TestNIBPParam;
    #endif

public:
    virtual void triggerReturn(void);

    virtual void serviceCloseValve(bool enter);
    virtual void servicePressureZero(void);

    virtual void run(void);
    virtual void enter(void);
    virtual void connectError(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPServiceZeroPoint();
    ~NIBPServiceZeroPoint();

private:
    bool _isReturn;
    bool _isEnterSuccess;
};
