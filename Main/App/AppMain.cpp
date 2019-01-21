/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/19
 **/

#include <QDesktopWidget>
#include "IApplication.h"
#include "FloatHandle.h"
#include "Starter.h"
#include "Debug.h"
#include "Init.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include "LayoutManager.h"
#include <unistd.h>

static IThread *_storageThread = NULL;
static QThread *_networkThread = NULL;

/**************************************************************************************************
 * 功能： 周期性任务，1秒执行一次。
 *************************************************************************************************/
static void _taskOneSec1(void)
{
    timeManager.recordRunTime();
    paramManager.checkProviderConnection();
    systemBoardProvider.checkConnection();
}

/**************************************************************************************************
 * 功能： 周期性任务，1s执行一次。
 *************************************************************************************************/
static void _taskOneSec2(void)
{
    // summaryStorageManager.checkCompletedItem();
    eventStorageManager.checkCompletedEvent();
    powerManger.run();

    if (!usbManager.isUSBExist())
    {
        sysStatusBar.hideIcon(SYSTEM_ICON_LABEL_USB);
    }
    else
    {
        sysStatusBar.changeIcon(SYSTEM_ICON_LABEL_USB, SYSTEM_ICON_USB_CONNECTED);
    }
}

/**************************************************************************************************
 * 功能： 周期性任务，50ms执行一次。
 *************************************************************************************************/
static void _task50MSec0(void)
{
    // 更新界面显示。
    static unsigned t = 0;
    if (0 != t)
    {
        double diff_t = timeDate.difftime(timeDate.time(), t);
        t += static_cast<int>(diff_t);
        if (isZero(diff_t))
        {
            return;
        }
    }
    else
    {
        t = timeDate.time();
    }

    timeManager.mainRun(t);
}

/**************************************************************************************************
 * 功能： 周期性任务，500ms执行一次。
 *************************************************************************************************/
static void _task500MSec1(void)
{
    nibpParam.machineRun();
}

/**************************************************************************************************
 * 功能： 周期性任务，50ms执行一次。
 *
 *************************************************************************************************/
static void _task50MSec(void)
{
    static unsigned t = 0;
    unsigned curT = timeManager.getCurTime();
    if (t == 0)
    {
        t = curT;
    }

    // current time might be changed, so we get the abstract value.
    // one second has passed
    if (qAbs(curT - t) < 1)
    {
        return;
    }

    // 数据收集
    trendCache.collectTrendData(t);

    /*
     * The paramDataStorageManager should run as follow order
     * store nibp measurement, handle alarm, handle 30 seconds trend
     */

    // paramDataStorageManager.addNIBPData(t);

    // 报警处理
    alertor.mainRun(t);

    // collect alarm status
    trendCache.collectTrendAlarmStatus(t);

    trendDataStorageManager.periodRun(t);
    paramManager.showSubParamValue();

    t = curT;
}

/**************************************************************************************************
 * 存储线程。
 *************************************************************************************************/
static void _storageThreadEntry(void)
{
    // 保存配置。
    systemConfig.saveToDisk();
    currentConfig.saveToDisk();

    static int loopCounter = 0;
    loopCounter++;

    // 数据存储
    static bool cleanup = true;
    if (cleanup)
    {
        DataStorageDirManager::cleanUpLastIncidentDir();
        cleanup = false;
    }

    if (!systemManager.isGoingToTrunOff())
    {
        trendDataStorageManager.run();
    }


    if (!systemManager.isGoingToTrunOff())
    {
        eventStorageManager.run();
    }


    if (!systemManager.isGoingToTrunOff())
    {
        rawDataCollector.run();
    }

    if (!systemManager.isGoingToTrunOff())
    {
        errorLog.run();
    }

    IThread::msleep(300);
}

/**************************************************************************************************
 * 功能： 初始化任务。
 *************************************************************************************************/
static void _initTasks(void)
{
    systemTick.addHook(50, 0, _task50MSec0);
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        systemTick.addHook(500, 200, _task500MSec1);
    }
    systemTick.addHook(50, 0, _task50MSec);

//    systemTick.addHook(1000, 0, _taskOneSec0);
    systemTick.addHook(1000, 100, _taskOneSec1);
    systemTick.addHook(1000, 200, _taskOneSec2);

    // 初始化所有的线程。
    _storageThread = new IThread("storage", _storageThreadEntry);

    _networkThread = new QThread();
    _networkThread->setObjectName("Network");
    networkManager.moveToThread(_networkThread);
    QObject::connect(_networkThread, SIGNAL(finished()), &networkManager, SLOT(deleteLater()));
}

/**************************************************************************************************
 * 功能： 启动。
 *************************************************************************************************/
static void _start(void)
{
    // 开启系统定时器运行（50ms）
    systemTick.startTick();

    _storageThread->start();

    _networkThread->start();
    _networkThread->setPriority(QThread::IdlePriority);


#if 1
    // load softkey
    softkeyManager.setContent(SOFTKEY_ACTION_STANDARD);

    // perform window layout
    windowManager.setLayout(layoutManager.mainLayout());
    layoutManager.mainLayout()->activate();

    // move window to the center
    QDesktopWidget *pDesk = QApplication::desktop();
    windowManager.move((pDesk->width() - windowManager.width()) / 2,
                       (pDesk->height() - windowManager.height()) / 2);

    int type = 0;
    systemConfig.getNumValue("UserFaceType", type);
    layoutManager.setUFaceType(static_cast<UserFaceType>(type));

#else
    systemManager.loadInitBMode();
#endif
}

/**************************************************************************************************
 * 功能： 停止。
 *************************************************************************************************/
static void _stop(void)
{
    // 停止打印。
    recorderManager.abort();

    _storageThread->stop();
    _networkThread->quit();

    // 删除线程对象。
    delete _storageThread;
    // network thread might be still running, need to delete later
    _networkThread->deleteLater();
}

/**************************************************************************************************
 * 功能： 主函数入口。
 *************************************************************************************************/
void appMain(IApplication &app)
{
    // 创建对象。
    newObjects();
    _initTasks();

    _start();            // 启动子系统。

    app.exec();          // 执行主循环。
    _stop();             // 停止子系统。

    // 退出前释放创建的对象。
    deleteObjects();
}
