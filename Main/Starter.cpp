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
    //检查指定路径下的崩溃日志是否存在，这里的exists（public属性）属于类外调用
    if(QFile::exists(CRASH_LOG_PATH))
    {
        //申请一个崩溃日志条目
        CrashLogItem *item = new CrashLogItem;
        //取名
        item->setName("System Crash");
        //采集原始崩溃日志的内容
        item->collect(CRASH_LOG_PATH);
        //将这个原始崩溃日志添加到全局对象错误日志的链表中
        errorLog.append(item);
        //移除这个原始崩溃日志
        QFile::remove(CRASH_LOG_PATH);
    }
}

bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event)
{
    return QCoreApplication::sendSpontaneousEvent(receiver, event);
}

// 启动加载功能，它读取系统板的实际模式，并将其转换成真正系统需要的工作模式，并根据模式的类型调用不同的启动如果。
int main(int argc, char *argv[])
{
    IApplication app(argc, argv);
    //捕捉段错误
    catchFatalError();
    //检查上次程序运行期间是否有错误发生
    checkCrash();
    //申请一个错误日志条目
    ErrorLogItem *item = new ErrorLogItem();
    //取名
    item->setName("Power On");
    //将这个日志添加到全局对象错误日志的链表中
    errorLog.append(item);
    // 构造出系统参数对象，用于获取当前面板上开关的位置。
    systemBoardProvider.construction();
    systemManager.construction();
    // 下面开始分析请求的模式，并决定系统最终的运行模式。
    appMain(app);
}
