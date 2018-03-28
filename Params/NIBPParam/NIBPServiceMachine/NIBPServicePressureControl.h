#pragma once
#include "NIBPServiceState.h"

class NIBPServicePressureControl : public NIBPServiceState
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
    virtual void connectError(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPServicePressureControl();
    ~NIBPServicePressureControl();

private:
    bool _isReturn;
    bool _isEnterSuccess;
};
