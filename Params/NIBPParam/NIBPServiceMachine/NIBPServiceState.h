#pragma once
#include <QString>
#include <QTime>
#include "NIBPProviderIFace.h"
#include "Debug.h"

enum NIBPServiceStateType
{
    NIBP_Service_STANDBY,                          // 准备模式。
    NIBP_Service_REPAIR,                           // 维护模式。
    NIBP_Service_CALIBRATE,                        // 校准模式。
    NIBP_Service_MANOMETER,                        // 压力计模式。
    NIBP_Service_PRESSURECONTROL,                  // 压力操控模式。
    NIBP_Service_CALIBRATE_ZERO,                   // 校零模式。
    NIBP_Service_UPGRADE,                          // 升级模式
    NIBP_Service_NULL,                             // 空
};

class NIBPServiceState
{
public:
    // 获取状态对象的名字。
    NIBPServiceStateType type(void);

    // 设置Provider对象。
    static void setProvider(NIBPProviderIFace *p);

    // 构造与析构。
    NIBPServiceState(NIBPServiceStateType type);
    virtual ~NIBPServiceState();

public:
    // 解析该数据包。
    virtual void unPacket(unsigned char */*packet*/, int /*len*/) { }

    virtual void triggerEnter(void) { }
    virtual void triggerReturn(void) { }

    //校准模式
    virtual void calibratedPoint(int /*point*/, int /*value*/) { }

    //压力控制模式
    virtual void servicePressureInflate(int /*Value*/) { }
    virtual void servicePressureDeflate(void) { }
    virtual void servicePatientType(unsigned char /*type*/) { }
    virtual void servicePressureProtect(bool /*enter*/) { }

    //校零
    virtual void serviceCloseValve(bool /*enter*/) { }
    virtual void servicePressureZero(void) { }

    // 主运行。
    virtual void run(void) { }

    // 进入该状态。
    virtual void enter(void) { }

    // NIBP通信连接错误
    virtual void connectError(void) { }

protected:
    static NIBPProviderIFace *provider_service;

    // 设置超时时限，ms单位。
    void setTimeOut(int t = 2000);

    // 是否超时。
    bool isTimeOut(void);

    // 自启动倒计时的时间流逝。
    int elapseTime(void);

    void timeStop(void);

    bool timeStart;

private:
    NIBPServiceStateType _type;
    QTime _elapseTime;
    int _timeOut;
};
