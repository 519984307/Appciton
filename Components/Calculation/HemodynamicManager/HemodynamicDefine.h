#pragma once
#include <QString>
#include "BaseDefine.h"

enum HemodynamicParam
{
    HEMODYNAMIC_PARAM_CO,
    HEMODYNAMIC_PARAM_HR,
    HEMODYNAMIC_PARAM_PAWP,
    HEMODYNAMIC_PARAM_ARTMEAN,
    HEMODYNAMIC_PARAM_PAMEAN,
    HEMODYNAMIC_PARAM_CVP,
    HEMODYNAMIC_PARAM_EDV,
    HEMODYNAMIC_PARAM_HEIGHT,
    HEMODYNAMIC_PARAM_WEIGHT,
    HEMODYNAMIC_PARAM_NR
};

enum HemodynamicOutput
{
    HEMODYNAMIC_OUTPUT_CI,
    HEMODYNAMIC_OUTPUT_SV,
    HEMODYNAMIC_OUTPUT_SVR,
    HEMODYNAMIC_OUTPUT_PVR,
    HEMODYNAMIC_OUTPUT_LCW,
    HEMODYNAMIC_OUTPUT_LVSW,
    HEMODYNAMIC_OUTPUT_RCW,
    HEMODYNAMIC_OUTPUT_RVSW,
    HEMODYNAMIC_OUTPUT_EF,
    HEMODYNAMIC_OUTPUT_BSA,
    HEMODYNAMIC_OUTPUT_SVI,
    HEMODYNAMIC_OUTPUT_SVRI,
    HEMODYNAMIC_OUTPUT_PVRI,
    HEMODYNAMIC_OUTPUT_LCWI,
    HEMODYNAMIC_OUTPUT_LVSWI,
    HEMODYNAMIC_OUTPUT_RCWI,
    HEMODYNAMIC_OUTPUT_RVSWI,
    HEMODYNAMIC_OUTPUT_NR
};

struct HemodynamicInfo
{
  float calcInput[HEMODYNAMIC_PARAM_NR];
  float calcOutput[HEMODYNAMIC_OUTPUT_NR];
  unsigned calcTime;
};

struct ParamLimitInfo
{
    float low;
    float high;
};

class HemodynamicSymbol
{
public:
    static const char *convert(HemodynamicParam index)
    {
        static const char *symbol[HEMODYNAMIC_PARAM_NR] =
        {
            "C.O.", "HR", "PAWP", "Art Mean", "PA Mean", "CVP", "EDV", "Height", "Weight"
        };
        return symbol[index];
    }

    static const char *convertUnit(HemodynamicParam index)
    {
        static const char *symbol[HEMODYNAMIC_PARAM_NR] =
        {
            "L/min", "bpm", "mmHg", "mmHg", "mmHg", "mmHg", "ml", "cm", "kg"
        };
        return symbol[index];
    }

    static const char *convert(HemodynamicOutput index)
    {
        static const char *symbol[HEMODYNAMIC_OUTPUT_NR] =
        {
            "C.I.", "SV", "SVR", "PVR", "LCW", "LVSW", "RCW", "RVSW", "EF",
            "BSA", "SVI", "SVRI", "PVRI", "LCWI", "LVSWI", "RCWI", "RVSWI"
        };
        return symbol[index];
    }

    static const char *convertUnit(HemodynamicOutput index)
    {
        static const char *symbol[HEMODYNAMIC_OUTPUT_NR] =
        {
            "L/min/m2", "ml", "DS/cm5", "DS/cm5", "kg.m", "g.m", "kg.m", "g.m", "%",
            "m2", "ml/m2", "DS.m2/cm5", "DS.m2/cm5", "kg.m/m2", "g.m/m2", "kg.m/m2", "g.m/m2"
        };
        return symbol[index];
    }

    static const char *convertRange(HemodynamicOutput index)
    {
        static const char *symbol[HEMODYNAMIC_OUTPUT_NR] =
        {
            "2.5~4.0", "60.0~130.0", "770~1500", "100~250", "5.4~10.0", "", "", "", "",
            "", "36.0~65.0", "1680~2580", "225~315", "3.0~5.5", "47.2~70.8", "0.54~0.66", "7.90~9.70"
        };
        return symbol[index];
    }
};
