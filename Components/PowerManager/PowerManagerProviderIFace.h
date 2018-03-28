#pragma once

enum PowerSuplyType
{
    POWER_SUPLY_UNKOWN,
    POWER_SUPLY_AC,
    POWER_SUPLY_BAT,
    POWER_SUPLY_AC_BAT,
    POWER_SUPLY_NR,
};

class PowerManagerProviderIFace
{
public:
    // 获取电源模式。
    virtual PowerSuplyType getPowerSuplyType(void) = 0;

    // 构造与析构。
    PowerManagerProviderIFace() {}
    virtual ~PowerManagerProviderIFace() {}
};
