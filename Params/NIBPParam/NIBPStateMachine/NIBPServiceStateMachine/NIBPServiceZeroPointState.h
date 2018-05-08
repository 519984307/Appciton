#pragma once
#include "NIBPState.h"
#include "NIBPServiceStateDefine.h"

class NIBPServiceZeroPointState : public NIBPState
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
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPServiceZeroPointState();
    ~NIBPServiceZeroPointState();

private:
    bool _isReturn;
    bool _isEnterSuccess;
};
