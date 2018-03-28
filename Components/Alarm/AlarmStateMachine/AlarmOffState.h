#pragma once
#include "AlarmState.h"

class AlarmOffState : public AlarmState
{
public:
    AlarmOffState();
    ~AlarmOffState();

    void enter();
    void exit();

    void timerEvent(QTimerEvent *e);

    void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);
};

