#pragma once
#include "EventDataDefine.h"

class EventDataSymbol
{
public:
    static const char *convert(EventType index)
    {
        static const char *symbol[EventTypeMax] =
        {
            "All", "PhyAlarm", "CodeMarker", "RealTimePrint",
            "NIBPMeasurement", "WaveFreeze"
        };
        return symbol[index];
    }

    static const char *convert(EventLevel index)
    {
        static const char *symbol[EVENT_LEVEL_NR] =
        {
            "All", "Hint", "Low", "Medium", "High"
        };
        return symbol[index];
    }

    static const char *convert(ECGEventGain index)
    {
        static const char *symbol[ECG_EVENT_GAIN_NR] =
        {
            "0.125 cm/mV", "0.25 cm/mV", "0.5 cm/mV", "1.0 cm/mV", "2.0 cm/mV", "4.0 cm/mV"
        };
        return symbol[index];
    }
};
