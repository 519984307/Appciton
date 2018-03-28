#pragma once
#include "Param.h"
#include <QTimer>
#include "NIBPServiceState.h"
#include "NIBPProviderIFace.h"
#include "TN3Provider.h"
#include <QMap>

enum NIBPServiceType
{
    NIBP_Service_REPAIR_Enter,
    NIBP_Service_REPAIR_Return,

    NIBP_Service_CALIBRATE_Enter,
    NIBP_Service_CALIBRATE_Return,

    NIBP_Service_MANOMETER_Enter,
    NIBP_Service_MANOMETER_Return,

    NIBP_Service_PRESSURECONTROL_Enter,
    NIBP_Service_PRESSURECONTROL_Return,

    NIBP_Service_CALIBRATE_ZERO_Enter,
    NIBP_Service_CALIBRATE_ZERO_Return,

    NIBP_Service_Pressurepoint,                    //校准点压力值输入应答
    NIBP_Service_NR
};


class NIBPParamService: public Param
{
    Q_OBJECT

    #ifdef CONFIG_UNIT_TEST
    friend class TestNIBPParam;
    #endif

public:
    static NIBPParamService &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new NIBPParamService();
        }
        return *_selfObj;
    }
    static NIBPParamService *_selfObj;
    ~NIBPParamService();

public:
    // 初始化参数。
    virtual void initParam(void);

    // 模块复位。
    void reset(void);

    // NIBP错误,模块禁用
    void errorDisable(void);
    bool isErrorDisable(void) {return _isNIBPError;}

    // 进入服务模式。
    void serviceEnter(bool enter);

    //退出指令
    void serviceReturn(void);

    //校准点压力值输入
    void servicePressurepoint(int point, int value);

    //充气值控制
    void servicePressureinflate(int Value);

    //放气控制
    void servicePressuredeflate(void);

    //关闭阀门
    void serviceCloseValve(bool enter);

    // 设置病人类型。
    void servicePatientType(unsigned char type);

    //校零
    void servicePressureZero(void);

    //过压保护
    void servicePressureProtect(bool enter);

    // 设置服务模式数据提供对象。
    void setServiceProvider(NIBPProviderIFace *provider);

    // 获取单位。
    UnitType getUnit(void);

    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 透传模式。
    void setPassthroughMode(bool flag);

public:
    // 解析数据包。
    void unPacket(unsigned char *packet, int len);

    // 状态机运行。
    void machineRun(void);

    // 切换虚拟机状态。
    void switchState(NIBPServiceStateType state);

    // 通信中断。
    void disConnected(bool flag);

    //通信错误标志
    bool connectWrongFlag;

private:
    NIBPParamService();
    NIBPServiceType _type;
    NIBPProviderIFace *_provider;
    bool _isNIBPError;                      // 模块错误；

// 状态机相关。
private:
    typedef QMap<NIBPServiceStateType, NIBPServiceState*> StateMap;
    StateMap _servicestates;
    NIBPServiceState *_currentServiceState;
};
#define nibpParamService (NIBPParamService::construction())
