#pragma once
#include "AlarmState.h"

class AlarmPauseState : public AlarmState
{
public:
    AlarmPauseState();
    ~AlarmPauseState();

    void enter();

    void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);
};
