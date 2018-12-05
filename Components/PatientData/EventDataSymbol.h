/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/12/5
 **/

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
            "All", "Prompt", "Low", "Medium", "High"
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
