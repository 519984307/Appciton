#include "Starter.h"
#include "IConfig.h"
#include "TimeManager.h"
#include "SystemManager.h"
#include "SystemBoardProvider.h"
#if defined(Q_WS_QWS)
#include <QWSServer>
#include <QBrush>
#endif
#include <QFile>
#include "ErrorLog.h"
#include "ErrorLogItem.h"

#define CRASH_LOG_PATH "/usr/local/nPM/bin/crashlog"

//check whether the program is crashed at last running process
static void checkCrash()
{
    if(QFile::exists(CRASH_LOG_PATH))
    {
        CrashLogItem *item = new CrashLogItem;
        item->setName("System Crash");
        item->collect(CRASH_LOG_PATH);
        errorLog.append(item);
        QFile::remove(CRASH_LOG_PATH);
    }
}

// 启动加载功能，它读取系统板的实际模式，并将其转换成真正系统需要的工作模式，并根据模式的类型调用不同的启动如果。
int main(int argc, char *argv[])
{
    IApplication app(argc, argv);
    catchFatalError();
    checkCrash();

    ErrorLogItem *item = new ErrorLogItem();
    item->setName("Power On");
    errorLog.append(item);

    // 构造出系统参数对象，用于获取当前面板上开关的位置。
    systemBoardProvider.construction();
    systemManager.construction();

    // 下面开始分析请求的模式，并决定系统最终的运行模式。
    appMain(app);
}
