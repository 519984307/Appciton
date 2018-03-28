#pragma once
#include "AlarmState.h"

class AlarmNormalState : public AlarmState
{
public:
    AlarmNormalState();
    ~AlarmNormalState();

    void enter();

    void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);
};

