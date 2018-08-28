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
#include "Version.h"
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
#endif

#define BACKLIGHT_DEV   "/etc/brightness"       // 背光控制文件接口

const char *systemSelftestMessage[SELFTEST_STATUS_NR][MODULE_POWERON_TEST_RESULT_NR] =
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


const char *systemSelftestTimeOut[MODULE_POWERON_TEST_RESULT_NR] =
{
    "ECG Module Selftest Fail",
    "NIBP Module Selftest Fail",
    "SPO2 Module Selftest Fail",
    "TEMP Module Selftest Fail",
    "CO2 Module Selftest Fail",
    "Printer Selftest Fail",
    "Front Panel Key test Fail",
};

SystemManager *SystemManager::_selfObj = NULL;

/**************************************************************************************************
 * 功能： 获取屏幕像素点之间的点距。
 *************************************************************************************************/
float SystemManager::getScreenPixelWPitch(void)
{
    float pixelPitch = 0.284;
    machineConfig.getNumValue("ScreenPixelWPitch", pixelPitch);
    return pixelPitch;
}

/**************************************************************************************************
 * 功能： 获取屏幕像素点之间的点距。
 *************************************************************************************************/
float SystemManager::getScreenPixelHPitch(void)
{
    float pixelPitch = 0.284;
    machineConfig.getNumValue("ScreenPixelHPitch", pixelPitch);
    return pixelPitch;
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
    case CONFIG_WIFI:
        path = "WIFIEnable";
        break;
    default:
        break;
    }

    bool enable = false;
    machineConfig.getNumValue(path, enable);
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
    default:
        break;
    }

    bool enable = false;
    machineConfig.getNumValue(path, enable);
    return enable;
}

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

    if (result == _modulePostResult[module])
    {
        return;
    }

    if (_selfTestFinish)
    {
        return;
    }

    if (SELFTEST_SUCCESS == _modulePostResult[module])
    {
        return;
    }

    if (SELFTEST_SUCCESS == result && _modulePostResult[module] == SELFTEST_UNKNOWN)
    {
        _modulePostResult[module] = result;
        return;
    }

    if (SELFTEST_MODULE_RESET != result)
    {
        _modulePostResult[module] = result;
    }
    else
    {
        _modulePostResult[module] = SELFTEST_UNKNOWN;
    }

    if (NULL == systemSelftestMessage[result][module])
    {
        return;
    }

    if (_selfTestResult->isVisible())
    {
        _selfTestResult->appendInfo(module, result, trs(systemSelftestMessage[result][module]));
    }
}

/***************************************************************************************************
 * 按键协议解析。
 **************************************************************************************************/
void SystemManager::parseKeyValue(const unsigned char *data, unsigned int len)
{
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
    enableBrightness(br);

    currentConfig.setNumValue("General|DefaultDisplayBrightness", static_cast<int>(br));
}

/***************************************************************************************************
 * 背光亮度设置。
 **************************************************************************************************/
void SystemManager::enableBrightness(BrightnessLevel br)
{
#ifdef Q_WS_X11
    QByteArray data;
    data.append(0x81);
    data.append(static_cast<char>(br));
    sendCommand(data);
#else
    if (_backlightFd < 0)
    {
        return;
    }

    if (br == BRT_LEVEL_NR)
    {
        debug("Invalid brightness!");
        return;
    }

    int brValue = 255 * (br + 1) / BRT_LEVEL_NR;
    char str[8];
    snprintf(str, sizeof(str), "%d", brValue);
    int ret = write(_backlightFd, str, sizeof(str));
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
    currentConfig.getNumValue("General|DefaultDisplayBrightness", b);
    return (BrightnessLevel)b;
}

/***************************************************************************************************
 * 加载初始底层模式。
 **************************************************************************************************/
void SystemManager::loadInitBMode()
{
    _handleBMode();
}

/***************************************************************************************************
 * 获取软件版本。
 **************************************************************************************************/
void SystemManager::getSoftwareVersion(QString &revision)
{
    revision.clear();
    revision += getSoftwareVersionNum();
    revision += ",\r\r\r";
    revision += BUILD_TIME;
}

/***************************************************************************************************
 * 获取软件版本。
 **************************************************************************************************/
QString SystemManager::getSoftwareVersionNum()
{
    QString version(IDM_SOFTWARE_VERSION);
    version += ".";
    version += SVN_VERSION;

    return version;
}

/***************************************************************************************************
 * 是否确认了自检结果。
 **************************************************************************************************/
bool SystemManager::isAcknownledgeSystemTestResult()
{
    return !_selfTestResult->isVisible();
}

/***************************************************************************************************
 * hide system selftest dialog。
 **************************************************************************************************/
