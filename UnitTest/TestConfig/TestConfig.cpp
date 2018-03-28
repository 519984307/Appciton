#include <assert.h>
#include <QApplication>
#include "Debug.h"
#include "IConfig.h"

static void _read_write_test(void)
{
    int value;
    QString str;

    // 测试int数据读写。
    machineConfig.setNumValue("ScreenWidth", 600);
    machineConfig.getNumValue("ScreenWidth", value);
    assert(value == 600);

    // 测量string读写。
    layoutConfig.setStrValue("Style|Opt0", "str0");
    layoutConfig.getStrValue("Style|Opt0", str);
    assert(str == "str0");

    // 属性读写测试。
    layoutConfig.setNumAttr("MonitorNormal", "NodeCount", 3);
    layoutConfig.getNumAttr("MonitorNormal", "NodeCount", value);
    assert(value == 3);

    layoutConfig.setStrAttr("MonitorNormal|Node0", "Name", "NULL");
    layoutConfig.getStrAttr("MonitorNormal|Node0", "Name", str);
    assert(str == "NULL");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    catchFatalError(); // 处理ctrl+c，kill等信号，捕捉段错误, 自动打印函数栈

    _read_write_test();

    layoutConfig.save();
    layoutConfig.saveToDisk();

    qDebug("All test cases in TestConfig.cpp are passed!");
    return a.exec();
}
