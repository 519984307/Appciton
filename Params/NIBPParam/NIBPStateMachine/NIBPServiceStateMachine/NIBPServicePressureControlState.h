#pragma once
#include "NIBPState.h"
#include "NIBPServiceStateDefine.h"

class NIBPServicePressureControlState : public NIBPState
{

    #ifdef CONFIG_UNIT_TEST
        friend class TestNIBPParam;
    #endif
public:
    virtual void triggerReturn(void);

    virtual void servicePressureInflate(int Value);
    virtual void servicePressureDeflate(void);
    virtual void servicePatientType(unsigned char type);
    virtual void servicePressureProtect(bool enter);

    virtual void run(void);
    virtual void enter(void);
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    NIBPServicePressureControlState();
    ~NIBPServicePressureControlState();

private:
    bool _isReturn;
    bool _isEnterSuccess;
};
