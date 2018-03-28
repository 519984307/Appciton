#pragma once
#include "KeyActionIFace.h"

class SupervisorModeKeyAction : public KeyActionIFace
{
public:
    SupervisorModeKeyAction();
    ~SupervisorModeKeyAction();

    virtual void keyF7Released(bool multiBtnPress);
    virtual void keyF8Pressed(bool multiBtnPress);
};

