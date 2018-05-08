#pragma once
#include "NIBPState.h"
#include "NIBPServiceStateDefine.h"

class NIBPServiceCalibrateState : public NIBPState
{
    #ifdef CONFIG_UNIT_TEST
        friend class TestNIBPParam;
    #endif
public:
    virtual void run(void);
    virtual void enter(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPServiceCalibrateState();
    ~NIBPServiceCalibrateState();

private:
    bool _isReturn;
    bool _isEnterSuccess;
};
