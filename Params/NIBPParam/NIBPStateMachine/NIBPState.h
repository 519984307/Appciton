#pragma once
#include "NIBPStateMachine.h"
#include "NIBPEventDefine.h"
#include <QObject>
#include <QTime>
#include <QTimer>

class NIBPStateMachine;
class NIBPState : public QObject, public TimerAction
{
    Q_OBJECT

public:
    // 处理事件。
    virtual void handleNIBPEvent(NIBPEvent event, const unsigned char args[], int argLen);

    virtual void run(void) {}

    // 进入。
    virtual void enter(void);
    virtual void enter(void *arg);

    // 退出。
    virtual void exit(void);

    // callback function when machine exit
    virtual void stateMachineExit(void);

    // 是否开启了CPR节拍音
    virtual bool isStartCPRMetronome() {return false;}

    // handle message after disarm/delivered/selftest
    virtual void handleShowBiphasicMesg() {}

public:
    // 获取状态的类型。
    unsigned char getID(void) const;

    // 设置关联的状态机。
    void setStateMachine(NIBPStateMachine *machine);

    // 获取关联的状态机。
    NIBPStateMachine *getStateMachine(void);

    // 构造与析构。
    NIBPState(unsigned char id);
    virtual ~NIBPState();

protected:
    // 设置超时时限，ms单位。
    void setTimeOut(int t = 2000);

    // 重新设置超时时限
    void resetTimeOut(int t);

    // 停止定时器
    void timeStop();

    // 切换指定的状态。
    void switchState(unsigned char newStateID);
    void switchState(unsigned char newStateID, void *arg);

    // 返回到状态机对象。
    void returnToMachine(void);
    void returnToMachine(void *arg);

private slots:
    void _timeout(void);

private:
    QTimer *_actionTime;
    QTime _elapseTime;
    unsigned char _id;
    NIBPStateMachine *_stateMachine; // 关联的状态机。
};