void SystemManager::closeSystemTestDialog()
{
    if (_selfTestResult->isVisible())
    {
        _selfTestResult->hide();
    }
}

void SystemManager::turnOff(bool flag)
{
    if (_isTurnOff == flag)
    {
        return;
    }

    _isTurnOff = flag;
    if (flag)
    {
        qDebug() << "System is going to turn off.";
        Util::waitInEventLoop(1000); // wait long enough to let the world get the message
    }
}

WorkMode SystemManager::getCurWorkMode() const
{
    return _workMode;
}

void SystemManager::setWorkMode(WorkMode workmode)
{
    if (_workMode == workmode)
    {
        return;
    }

    switch (workmode)
    {
    case WORK_MODE_NORMAL:
        enterNormalMode();
        break;
    case WORK_MODE_DEMO:
        enterDemoMode();
        break;
    default:
        break;
    }

    _workMode = workmode;
}

/***************************************************************************************************
 * system is support the test module
 **************************************************************************************************/
bool SystemManager::_isTestModuleSupport(ModulePoweronTestResult module)
{
    switch (module)
    {
    default:
        return true;
    }
}

void SystemManager::enterDemoMode()
{
    alarmIndicator.delAllPhyAlarm();
    windowManager.showDemoWidget(true);
    paramManager.connectDemoParamProvider();
}

void SystemManager::enterNormalMode()
{
    alarmIndicator.delAllPhyAlarm();
    windowManager.showDemoWidget(false);
    paramManager.connectParamProvider();
}

