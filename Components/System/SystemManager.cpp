/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/14
 **/

#include "SystemManager.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <QDesktopWidget>
#include <QApplication>
#include <QVector>
#include <QThread>
#include <TDA19988Ctrl.h>
#include "Debug.h"
#include "IConfig.h"
#include "ECGParam.h"
#include "CO2Param.h"
#include "RESPParam.h"
#include "AlarmStateMachine.h"
#include "Framework/Utility/Utility.h"
#include "Framework/ErrorLog/ErrorLog.h"
#include "Framework/ErrorLog/ErrorLogItem.h"
#include "WindowManager.h"
#include "ParamManager.h"
#include "AlarmIndicator.h"
#include <QTimer>
#include <QFileInfo>
#ifdef Q_WS_QWS
#include <QWSServer>
#include "RunningStatusBar.h"
#endif
#include "PatientManager.h"
#include "DataStorageDirManager.h"
#include "StandbyWindow.h"
#include "NIBPParam.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/TimeDate/TimeDate.h"
#include "EventStorageManagerInterface.h"
#include "NightModeManager.h"

#define BACKLIGHT_DEV   "/sys/class/backlight/backlight/brightness"       // 背光控制文件接口
#define BRIGHTNESS_NR       20

static const char *systemSelftestMessage[SELFTEST_STATUS_NR][MODULE_POWERON_TEST_RESULT_NR] =
{
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL},

    {NULL, NULL, NULL, NULL, NULL, NULL, NULL},

    {NULL, NULL, NULL, NULL, NULL, "PrinterRestart", NULL},

    {
        "ECGSelftestFailed", "NIBPSelftestFailed", "SPO2SelftestFailed", "TEMPSelftestFailed",
        "CO2SelftestFailed", "PrinterSelftestFailed", "PanelKeySelftestFailed"
    },

    {NULL, NULL, NULL, NULL, NULL, "PrinterSelftestSuccess", NULL},

    {NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

class SystemManagerPrivate
{
public:
    SystemManagerPrivate()
        : modulePostResult(MODULE_POWERON_TEST_RESULT_NR, SELFTEST_UNKNOWN),
          workerThread(NULL), workMode(WORK_MODE_NORMAL), timeFormat(TIME_FORMAT_12),
          dateFormat(DATE_FORMAT_Y_M_D), backlightFd(-1),
      #ifdef Q_WS_QWS
          isTouchScreenOn(false),
      #endif
          isStandby(false),
          isTurnOff(false),
          isAutoBrightness(false)
    {
    }

    /**
     * @brief enterDemoMode enter the demo mode
     */
    void enterDemoMode()
    {
        if (nibpParam.isMeasuring())
        {
            nibpParam.stopMeasure();
        }
        alarmIndicator.delAllPhyAlarm();
        windowManager.showDemoWidget(true);
        paramManager.connectDemoParamProvider();
        patientManager.setMonitor(true);
        patientManager.newPatient();
        nibpParam.clearTrendListData();
    }

    /**
     * @brief enterNormalMode enter the normal mode
     */
    void enterNormalMode()
    {
        alarmIndicator.delAllPhyAlarm();
        windowManager.showDemoWidget(false);
        paramManager.connectParamProvider(WORK_MODE_DEMO);
        QString curFolderPath = dataStorageDirManager.getCurFolder();
        QFileInfo fileInfo(curFolderPath);
        QString curFolderName = fileInfo.fileName();
        patientManager.dischargePatient();
        dataStorageDirManager.deleteData(curFolderName);
    }

    /**
     * @brief enterStandbyMode enter standby mode
     */
    void enterStandbyMode()
    {
        // demo模式不进入待机模式
        setStandbyStatus(true);
        StandbyWindow w;
        w.exec();
    }


    /**
     * @brief setStandbyStatus set the system standby status
     * @param standby standby status
     */
    void setStandbyStatus(bool standby);

    QVector<int>  modulePostResult;     // module power on selt-test result
    QThread *workerThread;
    WorkMode workMode;
    TimeFormat timeFormat;
    DateFormat dateFormat;
    int backlightFd;                    // 背光控制文件句柄。
#ifdef Q_WS_QWS
    bool isTouchScreenOn;
#endif
    bool isStandby;
    bool isTurnOff;
    bool isAutoBrightness;
};

/**************************************************************************************************
 * 功能： 获取屏幕像素点之间的点距。
 *************************************************************************************************/
float SystemManager::getScreenPixelWPitch(void)
{
    float pixelPitch = 0.284;
    int screenInfoIndex = 0;
    machineConfig.getNumValue("ScreenInfoEnable", screenInfoIndex);
    QStringList screenInfoList = machineConfig.getChildNodeNameList("ScreenInfo");
    QString screenInfoPath = QString("ScreenInfo|%1|ScreenPixelWPitch").arg(screenInfoList.at(screenInfoIndex));
    machineConfig.getNumValue(screenInfoPath, pixelPitch);
    return pixelPitch;
}

/**************************************************************************************************
 * 功能： 获取屏幕像素点之间的点距。
 *************************************************************************************************/
float SystemManager::getScreenPixelHPitch(void)
{
    float pixelPitch = 0.284;
    int screenInfoIndex = 0;
    machineConfig.getNumValue("ScreenInfoEnable", screenInfoIndex);
    QStringList screenInfoList = machineConfig.getChildNodeNameList("ScreenInfo");
    QString screenInfoPath = QString("ScreenInfo|%1|ScreenPixelHPitch").arg(screenInfoList.at(screenInfoIndex));
    machineConfig.getNumValue(screenInfoPath, pixelPitch);
    return pixelPitch;
}

float SystemManager::getScreenSize()
{
    float screenSize = 10.1;
    int screenInfoIndex = 0;
    machineConfig.getNumValue("ScreenInfoEnable", screenInfoIndex);
    QStringList screenInfoList = machineConfig.getChildNodeNameList("ScreenInfo");
    QString screenInfoPath = QString("ScreenInfo|%1|ScreenSize").arg(screenInfoList.at(screenInfoIndex));
    machineConfig.getNumValue(screenInfoPath, screenSize);
    return screenSize;
}

/**************************************************************************************************
 * 功能： 查询是否支持该功能。
 *************************************************************************************************/
bool SystemManager::isSupport(ConfiguredFuncs funcs) const
{
    QString path;
    switch (funcs)
    {
    case CONFIG_RESP:
        path = "RESPEnable";
        break;
    case CONFIG_ECG12LEADS:
        path = "ECG12LEADEnable";
        break;
    case CONFIG_SPO2:
        path = "SPO2Enable";
        break;
    case CONFIG_NIBP:
        path = "NIBPEnable";
        break;
    case CONFIG_CO2:
        path = "CO2Enable";
        break;
    case CONFIG_AG:
        path = "AGEnable";
        break;
    case CONFIG_CO:
        path = "COEnable";
        break;
    case CONFIG_IBP:
        path = "IBPEnable";
        break;
    case CONFIG_TEMP:
        path = "TEMPEnable";
        break;
    case CONFIG_PRINTER:
        path = "PrinterEnable";
        break;
    case CONFIG_WIFI:
        path = "WIFIEnable";
        break;
    case CONFIG_TOUCH:
        path = "TouchEnable";
        break;
    case CONFIG_O2:
        path = "O2Enable";
        break;
    case CONFIG_HDMI:
        path = "HDMIEnable";
        break;
    case CONFIG_NURSE_CALL:
        path = "NurseCallEnable";
        break;
    case CONFIG_ANALOG_OUTPUT:
        path = "AnalogOutputEnable";
        break;
    case CONFIG_SYNC_DEFIBRILLATION:
        path = "SyncDefibrillationEnable";
        break;
    case CONFIG_SPO2_HIGH_CONFIGURE:
        path = "SpO2ConfigureEnable";
        break;
    default:
        break;
    }

    bool enable = false;
    machineConfig.getModuleInitialStatus(path, &enable);
    return enable;
}

/**************************************************************************************************
 * 功能： 查询是否支持该功能。
 *************************************************************************************************/
bool SystemManager::isSupport(ParamID paramID) const
{
    QString path;
    switch (paramID)
    {
    case PARAM_DUP_ECG:
    case PARAM_ECG:
        return true;
    case PARAM_DUP_RESP:
    case PARAM_RESP:
        path = "RESPEnable";
        break;
    case PARAM_SPO2:
        path = "SPO2Enable";
        break;
    case PARAM_NIBP:
        path = "NIBPEnable";
        break;
    case PARAM_CO2:
        path = "CO2Enable";
        break;
    case PARAM_AG:
        path = "AGEnable";
        break;
    case PARAM_CO:
        path = "COEnable";
        break;
    case PARAM_IBP:
        path = "IBPEnable";
        break;
    case PARAM_TEMP:
        path = "TEMPEnable";
        break;
    case PARAM_O2:
        path = "O2Enable";
        break;
    default:
        break;
    }

    bool enable = false;
    machineConfig.getModuleInitialStatus(path, &enable);
    return enable;
}

#ifdef Q_WS_QWS
bool SystemManager::isTouchScreenOn() const
{
    return d_ptr->isTouchScreenOn;
}

void SystemManager::setTouchScreenOnOff(int sta)
{
    if (sta && isSupport(CONFIG_TOUCH))
    {
        d_ptr->isTouchScreenOn = true;
        QWSServer::instance()->openMouse();
    }
    else
    {
        d_ptr->isTouchScreenOn = false;
        QWSServer::instance()->closeMouse();
    }

    if (isSupport(CONFIG_TOUCH))
    {
        runningStatus.setTouchStatus(d_ptr->isTouchScreenOn);
    }
    else
    {
        runningStatus.clearTouchStatus();
    }

    // 将触摸屏使能的状态保存在系统配置文件中
    systemConfig.setNumValue("General|TouchScreen", static_cast<int>(d_ptr->isTouchScreenOn));

    return;
}

void SystemManager::configTouchScreen(int sta)
{
    QString fileName = "/etc/.using_capacitor_ts";
    QFile f(fileName);
    if (sta == TOUCHSCREEN_CAPACITIVE)
    {
        if (!f.open(QIODevice::ReadWrite))
        {
            qDebug() << "fail to create capacitive file " << fileName;
        }
    }
    else
    {
        if (f.exists())
        {
            f.remove();
        }
    }
}
#endif

/***************************************************************************************************
 * get the module config status
 **************************************************************************************************/
unsigned int SystemManager::getModuleConfig() const
{
    short status = 0;
    unsigned short config = CONFIG_RESP;
    while (config && config <= CONFIG_WIFI)
    {
        if (isSupport((ConfiguredFuncs)config))
        {
            status |= config;
        }
        config <<= 1;
    }
    return status;
}

/***************************************************************************************************
 * set module power on test result
 **************************************************************************************************/
void SystemManager::setPoweronTestResult(ModulePoweronTestResult module,
        ModulePoweronTestStatus result)
{
    if (module >= MODULE_POWERON_TEST_RESULT_NR || result >= SELFTEST_STATUS_NR)
    {
        return;
    }

    if (result == d_ptr->modulePostResult[module])
    {
        return;
    }

    if (SELFTEST_SUCCESS == d_ptr->modulePostResult[module])
    {
        return;
    }

    if (SELFTEST_SUCCESS == result && d_ptr->modulePostResult[module] == SELFTEST_UNKNOWN)
    {
        d_ptr->modulePostResult[module] = result;
        return;
    }

    if (SELFTEST_MODULE_RESET != result)
    {
        d_ptr->modulePostResult[module] = result;
    }
    else
    {
        d_ptr->modulePostResult[module] = SELFTEST_UNKNOWN;
    }

    if (NULL == systemSelftestMessage[result][module])
    {
        return;
    }
}

bool SystemManager::isNeonateMachine()
{
    int index = 0;
    machineConfig.getNumValue("NeonateMachine", index);
    return index == 1;
}

/***************************************************************************************************
 * 按键协议解析。
 **************************************************************************************************/
void SystemManager::parseKeyValue(const unsigned char *data, unsigned int len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
#ifdef Q_WS_QWS
    if ((NULL == data) || (len != 2))
    {
        return;
    }

    int keyCode;

    switch (data[0])
    {
    case ',':
        keyCode = Qt::Key_Left;
        break;
    case '.':
        keyCode = Qt::Key_Right;
        break;
    case 'r':
        keyCode = Qt::Key_Return;
        break;
    case 'a':
        keyCode = Qt::Key_F1;
        break;
    case 'b':
        keyCode = Qt::Key_F2;
        break;
    case 'c':
        keyCode = Qt::Key_F3;
        break;
    case 'd':
        keyCode = Qt::Key_F4;
        break;
    case 'e':
        keyCode = Qt::Key_F5;
        break;
    case 'f':
        keyCode = Qt::Key_F6;
        break;
    default:
        qdebug("unkown keyCode");
        return;
    }

    if (QApplication::activeWindow() == NULL && windowManager.topWindow() == NULL)
    {
        windowManager.activateWindow();
        qDebug() << Q_FUNC_INFO << "No active window, activate window manager";
    }

    QWSServer::processKeyEvent(0xffff, keyCode, Qt::NoModifier, data[1], false);
#endif
}

/***************************************************************************************************
 * 背光亮度设置。
 **************************************************************************************************/
void SystemManager::setBrightness(BrightnessLevel br)
{
    if (br != BRT_LEVEL_AUTO)
    {
        int brValue = br * 2;
        enableBrightness(static_cast<BrightnessLevel>(brValue));
        d_ptr->isAutoBrightness = false;
    }
    else
    {
        d_ptr->isAutoBrightness = true;
    }
    systemConfig.setNumValue("General|DefaultDisplayBrightness", static_cast<int>(br));
}

void SystemManager::setAutoBrightness(BrightnessLevel br)
{
    if (d_ptr->isAutoBrightness && !nightModeManager.nightMode())
    {
        enableBrightness(br);
    }
}

/***************************************************************************************************
 * 背光亮度设置。
 **************************************************************************************************/
void SystemManager::enableBrightness(BrightnessLevel br)
{
#ifdef Q_WS_X11
    Q_UNUSED(br)
#else
    if (d_ptr->backlightFd < 0)
    {
        return;
    }

    // br value takes the valid range.
    br = br < BRT_LEVEL_0 ? BRT_LEVEL_0 : br;
    br = br >= BRIGHTNESS_NR ? static_cast<BrightnessLevel>(BRIGHTNESS_NR - 1) : br;

    // add screen type select
    char *lightValue = NULL;
    char industrialLight[BRIGHTNESS_NR] = {1, 7, 15, 20, 25, 32, 38, 42, 46, 50,
                                           54, 58, 62, 67, 72, 79, 85, 92, 96, 100};
    char businessLight[BRIGHTNESS_NR] = {70, 64, 58, 52, 50, 47, 44, 41, 38, 36,
                                         33, 31, 28, 26, 24, 21, 18, 15, 7, 1};
    int index = BACKLIGHT_MODE_1;
    machineConfig.getNumValue("BacklightAdjustment", index);
    if (static_cast<BacklightAdjustment>(index) == BACKLIGHT_MODE_2)
    {
        lightValue = reinterpret_cast<char*>(&industrialLight);
    }
    else
    {
        lightValue = reinterpret_cast<char*>(&businessLight);
    }

    int brValue = *(lightValue + br);

    QString str = QString::number(brValue);

    int ret = write(d_ptr->backlightFd, qPrintable(str), str.length() + 1);

    if (ret < 0)
    {
        debug("Set brightness failed!");
    }
#endif
}

/***************************************************************************************************
 * 获取背光亮度。
 **************************************************************************************************/
BrightnessLevel SystemManager::getBrightness(void)
{
    int b = BRT_LEVEL_4;
    systemConfig.getNumValue("General|DefaultDisplayBrightness", b);
    return static_cast<BrightnessLevel>(b);
}

bool SystemManager::isGoingToTrunOff() const
{
    return d_ptr->isTurnOff;
}

void SystemManager::turnOff(bool flag)
{
    if (d_ptr->isTurnOff == flag)
    {
        return;
    }

    d_ptr->isTurnOff = flag;
    if (flag)
    {
        qDebug() << "System is going to turn off.";
        Util::waitInEventLoop(1000);  // wait long enough to let the world get the message
    }
}

WorkMode SystemManager::getCurWorkMode() const
{
    return d_ptr->workMode;
}

void SystemManager::setWorkMode(WorkMode workmode)
{
    if (d_ptr->workMode == workmode)
    {
        return;
    }

    switch (workmode)
    {
    case WORK_MODE_NORMAL:
        d_ptr->workMode = workmode;
        d_ptr->enterNormalMode();
        break;
    case WORK_MODE_DEMO:
        d_ptr->workMode = workmode;
        d_ptr->enterDemoMode();
        break;
    case WORK_MODE_STANDBY:
    {
        if (d_ptr->workMode != WORK_MODE_DEMO)
        {
            d_ptr->workMode = workmode;
            d_ptr->enterStandbyMode();
        }
        break;
    }
    default:
        break;
    }

    // 切换模式时,清空当前正在储存的事件
    EventStorageManagerInterface *eventStorageManager = EventStorageManagerInterface::getEventStorageManager();
    if (eventStorageManager)
    {
        eventStorageManager->clearEventItemList();
    }

    // 添加当前工作模式改变信号，用作切换模式下的其他参数的及时更新信号
    emit workModeChanged(workmode);
}

void SystemManager::setSystemTimeFormat(const TimeFormat &format)
{
    if (d_ptr->timeFormat == format || format >= TIME_FORMAT_NR)
    {
        return;
    }
    d_ptr->timeFormat = format;
    timeDate->setTimeFormat(format);
    systemConfig.setNumValue("DateTime|TimeFormat", static_cast<int>(format));
    emit systemTimeFormatUpdated(format);
}

TimeFormat SystemManager::getSystemTimeFormat() const
{
    return d_ptr->timeFormat;
}

void SystemManager::setSystemDateFormat(const DateFormat &format)
{
    if (d_ptr->dateFormat == format || format >= DATE_FORMAT_NR)
    {
        return;
    }
    d_ptr->dateFormat = format;
    timeDate->setDateFormat(format);
    systemConfig.setNumValue("DateTime|DateFormat", static_cast<int>(format));
}

DateFormat SystemManager::getSystemDateFormat() const
{
    return d_ptr->dateFormat;
}

void SystemManagerPrivate::setStandbyStatus(bool standby)
{
    isStandby = standby;
    if (standby)
    {
        paramManager.disconnectParamProvider(WORK_MODE_STANDBY);
    }
    else
    {
        paramManager.connectParamProvider(WORK_MODE_STANDBY);
    }
}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
SystemManager::SystemManager() :  //申请一个动态的模块加载结果数组
    d_ptr(new SystemManagerPrivate())
{
    // 打开背光灯文件描述符
    d_ptr->backlightFd = open(BACKLIGHT_DEV, O_WRONLY | O_NONBLOCK);
    if (d_ptr->backlightFd < 0)
    {
        debug("Open %s failed: %s\n", BACKLIGHT_DEV, strerror(errno));
    }

    // 查询配置文件中是否支持下列项目
    if (!isSupport(CONFIG_SPO2))
    {
        d_ptr->modulePostResult[TS3_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
        d_ptr->modulePostResult[S5_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
    }
    if (!isSupport(CONFIG_NIBP))
    {
        d_ptr->modulePostResult[N5_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
    }

    if (!isSupport(CONFIG_TEMP))
    {
        d_ptr->modulePostResult[TT3_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
        d_ptr->modulePostResult[T5_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
    }

    // 暂时没有自检
    d_ptr->modulePostResult[CO2_MODULE_SELFTEST_RESULT] = SELFTEST_SUCCESS;

    d_ptr->isTurnOff = false;

    if (isSupport(CONFIG_HDMI))
    {
        d_ptr->workerThread = new QThread();
        d_ptr->workerThread->setObjectName("HDMICtrl");
        TDA19988Ctrl *hdmiCtrl = new TDA19988Ctrl();
        hdmiCtrl->moveToThread(d_ptr->workerThread);
        hdmiCtrl->connect(d_ptr->workerThread, SIGNAL(finished()), hdmiCtrl, SLOT(deleteLater()));
        d_ptr->workerThread->start();
    }

    /* load the time foramt */
    int timeformat = TIME_FORMAT_12;
    systemConfig.getNumValue("DateTime|TimeFormat", timeformat);
    if (timeformat < TIME_FORMAT_NR && timeformat >=0)
    {
        d_ptr->timeFormat = static_cast<TimeFormat>(timeformat);
    }
    timeDate->setTimeFormat(d_ptr->timeFormat);

    /* load the date format */
    int dateformat = DATE_FORMAT_Y_M_D;
    systemConfig.getNumValue("DateTime|DateFormat", dateformat);
    if (dateformat < DATE_FORMAT_NR && dateformat >=0)
    {
        d_ptr->dateFormat = static_cast<DateFormat>(dateformat);
    }
    timeDate->setDateFormat(d_ptr->dateFormat);

#ifdef Q_WS_QWS
    int val = 0;
    machineConfig.getNumValue("TouchEnable", val);
    configTouchScreen(val);
    setTouchScreenOnOff(val);
#endif
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
SystemManager &SystemManager::getInstance()
{
    static SystemManager * instance = NULL;
    if (instance == NULL)
    {
        instance = new SystemManager();
        SystemManagerInterface *old = registerSystemManager(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

SystemManager::~SystemManager()
{
    if (d_ptr->backlightFd != -1)
    {
        close(d_ptr->backlightFd);
    }

    d_ptr->modulePostResult.clear();

    if (NULL != d_ptr->workerThread)
    {
        d_ptr->workerThread->quit();
        d_ptr->workerThread->wait();
    }
}
