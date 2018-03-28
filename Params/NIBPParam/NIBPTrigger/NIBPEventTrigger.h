#pragma once
#include "NIBPState.h"
#include <QTimer>
#include <QTime>

// 监控NIBP测量的总时间，辅助实现连续测量的计时功能。
class NIBPEventTrigger : public QObject
{
    Q_OBJECT

#ifdef CONFIG_UNIT_TEST
    friend class TestNIBPParam;
#endif

public:
    static NIBPEventTrigger &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new NIBPEventTrigger();
        }
        return *_selfObj;
    }
    static NIBPEventTrigger *_selfObj;
    ~NIBPEventTrigger();

public:
    void triggerbutton(void);
    void triggerpatienttype(void);
    void triggermodel(bool STAT_Flag);
    void triggerPD(void);
    void triggerError(void);
    void triggerReset(void);
    void stop(void) { triggerbutton(); }
    int safeWaitTime(void);

private:
    NIBPEventTrigger();
    NIBPState *_currentState;
    int _timing;
    int _safeWaitTiming;                        //安全间隔倒计时时间
    void _switchToAuto(void);                   //状态转为AUTO
    void _switchToManual(void);                 //状态转为MANUAL
    void _safeWaitTimeSTATStop(void);           //STAT在安全间隔内被关闭
    NIBPState *state;
};
#define nibpEventTrigger (NIBPEventTrigger::construction())
#define deleteNIBPEventTrigger() (delete NIBPEventTrigger::_selfObj)
