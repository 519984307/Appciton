#pragma once
#include "NIBPServiceState.h"

class NIBPServiceCalibrate : public NIBPServiceState
{
    #ifdef CONFIG_UNIT_TEST
        friend class TestNIBPParam;
    #endif
public:
    virtual void triggerReturn(void);
    virtual void calibratedPoint(int point, int value);

    virtual void run(void);
    virtual void enter(void);
    virtual void connectError(void);
    virtual void unPacket(unsigned char *packet, int len);

    NIBPServiceCalibrate();
    ~NIBPServiceCalibrate();

private:
    bool _isReturn;
    bool _isEnterSuccess;
};
