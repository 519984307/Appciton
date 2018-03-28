#pragma once
#include "LanguageManager.h"

enum UnitType
{
    UNIT_NONE,
    UNIT_TC,
    UNIT_TF,
    UNIT_TDC,
    UNIT_TDF,
    UNIT_MMHG,
    UNIT_KPA,
    UNIT_PERCENT,
    UNIT_RPM,
    UNIT_BPM,
    UNIT_MICROVOLT,
    UNIT_MV,
    UNIT_KG,
    UNIT_LB,
    UNIT_CM,
    UNIT_MM,
    UNIT_INCH,
    UNIT_MS,
    UNIT_PPM,
    UNIT_CPM,
    UNIT_MA,
    UNIT_LPM,
    UNIT_LPMPSQM,
    UNIT_NR
};

class Unit
{
public:
    static const char *getSymbol(UnitType t)
    {
        static const char *symbol[UNIT_NR] =
        {
            " ", "celsius", "fahrenheit", "celsius", "fahrenheit", "mmHg", "kPa", "percent",
            "rpm", "bpm", "μv", "mv", "kg", "lb", "cm", "mm", "inch", "ms", "PPM", "CPM", "mA",
            "lpm", "lpmpsqm"
        };
        return symbol[t];
    }

    static const QString &localeSymbol(UnitType t)
    {
        return trs(getSymbol(t));
    }

    // mmHg和%之间的换算公式如下：
    // (二氧化碳)mmHg = (二氧化碳浓度)*(来至ISA的大气压(KPa))*(750 / 100).
    // 若体积分数为srcVal%，当前气压值为baro百帕，因为1mmHg = 1/7.5 KPa,则srcVal%转换成KPa为：
    // (二氧化碳)KPa = srcVal / 100 * baro / 10 * (750 / 100) / 7.5 (KPa)
    //              = srcVal * baro / 1000 (KPa)
    // 单位换算。
    template<typename T2>
    static inline QString convert(UnitType destUnit, UnitType srcUnit,
                const T2 &srcVal, const float &baro = 1013)
    {
        QString destVal = QString::number(srcVal);

        // % <---> mmHg
        if((destUnit == UNIT_PERCENT) && (srcUnit == UNIT_MMHG))
        {
            destVal.sprintf("%.1f", srcVal / baro * 1000 / 7.5);
        }
        else if((destUnit == UNIT_MMHG) && (srcUnit == UNIT_PERCENT))
        {
            destVal = QString::number((int)(srcVal * baro / 1000 * 7.5 + 0.5));
        }

        // kPa <---> mmHg
        else if((destUnit == UNIT_KPA) && (srcUnit == UNIT_MMHG))
        {
            destVal = QString::number(srcVal / 7.5, 'f', 1);
        }
        else if((destUnit == UNIT_MMHG) && (srcUnit == UNIT_KPA))
        {
            destVal = QString::number((int)(srcVal * 7.5 + 0.5));
        }

        // % <---> kPa
        else if((destUnit == UNIT_PERCENT) && (srcUnit == UNIT_KPA))
        {
            destVal.sprintf("%.1f", srcVal / baro * 1000);
        }
        else if((destUnit == UNIT_KPA) && (srcUnit == UNIT_PERCENT))
        {
            destVal = QString::number(srcVal * baro / 1000, 'f', 1);
        }

        // inch <---> cm
        else if((destUnit == UNIT_INCH) && (srcUnit == UNIT_CM))
        {
            destVal.sprintf("%.1f", srcVal / 2.54);
        }
        else if((destUnit == UNIT_CM) && (srcUnit == UNIT_INCH))
        {
            destVal.sprintf("%.1f", srcVal * 2.54);
        }

        // inch <---> mm
        else if((destUnit == UNIT_INCH) && (srcUnit == UNIT_MM))
        {
            destVal.sprintf("%.1f", srcVal / 25.4);
        }
        else if((destUnit == UNIT_MM) && (srcUnit == UNIT_INCH))
        {
            destVal.sprintf("%.1f", srcVal * 25.4);
        }

        // cm <---> mm
        else if((destUnit == UNIT_CM) && (srcUnit == UNIT_MM))
        {
            destVal.sprintf("%.1f", srcVal / 10.0);
        }
        else if((destUnit == UNIT_MM) && (srcUnit == UNIT_CM))
        {
            destVal = QString::number((int)srcVal * 10);
        }

        // lb <---> kg
        else if((destUnit == UNIT_LB) && (srcUnit == UNIT_KG))
        {
            destVal.sprintf("%.1f", srcVal * 2.20462);
        }
        else if((destUnit == UNIT_KG) && (srcUnit == UNIT_LB))
        {
            destVal.sprintf("%.1f", srcVal / 2.20462);
        }

        // ℉ <--->℃
        else if((destUnit == UNIT_TC) && (srcUnit == UNIT_TF))
        {
            destVal.sprintf("%.1f", (srcVal - 32.0) / 1.8);
        }
        else if((destUnit == UNIT_TF) && (srcUnit == UNIT_TC))
        {
            destVal.sprintf("%.1f", 1.8 * srcVal + 32);
        }

        // ℉D <--->℃D
        else if((destUnit == UNIT_TDF) && (srcUnit == UNIT_TDC))
        {
            destVal.sprintf("%.1f", 1.8 * srcVal);
        }
        else if((destUnit == UNIT_TDC) && (srcUnit == UNIT_TDF))
        {
            destVal.sprintf("%.1f", srcVal / 1.8);
        }

        return destVal;
    }
};
