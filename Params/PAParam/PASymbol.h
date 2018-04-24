#pragma once
#include "PADfine.h"

// 将定义的枚举转换成符号。
class PASymbol
{
public:
    static const char *convert(PressureDesigner_d index)
    {
        static const char *symbol[PD_NR] =
        {
            "Art", "PA", "Ao", "UAP", "BAP", "FAP", "CVP",
            "LAP", "RAP", "ICP","UVP","LV","P1","P2","P3","P4"
        };
        return symbol[index];
    }

    static const char *convert(WaveformRuler_d index)
    {
        static const char *symbol[WAVEFORM_PULER_NR] =
        {
            "auto", "manual", "-10~10", "0~20", "0~30", "0~40", "0~60","0~80",
            "0~140", "30~140", "60~140", "0~160", "0~200", "0~240", "0~300",
        };
        return symbol[index];
    }

    static const char *convert(SweepSpeed_d index)
    {
        static const char *symbol[SWEEP_SPEED_NR] =
        {
            "6.25mm/s", "12.5mm/s", "25mm/s", "50mm/s"
        };
        return symbol[index];
    }

    static const char *convert(FrequencyFiltering_d index)
    {
        static const char *symbol[FREQUENCY_FILTERING_NR] =
        {
            "8HZ", "12.5HZ", "20HZ", "40HZ"
        };
        return symbol[index];
    }

    static const char *convert(PPVSource_d index)
    {
        static const char *symbol[PPV_SOURCE_NR] =
        {
            "IBP", "Art", "Ao", "FAP", "BAP", "PA",
            "UAP", "P1", "P2", "P3", "P4", "LV","AUTO"
        };
        return symbol[index];
    }
};
