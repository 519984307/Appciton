#include <QApplication>
#include "Debug.h"
#include "DateTime.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    catchFatalError(); // 处理ctrl+c，kill等信号，捕捉段错误, 自动打印函数栈

    QString str;
    dateTimeCtrl.getTime(str);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getTime(str, true);
    qDebug("%s", str.toAscii().data());

    dateTimeCtrl.getDate(str);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getDate(str, true);
    qDebug("%s", str.toAscii().data());

    dateTimeCtrl.getDateTime(str, true, true);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getDateTime(str, true, false);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getDateTime(str, false, true);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getDateTime(str, false, false);
    qDebug("%s", str.toAscii().data());

    unsigned t = dateTimeCtrl.Time();
    dateTimeCtrl.getTime(t, str);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getTime(t, str, true);
    qDebug("%s", str.toAscii().data());

    dateTimeCtrl.getDate(t, str);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getDate(t, str, true);
    qDebug("%s", str.toAscii().data());

    dateTimeCtrl.getDateTime(t, str, true, true);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getDateTime(t, str, true, false);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getDateTime(t, str, false, true);
    qDebug("%s", str.toAscii().data());
    dateTimeCtrl.getDateTime(t, str, false, false);
    qDebug("%s", str.toAscii().data());

    qDebug("All test cases in TestDateTime.cpp are passed!");
    return a.exec();
}
