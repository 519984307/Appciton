#pragma once
#include <QObject>
#include <QBitArray>
#include "Param.h"
#include "SystemDefine.h"

#ifdef Q_WS_X11
#include <QTcpSocket>
#include <QQueue>
#endif

#define IDM_SOFTWARE_VERSION ("1.0.3")

enum ConfiguredFuncs
{
    CONFIG_RESP = 0x02,
    CONFIG_ECG12LEADS = 0x04,
    CONFIG_SPO2 = 0x08,
    CONFIG_NIBP = 0x10,
    CONFIG_CO2 = 0x20,
    CONFIG_AG,
    CONFIG_CO,
    CONFIG_IBP,
    CONFIG_TEMP = 0x40,
    CONFIG_WIFI = 0x80,
};

// 前面板按键状态,与系统板获取按键状态命令回复一致
enum PanelKeyStatus
{
    PANEL_KEY_0 = 0,
    PANEL_KEY_1,
    PANEL_KEY_2,
    PANEL_KEY_3,
    PANEL_KEY_4,
    PANEL_KEY_ALARM,
    PANEL_KEY_MENU,
    PANEL_KEY_NIBP,
    PANEL_KEY_RECORDER,
    PANEL_KEY_ENERGY_DN,
    PANEL_KEY_ENERGY_UP,
    PANEL_KEY_CHARGING,
    PANEL_KEY_SHOCK,
    PANEL_KEY_ENTER,
    PANEL_KEY_0_4,
    PANEL_KEY_1_4,
    PANEL_KEY_0_3,
    PANEL_KEY_NR = 24
};

// 模块开机检查结果类型
enum ModulePoweronTestResult
{
    TE3_MODULE_SELFTEST_RESULT,
    E5_MODULE_SELFTEST_RESULT,
    TN3_MODULE_SELFTEST_RESULT,
    TS3_MODULE_SELFTEST_RESULT,
    TT3_MODULE_SELFTEST_RESULT,
    CO2_MODULE_SELFTEST_RESULT,
    PRINTER_SELFTEST_RESULT,
    PANEL_KEY_POWERON_TEST_RESULT,
    MODULE_POWERON_TEST_RESULT_NR
};

//模块开机测试状态
enum ModulePoweronTestStatus
{
    SELFTEST_UNKNOWN,           //未开始
    SELFTEST_FAILED_AND_RETRY,  //自检失败并再次尝试
    SELFTEST_MODULE_RESET,      //模块复位
    SELFTEST_FAILED,            //自检失败
    SELFTEST_SUCCESS,           //自检成功
    SELFTEST_NOT_SUPPORT,       //模块不支持
    SELFTEST_STATUS_NR
};

class QTimer;
class SystemSelftestMenu;
class SystemManager : public QObject
{
    Q_OBJECT

#ifdef CONFIG_UNIT_TEST
    friend class TestNIBPParam;
#endif
public:
    static SystemManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SystemManager();
        }
        return *_selfObj;
    }
    static SystemManager *_selfObj;
    virtual ~SystemManager();

public:
    // 获取屏幕像素点之间的距离。
    float getScreenPixelWPitch(void);
    float getScreenPixelHPitch(void);

    //  查询是否支持该功能。
    bool isSupport(ConfiguredFuncs funcs) const;

    // get the module config status
    unsigned int getModuleConfig() const;

    // set module power on test result
    void setPoweronTestResult(ModulePoweronTestResult module, ModulePoweronTestStatus result);

public:
    // 按键协议解析。
    void parseKeyValue(const unsigned char *data, unsigned int len);

    // 背光亮度设置/获取。
    void setBrightness(BrightnessLevel br);
    void enableBrightness(BrightnessLevel br);
    BrightnessLevel getBrightness(void);

    // 加载初始底层模式。
    void loadInitBMode(void);

    // 获取IDM上位机软件版本
    void getSoftwareVersion(QString &revision);
    QString getSoftwareVersionNum();

    // 是否确认了自检结果
    bool isAcknownledgeSystemTestResult();

    // system self-test is over
    bool isSystemSelftestOver() const {return _selfTestFinish;}
    void systemSelftestOver() {_selfTestFinish = true;}

    // hide system test diag
    void closeSystemTestDialog();

    // check whether the system is going to turn off
    bool isGoingToTrunOff() const {return _isTurnOff;}
    void turnOff(bool flag);

private:
    bool _isTestModuleSupport(ModulePoweronTestResult module);

#ifdef Q_WS_X11
public:
    bool sendCommand(const QByteArray &cmd);

public slots:
    void onCtrlSocketReadReady();

signals:
    void metronomeReceived();

private:
    QTcpSocket *_ctrlSocket;
    QQueue<char> _socketInfoData;
#endif

private slots:
    void _publishTestResult();

private:
    SystemManager();

    void _handleBMode(void);

    int _backlightFd;       // 背光控制文件句柄。

private:
    QVector<int> _modulePostResult; //模块开机测试结果
    QTimer *_publishTestTimer;
    SystemSelftestMenu *_selfTestResult;
    bool _selfTestFinish;
    bool _isTurnOff;
};
#define systemManager (SystemManager::construction())
#define deleteSystemManager() (delete SystemManager::_selfObj);
