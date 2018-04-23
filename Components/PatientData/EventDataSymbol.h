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
};