#ifdef Q_WS_X11
bool SystemManager::sendCommand(const QByteArray &cmd)
{
    if (_ctrlSocket->isValid() && _ctrlSocket->state() == QAbstractSocket::ConnectedState)
    {
        _ctrlSocket->write(cmd);
        _ctrlSocket->waitForBytesWritten(1000);
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
    QByteArray data = _ctrlSocket->readAll();
    for (int i = 0; i < data.size(); i++)
    {
        _socketInfoData.append(data.at(i));
    }

    while (_socketInfoData.size() >= SOCKET_INFO_PACKET_LENGHT)
    {
        unsigned char infoType = _socketInfoData.takeFirst();
        if (!(infoType & 0x80))
        {
            // not a info type
            continue;
        }
        char infoData = _socketInfoData.takeFirst();
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
void SystemManager::_publishTestResult(void)
{
    if (!_selfTestFinish)
    {
        int successCount = _modulePostResult.count(SELFTEST_SUCCESS);
        int failCount = _modulePostResult.count(SELFTEST_FAILED);
        int notsupportCount = _modulePostResult.count(SELFTEST_NOT_SUPPORT);
        int notPerformCount = _modulePostResult.count(SELFTEST_UNKNOWN);

        if (successCount + failCount + notsupportCount == MODULE_POWERON_TEST_RESULT_NR)
        {
            _publishTestTimer->stop();
            if (MODULE_POWERON_TEST_RESULT_NR == (successCount + notsupportCount))
            {
                _selfTestResult->testOver(true, trs("SystemSelfTestPass"));
                if (!_selfTestResult->isVisible())
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

                    _selfTestResult->show();
                }
            }
            else
            {
                // test fail
                QString str("");
                if (SELFTEST_SUCCESS != _modulePostResult[TE3_MODULE_SELFTEST_RESULT])
                {
                    str += trs("ECG");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[E5_MODULE_SELFTEST_RESULT])
                {
                    str += trs("ECG");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[TN3_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != _modulePostResult[TN3_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("NIBP");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[TS3_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != _modulePostResult[TS3_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("SPO2");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[S5_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != _modulePostResult[S5_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("SPO2");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[TT3_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != _modulePostResult[TT3_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("TEMP");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[T5_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != _modulePostResult[T5_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("TEMP");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[CO2_MODULE_SELFTEST_RESULT] &&
                        SELFTEST_NOT_SUPPORT != _modulePostResult[CO2_MODULE_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("CO2");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[PRINTER72_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("Printer");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[PRINTER48_SELFTEST_RESULT])
                {
                    if (!str.isEmpty())
                    {
                        str += ",";
                    }

                    str += trs("Printer");
                }

                if (SELFTEST_SUCCESS != _modulePostResult[PANEL_KEY_POWERON_TEST_RESULT])
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

                _selfTestResult->testOver(false, str);
                if (_selfTestResult->isVisible())
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

                _selfTestResult->show();
            }
        }
        else if (successCount + notPerformCount != MODULE_POWERON_TEST_RESULT_NR)
        {
            if (_selfTestResult->isVisible())
            {
                return;
            }

            bool showDialog = false;
            for (int i = TE3_MODULE_SELFTEST_RESULT; i < MODULE_POWERON_TEST_RESULT_NR; ++i)
            {
                if (_modulePostResult[i] == SELFTEST_SUCCESS ||
                        _modulePostResult[i] == SELFTEST_NOT_SUPPORT ||
                        _modulePostResult[i] == SELFTEST_UNKNOWN)
                {
                    continue;
                }

                if (NULL != systemSelftestMessage[_modulePostResult[i]][i]
                        && _isTestModuleSupport((ModulePoweronTestResult)i))
                {
                    showDialog = true;
                    _selfTestResult->appendInfo((ModulePoweronTestResult) i, (ModulePoweronTestStatus)_modulePostResult[i],
                                                trs(systemSelftestMessage[_modulePostResult[i]][i]));
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

                _selfTestResult->show();
            }
        }
    }
    else
    {
        if (!_selfTestResult->isVisible())
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

            _selfTestResult->show();
        }
    }
}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
SystemManager::SystemManager() ://申请一个动态的模块加载结果数组
    _modulePostResult(MODULE_POWERON_TEST_RESULT_NR, SELFTEST_UNKNOWN),
    _workMode(WORK_MODE_NORMAL)
{
    // 打开背光灯文件描述符
    _backlightFd = open(BACKLIGHT_DEV, O_WRONLY | O_NONBLOCK);
    if (_backlightFd < 0)
    {
        debug("Open %s failed: %s\n", BACKLIGHT_DEV, strerror(errno));
    }

    // 构建一个500ms的轮询函数接口_publishTestResult()--发布测试结果
    _publishTestTimer = new QTimer();
    _publishTestTimer->setInterval(500);
    connect(_publishTestTimer, SIGNAL(timeout()), this, SLOT(_publishTestResult()));

    // 查询配置文件中是否支持下列项目
    if (!isSupport(CONFIG_SPO2))
    {
        _modulePostResult[TS3_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
        _modulePostResult[S5_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
    }
    if (!isSupport(CONFIG_NIBP))
    {
        _modulePostResult[TN3_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
    }

    if (!isSupport(CONFIG_TEMP))
    {
        _modulePostResult[TT3_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
        _modulePostResult[T5_MODULE_SELFTEST_RESULT] = SELFTEST_NOT_SUPPORT;
    }

    // 暂时没有自检
    _modulePostResult[CO2_MODULE_SELFTEST_RESULT] = SELFTEST_SUCCESS;

    _selfTestResult = new SystemSelftestMenu();
    _selfTestFinish = false;
    _isTurnOff = false;

    _workerThread = new QThread();
    TDA19988Ctrl *hdmiCtrl = new TDA19988Ctrl();
    hdmiCtrl->moveToThread(_workerThread);
    hdmiCtrl->connect(_workerThread, SIGNAL(finished()), hdmiCtrl, SLOT(deleteLater()));
    _workerThread->start();


#ifdef Q_WS_X11
    _ctrlSocket = new QTcpSocket(this);
    _ctrlSocket->connectToHost("192.168.10.2", 8088);
    _ctrlSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(_ctrlSocket, SIGNAL(readyRead()), this, SLOT(onCtrlSocketReadReady()));
    if (!_ctrlSocket->waitForConnected(1000))
    {
        qdebug("connect to control server failed!");
    }
#endif
}


/**************************************************************************************************
 * 处理模式切换。
 *************************************************************************************************/
void SystemManager::_handleBMode(void)
{
    QDesktopWidget *pDesk = QApplication::desktop();

    // Layout & Show
    windowManager.move((pDesk->width() - windowManager.width()) / 2,
                       (pDesk->height() - windowManager.height()) / 2);

    if (ecgParam.getDisplayMode() == ECG_DISPLAY_12_LEAD_FULL)
    {
        ecgParam.setDisplayMode(ECG_DISPLAY_NORMAL, false);
    }

    // 显示界面界面。
    UserFaceType type = UFACE_MONITOR_STANDARD;

    // 处理CO2和RESP的使能。
//    _handleCO2RESP();//因调试需要，临时关闭

    windowManager.setUFaceType(type);

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

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
SystemManager::~SystemManager()
{
#ifdef Q_WS_X11
    delete _ctrlSocket;
#endif

    if (_backlightFd != -1)
    {
        close(_backlightFd);
    }

    _modulePostResult.clear();

    if (NULL != _publishTestTimer)
    {
        delete _publishTestTimer;
        _publishTestTimer = NULL;
    }

    if (NULL != _selfTestResult)
    {
        delete _selfTestResult;
        _selfTestResult = NULL;
    }

    if (NULL != _workerThread)
    {
        _workerThread->quit();
        _workerThread->wait();
    }
}
