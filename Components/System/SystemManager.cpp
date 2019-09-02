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
#include "WindowManager.h"
#include "ECGParam.h"
#include "CO2Param.h"
#include "RESPParam.h"
#include "AlarmStateMachine.h"
#include "SystemSelftestMenu.h"
#include "ComboListPopup.h"
#include "Utility.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include <QProcess>
#include <QDir>
#include <QKeyEvent>
#include "WindowManager.h"
#include "ParamManager.h"
#include "AlarmIndicator.h"
#include <QTimer>
#ifdef Q_WS_QWS
#include <QWSServer>
#include "RunningStatusBar.h"
#endif
#include "PatientManager.h"
#include "DataStorageDirManager.h"
#include "StandbyWindow.h"
#include "NIBPParam.h"
#include "LanguageManager.h"
#include "EventStorageManagerInterface.h"

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
          publishTestTimer(NULL), workerThread(NULL), selfTestResult(NULL),
      #ifdef Q_WS_X11
          ctrlSocket(NULL),
      #endif
          workMode(WORK_MODE_NORMAL), backlightFd(-1),
      #ifdef Q_WS_QWS
          isTouchScreenOn(false),
      #endif
          selfTestFinish(false),
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

    /**
     * @brief handleBMode handle the board mode
     */
    void handleBMode()
    {
        QDesktopWidget *pDesk = QApplication::desktop();

        // Layout & Show
        windowManager.move((pDesk->width() - windowManager.width()) / 2,
                           (pDesk->height() - windowManager.height()) / 2);

        // 显示界面界面。
        //    UserFaceType type = UFACE_MONITOR_STANDARD;

        // 处理CO2和RESP的使能。
        //    _handleCO2RESP();//因调试需要，临时关闭

        // 立即刷新界面，防止界面残留
        QApplication::processEvents(QEventLoop::ExcludeSocketNotifiers |
                                    QEventLoop::ExcludeUserInputEvents);

        // 清除下拉列表
        if (ComboListPopup::current())
        {
            ComboListPopup::current()->close();
        }

        // 清除上个模式留下的弹出框
        while (NULL != QApplication::activeModalWidget())
        {
            QApplication::activeModalWidget()->hide();
            menuManager.close();
        }
    }

    QVector<int>  modulePostResult;     // module power on selt-test result
    QTimer *publishTestTimer;
    QThread *workerThread;
    SystemSelftestMenu *selfTestResult;
#ifdef Q_WS_X11
    QTcpSocket *ctrlSocket;
    QQueue<char> socketInfoData;
#endif
    WorkMode workMode;
    int backlightFd;                    // 背光控制文件句柄。
#ifdef Q_WS_QWS
    bool isTouchScreenOn;
#endif
    bool selfTestFinish;
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
    case CONFIG_NURSECALL:
        path = "NurseCallEnable";
        break;
    case CONFIG_O2:
        path = "O2Enable";
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

    if (d_ptr->selfTestFinish)
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

    if (d_ptr->selfTestResult->isVisible())
    {
        d_ptr->selfTestResult->appendInfo(module, result, trs(systemSelftestMessage[result][module]));
    }
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

    if (QApplication::activeWindow() == NULL)
    {
        windowManager.activateWindow();
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
        enableBrightness(brValue);
        d_ptr->isAutoBrightness = false;

        systemConfig.setNumValue("General|DefaultDisplayBrightness", static_cast<int>(br));
    }
    else
    {
        d_ptr->isAutoBrightness = true;
    }
}

void SystemManager::setAutoBrightness(int br)
{
    if (d_ptr->isAutoBrightness)
    {
        enableBrightness(br);
    }
}

/***************************************************************************************************
 * 背光亮度设置。
 **************************************************************************************************/
