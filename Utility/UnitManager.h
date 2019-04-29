/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/6
 **/

#pragma once
#include <QString>
#include <typeinfo>

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
    UNIT_CMH2O,
    UNIT_NR
};

struct Range
{
    Range() : upLimit(0.0), downLimit(0.0){}
    float upLimit;
    float downLimit;
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
            "lpm", "lpmpsqm", "cmH2O"
        };
        return symbol[t];
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
        QString destVal;
        if (strcmp(typeid(srcVal).name(), "f") == 0 || strcmp(typeid(srcVal).name(), "d") == 0)
        {
            // float或者double类型时,默认显示一位小数
            destVal = QString::number(srcVal, 'f', 1);
        }
        else
        {
            destVal = QString::number(srcVal);
        }
        // % <---> mmHg
        if ((destUnit == UNIT_PERCENT) && (srcUnit == UNIT_MMHG))
        {
            destVal = QString::number(srcVal / baro * 1000 / 7.5, 'f', 1);
        }
        else if ((destUnit == UNIT_MMHG) && (srcUnit == UNIT_PERCENT))
        {
            destVal = QString::number(static_cast<int>(srcVal * baro / 1000 * 7.5 + 0.5));
        }

        // kPa <---> mmHg
        else if ((destUnit == UNIT_KPA) && (srcUnit == UNIT_MMHG))
        {
            destVal = QString::number(srcVal / 7.5, 'f', 1);
        }
        else if ((destUnit == UNIT_MMHG) && (srcUnit == UNIT_KPA))
        {
            destVal = QString::number(static_cast<int>(srcVal * 7.5 + 0.5));
        }

        // % <---> kPa
        else if ((destUnit == UNIT_PERCENT) && (srcUnit == UNIT_KPA))
        {
            destVal = QString::number(srcVal / baro * 1000, 'f', 1);
        }
        else if ((destUnit == UNIT_KPA) && (srcUnit == UNIT_PERCENT))
        {
            destVal = QString::number(srcVal * baro / 1000, 'f', 1);
        }

        // inch <---> cm
        else if ((destUnit == UNIT_INCH) && (srcUnit == UNIT_CM))
        {
            destVal = QString::number(srcVal / 2.54, 'f', 1);
        }
        else if ((destUnit == UNIT_CM) && (srcUnit == UNIT_INCH))
        {
            destVal = QString::number(srcVal * 2.54, 'f', 1);
        }

        // inch <---> mm
        else if ((destUnit == UNIT_INCH) && (srcUnit == UNIT_MM))
        {
            destVal = QString::number(srcVal / 25.4, 'f', 1);
        }
        else if ((destUnit == UNIT_MM) && (srcUnit == UNIT_INCH))
        {
            destVal = QString::number(srcVal * 25.4, 'f', 1);
        }

        // cm <---> mm
        else if ((destUnit == UNIT_CM) && (srcUnit == UNIT_MM))
        {
            destVal = QString::number(srcVal / 10.0, 'f', 1);
        }
        else if ((destUnit == UNIT_MM) && (srcUnit == UNIT_CM))
        {
            destVal = QString::number(static_cast<int>(srcVal * 10));
        }

        // lb <---> kg
        else if ((destUnit == UNIT_LB) && (srcUnit == UNIT_KG))
        {
            destVal = QString::number(srcVal * 2.20462, 'f', 1);
        }
        else if ((destUnit == UNIT_KG) && (srcUnit == UNIT_LB))
        {
            destVal = QString::number(srcVal / 2.20462, 'f', 1);
        }

        // ℉ <--->℃
        else if ((destUnit == UNIT_TC) && (srcUnit == UNIT_TF))
        {
            destVal = QString::number((srcVal - 32.0) / 1.8, 'f', 1);
        }
        else if ((destUnit == UNIT_TF) && (srcUnit == UNIT_TC))
        {
            destVal = QString::number(1.8 * srcVal + 32, 'f', 1);
        }

        // ℉D <--->℃D
        else if ((destUnit == UNIT_TDF) && (srcUnit == UNIT_TDC))
        {
            destVal = QString::number(1.8 * srcVal, 'f', 1);
        }
        else if ((destUnit == UNIT_TDC) && (srcUnit == UNIT_TDF))
        {
            destVal = QString::number(srcVal / 1.8, 'f', 1);
        }

        return destVal;
    }

    /**
     * @brief getRange 获取相应单位的输入值范围
     * @param type
     * @return
     */
    static inline Range getRange(UnitType type)
    {
        Range range;
        switch (type) {
        case UNIT_INCH:
            range.upLimit = 118.0;
            range.downLimit = 8.0;
            break;
        case UNIT_CM:
            range.upLimit = 300.0;
            range.downLimit = 20.0;
            break;
        case UNIT_KG:
            range.upLimit = 200.0;
            range.downLimit = 0.1;
            break;
        case UNIT_LB:
            range.upLimit = 440.9;
            range.downLimit = 0.2;
            break;
        default:
            break;
        }
        return range;
    }
};
