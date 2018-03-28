#pragma once
#include "PACEDefine.h"

class PACEProviderIFace
{
public:
    // Set pace parameters
    virtual void setParameters(PACEMode mode, int rate, int current) = 0;

    // Start PACE.
    virtual void begin(void) = 0;

    // Stop PACE.
    virtual void stop(void) = 0;

    // enable 4:1 mode.
    virtual void enable41(void) = 0;

    // disable 4:1 mode.
    virtual void disable41(void) = 0;

    // 构造与析构。
    PACEProviderIFace() { }
    virtual ~PACEProviderIFace() { }
};
