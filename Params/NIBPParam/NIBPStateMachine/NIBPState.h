#pragma once
#include <QString>
#include <QTime>
#include "NIBPProviderIFace.h"
#include "Debug.h"

enum NIBPStateType
{
    NIBP_STATE_STANDBY,                    // 待机状态。
    NIBP_STATE_STARTING,                   // 启动测量状态。
    NIBP_STATE_MEASURING,                  // 正在测量状态。
    NIBP_STATE_STOPING,                    // 停止状态。
    NIBP_STATE_GET_RESULT,                 // 获取结果状态。
    NIBP_STATE_TIMING,                     // 测量倒计时状态。
    NIBP_STATE_SAFEWAITTIME,               // 安全等待时间
    NIBP_STATE_ERROR,                      // 错误状态
    NIBP_STATE_NR
};

class NIBPState
{
#ifdef CONFIG_UNIT_TEST
    friend class TestNIBPParam;
#endif
public:
    // 获取状态对象的名字。
    NIBPStateType type(void);

    // 设置Provider对象。
    static void setProvider(NIBPProviderIFace *p);

    // 构造与析构。
    NIBPState(NIBPStateType type);
    virtual ~NIBPState();

public:
    // 解析该数据包。
    virtual void unPacket(unsigned char */*packet*/, int /*len*/) { }

    // 主运行。
    virtual void run(void) { }

    // 进入该状态。
    virtual void enter(void) { }

protected:
    static NIBPProviderIFace *provider;

    // 设置超时时限，ms单位。
    void setTimeOut(int t = 2000);

    // 重新设置超时时限
    void resetTimeOut(int t);

    // 增加倒计时时间。
    void addTime(int t);

    // 是否超时。
    bool isTimeOut(void);

    // 自启动倒计时的时间流逝。
    int elapseTime(void);

private:
    NIBPStateType _type;
    QTime _elapseTime;
    int _timeOut;
};
