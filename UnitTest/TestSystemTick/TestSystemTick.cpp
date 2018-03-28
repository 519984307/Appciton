#include <QApplication>
#include <QTime>
#include "Debug.h"
#include "SystemTick.h"

static void _100MS(void)
{
    static bool flag = false;
    static QTime t;

    flag = !flag;
    if(flag)
    {
        t.restart();
    }
    else
    {
        qDebug("100MS Time elapsed: %d ms", t.elapsed());
    }
}

static void _500MS(void)
{
    static bool flag = false;
    static QTime t;

    flag = !flag;
    if(flag)
    {
        t.restart();
    }
    else
    {
        qDebug("\t500MS Time elapsed: %d ms", t.elapsed());
    }
}

static void _1S(void)
{
    static bool flag = false;
    static QTime t;

    flag = !flag;
    if(flag)
    {
        t.restart();
    }
    else
    {
        qDebug("\t\t1000MS Time elapsed: %d ms", t.elapsed());
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    catchFatalError(); // 处理ctrl+c，kill等信号，捕捉段错误, 自动打印函数栈

    systemTick.addHook(100, 0, _100MS);
    systemTick.addHook(500, 0, _500MS);
    systemTick.addHook(1000, 0, _1S);
    systemTick.startTick();

    qDebug("All test cases in TestSystemTick.cpp are passed!");
    return a.exec();
}