void SystemManager::enableBrightness(int br)
{
#ifdef Q_WS_X11
    QByteArray data;
    data.append(0x81);
    data.append(static_cast<char>(br));
    sendCommand(data);
#else
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

    if (d_ptr->backlightFd < 0)
    {
        return;
    }

    if (br == BRT_LEVEL_NR)
    {
        return;
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

/***************************************************************************************************
 * 加载初始底层模式。
 **************************************************************************************************/
void SystemManager::loadInitBMode()
{
    d_ptr->handleBMode();
}

/***************************************************************************************************
 * 是否确认了自检结果。
 **************************************************************************************************/
bool SystemManager::isAcknownledgeSystemTestResult()
{
    return !d_ptr->selfTestResult->isVisible();
}

bool SystemManager::isSystemSelftestOver() const
{
    return d_ptr->selfTestFinish;
}

void SystemManager::systemSelftestOver()
{
    d_ptr->selfTestFinish = true;
}

/***************************************************************************************************
 * hide system selftest dialog。
 **************************************************************************************************/
void SystemManager::closeSystemTestDialog()
{
    if (d_ptr->selfTestResult->isVisible())
    {
        d_ptr->selfTestResult->hide();
    }
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

#ifdef Q_WS_X11
bool SystemManager::sendCommand(const QByteArray &cmd)
{
    if (d_ptr->ctrlSocket->isValid() && d_ptr->ctrlSocket->state() == QAbstractSocket::ConnectedState)
    {
        d_ptr->ctrlSocket->write(cmd);
        d_ptr->ctrlSocket->waitForBytesWritten(1000);
        return true;
    }
    else
    {
        return false;
    }
}

#define SOCKET_INFO_PACKET_LENGHT 2
enum ControlInfo
{
    CTRL_INFO_METRONOME = 0x80,
};

void SystemManager::onCtrlSocketReadReady()
{
    QByteArray data = d_ptr->ctrlSocket->readAll();
    for (int i = 0; i < data.size(); i++)
    {
        d_ptr->socketInfoData.append(data.at(i));
    }

    while (d_ptr->socketInfoData.size() >= SOCKET_INFO_PACKET_LENGHT)
    {
        unsigned char infoType = d_ptr->socketInfoData.takeFirst();
        if (!(infoType & 0x80))
        {
            // not a info type
            continue;
        }
//        char infoData = d_ptr->socketInfoData.takeFirst();
        switch ((ControlInfo)infoType)
        {
        case CTRL_INFO_METRONOME:
        {
            emit metronomeReceived();
        }
        break;

        default:
            qdebug("unknown info type:%02x", infoType);
            break;
        }
    }
}
#endif

/***************************************************************************************************
 * publish test result time out。
 **************************************************************************************************/
void SystemManager::publishTestResult(void)
{
    if (!d_ptr->selfTestFinish)
    {
        int successCount = d_ptr->modulePostResult.count(SELFTEST_SUCCESS);
        int failCount = d_ptr->modulePostResult.count(SELFTEST_FAILED);
        int notsupportCount = d_ptr->modulePostResult.count(SELFTEST_NOT_SUPPORT);
        int notPerformCount = d_ptr->modulePostResult.count(SELFTEST_UNKNOWN);

        if (successCount + failCount + notsupportCount == MODULE_POWERON_TEST_RESULT_NR)
        {
            d_ptr->publishTestTimer->stop();
            if (MODULE_POWERON_TEST_RESULT_NR == (successCount + notsupportCount))
            {
                d_ptr->selfTestResult->testOver(true, trs("SystemSelfTestPass"));
                if (!d_ptr->selfTestResult->isVisible())
                {
                    // 清除下拉列表
                    if (ComboListPopup::current())
                    {
                        ComboListPopup::current()->close();
                    }

                    // 清除上个模式留下的弹出框
                    while (NULL != QApplication::activeModalWidget())
                    {
                        QApplication::activeModalWidget()->hide();
                        menuManager.close();
                    }

                    d_ptr->selfTestResult->show();
                }
            }
            else
            {
                // test fail
                QString str("");
                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[TE3_MODULE_SELFTEST_RESULT])
                {
                    str += trs("ECG");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[E5_MODULE_SELFTEST_RESULT])
                {
                    str += trs("ECG");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[N5_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != d_ptr->modulePostResult[N5_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("NIBP");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[TS3_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != d_ptr->modulePostResult[TS3_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("SPO2");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[S5_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != d_ptr->modulePostResult[S5_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("SPO2");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[TT3_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != d_ptr->modulePostResult[TT3_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("TEMP");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[T5_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != d_ptr->modulePostResult[T5_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("TEMP");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[CO2_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != d_ptr->modulePostResult[CO2_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("CO2");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[PRINTER72_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("Printer");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[PRINTER48_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("Printer");
                }

                if (SELFTEST_SUCCESS != d_ptr->modulePostResult[PANEL_KEY_POWERON_TEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("FrontPanelKey");
                }

                if (!str.isEmpty())
                {
                    str += trs("SelfTestFail");
                }

                d_ptr->selfTestResult->testOver(false, str);
                if (d_ptr->selfTestResult->isVisible())
                {
                    return;
                }

                // 清除下拉列表
                if (ComboListPopup::current())
                {
                    ComboListPopup::current()->close();
                }

                // 清除上个模式留下的弹出框
                while (NULL != QApplication::activeModalWidget())
                {
                    QApplication::activeModalWidget()->hide();
                    menuManager.close();
                }

                d_ptr->selfTestResult->show();
            }
        }
        else if (successCount + notPerformCount != MODULE_POWERON_TEST_RESULT_NR)
        {
            if (d_ptr->selfTestResult->isVisible())
            {
                return;
            }

            bool showDialog = false;
            for (int i = TE3_MODULE_SELFTEST_RESULT; i < MODULE_POWERON_TEST_RESULT_NR; ++i)
            {
                if (d_ptr->modulePostResult[i] == SELFTEST_SUCCESS ||
                        d_ptr->modulePostResult[i] == SELFTEST_NOT_SUPPORT ||
                        d_ptr->modulePostResult[i] == SELFTEST_UNKNOWN)
                {
                    continue;
                }

                if (NULL != systemSelftestMessage[d_ptr->modulePostResult[i]][i])
                {
                    showDialog = true;
                    d_ptr->selfTestResult->appendInfo((ModulePoweronTestResult) i,
                                                      (ModulePoweronTestStatus)d_ptr->modulePostResult[i],
                                                trs(systemSelftestMessage[d_ptr->modulePostResult[i]][i]));
                }
            }

            if (showDialog)
            {
                // 清除下拉列表
                if (ComboListPopup::current())
                {
                    ComboListPopup::current()->close();
                }

                // 清除上个模式留下的弹出框
                while (NULL != QApplication::activeModalWidget())
                {
                    QApplication::activeModalWidget()->hide();
                    menuManager.close();
                }

                d_ptr->selfTestResult->show();
            }
        }
    }
    else
    {
        if (!d_ptr->selfTestResult->isVisible())
        {
            // 清除下拉列表
            if (ComboListPopup::current())
            {
                ComboListPopup::current()->close();
            }

            // 清除上个模式留下的弹出框
            while (NULL != QApplication::activeModalWidget())
            {
                QApplication::activeModalWidget()->hide();
                menuManager.close();
            }

            d_ptr->selfTestResult->show();
        }
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

    // 构建一个500ms的轮询函数接口_publishTestResult()--发布测试结果
    d_ptr->publishTestTimer = new QTimer();
    d_ptr->publishTestTimer->setInterval(500);
    connect(d_ptr->publishTestTimer, SIGNAL(timeout()), this, SLOT(publishTestResult()));

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

    d_ptr->selfTestResult = new SystemSelftestMenu();
    d_ptr->selfTestFinish = false;
    d_ptr->isTurnOff = false;

    d_ptr->workerThread = new QThread();
    TDA19988Ctrl *hdmiCtrl = new TDA19988Ctrl();
    hdmiCtrl->moveToThread(d_ptr->workerThread);
    hdmiCtrl->connect(d_ptr->workerThread, SIGNAL(finished()), hdmiCtrl, SLOT(deleteLater()));
    d_ptr->workerThread->start();

#ifdef Q_WS_QWS
    int val = 0;
    machineConfig.getNumValue("TouchEnable", val);
    setTouchScreenOnOff(val);
#endif

#ifdef Q_WS_X11
    d_ptr->ctrlSocket = new QTcpSocket(this);
    d_ptr->ctrlSocket->connectToHost("192.168.10.2", 8088);
    d_ptr->ctrlSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(d_ptr->ctrlSocket, SIGNAL(readyRead()), this, SLOT(onCtrlSocketReadReady()));
    if (!d_ptr->ctrlSocket->waitForConnected(1000))
    {
        qdebug("connect to control server failed!");
    }
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
#ifdef Q_WS_X11
    delete d_ptr->ctrlSocket;
#endif

    if (d_ptr->backlightFd != -1)
    {
        close(d_ptr->backlightFd);
    }

    d_ptr->modulePostResult.clear();

    if (NULL != d_ptr->publishTestTimer)
    {
        delete d_ptr->publishTestTimer;
        d_ptr->publishTestTimer = NULL;
    }

    if (NULL != d_ptr->selfTestResult)
    {
        delete d_ptr->selfTestResult;
        d_ptr->selfTestResult = NULL;
    }

    if (NULL != d_ptr->workerThread)
    {
        d_ptr->workerThread->quit();
        d_ptr->workerThread->wait();
    }
}
