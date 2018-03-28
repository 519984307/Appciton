#pragma once
#include "AlarmState.h"

class AlarmAudioOffState : public AlarmState
{
public:
    AlarmAudioOffState();
    ~AlarmAudioOffState();

    void enter();
    void exit();

    void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);

    void timerEvent(QTimerEvent *e);
};
