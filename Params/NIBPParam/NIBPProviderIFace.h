#pragma once
#include "PatientDefine.h"
#include "NIBPDefine.h"
#include <QString>

// 定义ECG数据提供对象需要实现的接口方法。
class NIBPProviderIFace
{
public:
    // 起停测量。
    virtual void startMeasure(PatientType /*type*/) { }
    virtual void stopMeasure(void) { }

    // 压力数据，不是压力数据返回-1。
    virtual short lowPressure(unsigned char */*packet*/) {return -1;}

    // 透传模式。
    virtual void setPassthroughMode(bool /*flag*/) { }

    // 获取自检状态。
    virtual void sendSelfTest(void) { }

    // 设置预充气压力值。
    virtual void setInitPressure(short /*pressure*/) { }

    // 设置智能压力使能。
    virtual void enableSmartPressure(bool /*enable*/) { }

    // 设置病人类型。
    virtual void setPatientType(unsigned char /*type*/) { }

    // 发送启动指令是否有该指令的应答。
    virtual bool needStartACK(void) { return false; }
    virtual bool isStartACK(unsigned char */*packet*/) {return false;}

    // 发送停止指令是否有该指令的应答。
    virtual bool needStopACK(void) { return false; }
    virtual bool isStopACK(unsigned char */*packet*/) {return false;}

    // 压力数据，不是压力数据返回-1。
    virtual short cuffPressure(unsigned char */*packet*/) {return -1;}

    // 测量是否结束。
    virtual bool isMeasureDone(unsigned char */*packet*/) {return false;}

    // 是否为错误数据包。
    virtual NIBPOneShotType isMeasureError(unsigned char */*packet*/)
    {
        return NIBP_ONESHOT_NONE;
    }

    // 发送获取结果请求。
    virtual void getResult(void) { }

    // 是否为结果包。
    virtual bool isResult(unsigned char */*packet*/,short &/*sys*/,
                          short &/*dia*/, short &/*map*/, short &/*pr*/, NIBPOneShotType &/*err*/)
    {
        return false;
    }

    // 进入维护模式。
    virtual void service_Enter(bool /*enter*/) { }
    virtual bool isService_Enter(unsigned char */*packet*/) {return false;}

    // 进入校准模式。
    virtual void service_Calibrate(bool /*enter*/) { }
    virtual bool isService_Calibrate(unsigned char */*packet*/) {return false;}

    //校准点压力值输入
    virtual void service_Pressurepoint(int /*num*/, short /*pressure*/) { }
    virtual bool isService_Pressurepoint(unsigned char */*packet*/) {return false;}

    // 进入压力计模式控制。
    virtual void service_Manometer(bool /*enter*/) { }
    virtual bool isService_Manometer(unsigned char */*packet*/) {return false;}

    // 进入压力操控模式。
    virtual void service_Pressurecontrol(bool /*enter*/) { }
    virtual bool isService_Pressurecontrol(unsigned char */*packet*/) {return false;}

    //压力控制（充气）
    virtual void service_Pressureinflate(short /*pressure*/) { }
    virtual bool isService_Pressureinflate(unsigned char */*packet*/) {return false;}

    //放气控制
    virtual void service_Pressuredeflate(void) { }
    virtual bool isService_Pressuredeflate(unsigned char */*packet*/) {return false;}

    //气阀控制
    virtual void service_Valve(bool /*enter*/) { }
    virtual bool isService_Valve(unsigned char */*packet*/)  {return false;}

    // 压力数据，不是压力数据返回-1。
    virtual int service_cuffPressure(unsigned char */*packet*/) {return -1;}

    //进入校零模式
    virtual void service_CalibrateZero(bool /*enter*/) { }
    virtual bool isService_CalibrateZero(unsigned char */*packet*/) {return false;}

    //校零
    virtual void service_PressureZero(void) { }
    virtual bool isService_PressureZero(unsigned char */*packet*/) {return false;}

    //过压保护
    virtual void service_PressureProtect(bool /*enter*/) { }
    virtual bool isService_PressureProtect(unsigned char */*packet*/) {return false;}

    //进入升级模式
    virtual void service_UpgradeEnter(void) { }
    virtual bool isService_UpgradeEnter(unsigned char */*packet*/) {return false;}

    //数据发送启动
    virtual void service_UpgradeSend(void) { }
    virtual bool isService_UpgradeSend(unsigned char */*packet*/) {return false;}

    //升级模式擦除Flash
    virtual void service_UpgradeClear(void) { }
    virtual bool isService_UpgradeClear(unsigned char */*packet*/) {return false;}

    //升级模式硬件版本号
    virtual void service_UpgradeRead(void) { }
    virtual bool isService_UpgradeReade(unsigned char */*packet*/) {return false;}

    //文件片段传输
    virtual void service_UpgradeFile(unsigned char */*packet*/) { }
    virtual bool isService_UpgradeFile(unsigned char */*packet*/) {return false;}

    //升级结果
    virtual bool isService_UpgradeFinish(unsigned char */*packet*/) {return false;}

    //发送协议命令
    virtual void sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/,
                             unsigned int /*len*/) { }

public:
    NIBPProviderIFace() {  }
    virtual ~NIBPProviderIFace() { }
};
