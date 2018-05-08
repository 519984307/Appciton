#pragma once
#include "NIBPStateMachineDefine.h"
#include "NIBPEventDefine.h"
#include <QMap>
#include <QObject>

class NIBPState;
class NIBPStateMachine : public QObject
{
    Q_OBJECT

public:
    // 主运行。
    virtual void run(void);

    // 重启复位。
    virtual void reset(void);

    // 处理事件。
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen);

    // 虚拟机进入与退出。
    virtual void enter(void);
    virtual void exit(void);

    // name
    virtual const QString name() = 0;

    // 切换状态。
    virtual void switchToState(unsigned char newStateID);
    virtual void switchToState(unsigned char newStateID, void *arg);

    // 状态对象退出。
    virtual void stateExit(unsigned char stateID, void *arg);
    virtual void stateExit(unsigned char stateID);
    virtual void stateExit(void);

    // 是否开启了CPR节拍音
    virtual bool isStartCPRMetronome() {return false;}

    // 当前状态类型
    virtual int curStatusType() {return -1;}

    // check whether the state machine is exited
    bool isExit() const;

public:
    // 获取状态机的类型。
    NIBPStateMachineType type(void) const;

    // 构造与析构。
    NIBPStateMachine(NIBPStateMachineType type);
    virtual ~NIBPStateMachine();

protected:
    // 开始定时器和中止定时器。
    void beginTimer(int interval);
    void endTimer(void);

    // 注册一个状态对象。
    void registerState(NIBPState *state);

    // 获取一个状态。
    NIBPState *getState(unsigned char stateID);

    // 当前活跃的状态。
    NIBPState *activateState;

private:
    int _timerID;
    bool _isExited;
    NIBPStateMachineType _type;
    QMap<unsigned char, NIBPState*> _nibpStates;
};
