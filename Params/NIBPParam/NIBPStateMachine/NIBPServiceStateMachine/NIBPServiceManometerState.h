#pragma once
#include "NIBPState.h"
#include "NIBPServiceStateDefine.h"

class NIBPServiceManometerState : public NIBPState
{

    #ifdef CONFIG_UNIT_TEST
        friend class TestNIBPParam;
    #endif

public:
    virtual void enter(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPServiceManometerState();
    ~NIBPServiceManometerState();

private:
    bool _isReturn;
    bool _isEnterSuccess;
};
