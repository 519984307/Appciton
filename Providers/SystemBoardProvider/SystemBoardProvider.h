#pragma once
#include "BLMProvider.h"
#include "SystemDefine.h"
#include "PowerManagerProviderIFace.h"
#include "LightProviderIFace.h"

/***************************************************************************************************
 * 系统前面板数据提供者对象定义
 **************************************************************************************************/
class SystemBoardProvider : public BLMProvider,
        public PowerManagerProviderIFace, public LightProviderIFace
{
    Q_OBJECT

public:
    static SystemBoardProvider &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SystemBoardProvider();
        }
        return *_selfObj;
    }
    static SystemBoardProvider *_selfObj;

    SystemBoardProvider();
    ~SystemBoardProvider();

public:// PowerParamProviderIFace.
    virtual PowerSuplyType getPowerSuplyType(void);

    //获取版本号
    virtual void sendVersion(void);

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

public: // LightProviderIFace
    // 自检，发生在开机时刻，依次亮静音灯、低、中、高级别报警灯，最后熄灭。
    virtual void selfTest(void);

    // 设置当前的报警级别。
    // 当hasAlarmed为false时，表明当前无报警后面的priority不起作用。
    // 当hasAlarmed为true时，priority表示报警的级别。
    virtual void updateAlarm(bool hasAlarmed, AlarmPriority priority);

    // 设置报警静音，当静音打开后只亮静音灯，其他形式的灯光关闭。
    virtual void enableAlarmAudioMute(bool enable);

    // 指示灯控制。
    virtual void enableIndicatorLight(bool enable);

    // 开启/关闭除颤准备灯
    virtual void enableDefibReadyLED(bool enable);

public:
    // 查询初始旋钮的位置和快捷按钮的状态。
    void querySwitchKeyStatus(void);

    // 查询固定不变的电池信息
    void queryFixedBatMessage(void);

    // 触发蜂鸣器。
    void triggerBuzzer(void);

    // 版本信息。
    void getVersionInfo(QString &hwVersion, QString &swVersion, QString &protoVersion);

    // 获取错误警告码
    ErrorWaringCode getErrorWaringCode(void);

    // 请求关机
    void requestShutdown(void);

    // 请求复位
    void requestReset(void);

    // notify PD Module reset
    void pdModuleReset();

protected:
    virtual void handlePacket(unsigned char *data, int len);
    virtual void disconnected(void);
    virtual void reconnected(void);

private:
    void _parseVersionInfo(unsigned char *data, int len);
    void _parseKeyEvent(unsigned char *data, int len);
    void _parsePowerStat(unsigned char *data, int len);
    void _parseOperateMode(unsigned char *data, int len);
    void _parseFastOperateMode(unsigned char *data, int len);
    void _parsePowerOnStat(unsigned char *data, int len);
    void _parseFixedBatteryInfo(unsigned char *data, int len);
    void _parsePeriodicBatteryInfo(unsigned char *data, int len);
    void _parsePeriodicBatteryValue(unsigned char *data, int len);
    void _parseErrorWaringCode(unsigned char *data, int len);

    // 通知消息应答。
    void _notifyAck(unsigned char *data, int len);

private:
    bool _gotInitSwitchKeyStatus;

    struct ModeStatus
    {
        ModeStatus()
        {
           powerSuply = POWER_SUPLY_UNKOWN;
           errorCode = ERR_CODE_NONE;
        }

        QString hwVersion;
        QString swVersion;
        QString protoVersion;
        PowerSuplyType powerSuply;
        ErrorWaringCode errorCode;
    } modeStatus;

    bool _recordBatInfo;
    unsigned char _adcValue[2];
};
#define systemBoardProvider (SystemBoardProvider::construction())
#define deleteSystemBoardProvider() (delete SystemBoardProvider::_selfObj)
