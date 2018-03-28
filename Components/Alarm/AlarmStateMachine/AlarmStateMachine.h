#pragma once
#include <QMap>
#include <QObject>
#include "AlarmStateDefine.h"

// 报警状态状态机
class QTimer;
class AlarmState;
class AlarmStateMachine : public QObject
{
    Q_OBJECT

public:
    static AlarmStateMachine &Construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new AlarmStateMachine();
        }

        return *_selfObj;
    }

    static AlarmStateMachine *_selfObj;

    ~AlarmStateMachine();

    // 状态机开始
    void start();

    // 事件处理
    void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);

    // 状态切换
    void switchState(ALarmStateType type);

    // 获取报警使能状态
    inline bool isEnableAlarmAudioOff() const {return _isEnableAlarmAudioOff;}
    inline bool isEnableAlarmOff() const {return _isEnableAlarmOff;}

private slots:
    void _timeOut();

private:
    AlarmStateMachine();

private:
    bool _isEnableAlarmAudioOff;
    bool _isEnableAlarmOff;
    unsigned _pressTime;//按键时间

    AlarmState *_currentState;   //当前活跃的状态

    typedef QMap<ALarmStateType, AlarmState*> AlarmStateMap;
    AlarmStateMap _alarmStateMap;

    QTimer *_timer;
};
#define alarmStateMachine (AlarmStateMachine::Construction())
#define deleteAlarmStateMachine() (delete AlarmStateMachine::_selfObj)
