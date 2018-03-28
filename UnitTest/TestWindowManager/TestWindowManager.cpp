#include <QApplication>
#include <QString>
#include "IConfig.h"
#include "AlarmMuteIndicator.h"
#include "DateTimeWidget.h"
#include "ElapsedTimeWidget.h"
#include "PatientInfoWidget.h"
#include "SystemStateWidget.h"
#include "PhysAlarmInfoWidget.h"
#include "TechAlarmInfoWidget.h"
#include "SoftKeyManager.h"
#include "WindowManager.h"
#include "Debug.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    catchFatalError(); // 处理ctrl+c，kill等信号，捕捉段错误, 自动打印函数栈

    windowManager.Construction();

    // Info bar
    windowManager.addWidget(new DateTimeWidget());
    windowManager.addWidget(new PatientInfoWidget());
    windowManager.addWidget(new SystemStateWidget());
    windowManager.addWidget(new ElapsedTimeWidget());

    // Alarm bar
    PhysAlarmInfoWidget *physAlarmInfo = new PhysAlarmInfoWidget();
    TechAlarmInfoWidget *techAlarmInfo = new TechAlarmInfoWidget();
    windowManager.addWidget(new AlarmMuteIndicator());
    windowManager.addWidget(physAlarmInfo);
    windowManager.addWidget(techAlarmInfo);

    // Softkeys
    windowManager.addWidget(new SoftKeyManager());

    // Layouts
    int styleCount = 0;
    layoutConfig.getNumAttr("Style", "Count", styleCount);
    for (int i = 0; i < styleCount; i++)
    {
        QString styleName;
        QString indexStr = QString("%1|%2").arg("Style", "Opt" + QString::number(i));
        layoutConfig.getStrValue(indexStr, styleName);
    }

    // Set window size
    int screenWidth = 0;
    int screenHeight = 0;
    machineConfig.getNumValue("ScreenWidth", screenWidth);
    machineConfig.getNumValue("ScreenHeight", screenHeight);
    windowManager.setFixedSize(screenWidth, screenHeight);

    // Layout & Show
    windowManager.setUFaceType(MONITOR_NORMAL_UFACE);
    windowManager.show();

    int ret = a.exec();
    deleteWindowManager();
    return ret;
}

