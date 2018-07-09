#include <QApplication>
#include "NormalModeKeyAction.h"
#include "Debug.h"
#include "PublicMenuManager.h"
#include "Alarm.h"
#include "PopupWidget.h"
#include "NIBPParam.h"
#include "CO2Param.h"
#include "SoundManager.h"
#include "AlarmLimitMenu.h"
#include "SoftKeyManager.h"
#include "ECGParam.h"
#include "WindowManager.h"
#include "ECGMenu.h"
#include "ComboListPopup.h"
#include "SystemManager.h"
#include "SoundManager.h"
#include "SystemAlarm.h"
#include "MenuManager.h"
#include "FreezeWidget.h"
#include <QPointer>
#include "RecorderManager.h"
#include "ContinuousPageGenerator.h"
#include "AlarmStateMachine.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NormalModeKeyAction::NormalModeKeyAction()
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NormalModeKeyAction::~NormalModeKeyAction()
{

}

void NormalModeKeyAction::keyF1Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    //menu
    //handle event when release
}

void NormalModeKeyAction::keyF2Pressed(bool multiBtnPress)
{

    static QPointer<FreezeWidget> currentFreezeWidget;
    if (multiBtnPress)
    {
        return;
    }

    if(currentFreezeWidget)
    {
        currentFreezeWidget->done(0);
        return;
    }

    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->close();
    }

    FreezeWidget freezeWidget;
    currentFreezeWidget = &freezeWidget;
    freezeWidget.exec();
}

void NormalModeKeyAction::keyF3Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    //print
    recorderManager.addPageGenerator(new ContinuousPageGenerator());
}

void NormalModeKeyAction::keyF4Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }
    //nibp
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        nibpParam.keyPressed();
    }
}

void NormalModeKeyAction::keyF5Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    alarmStateMachine.alarmPause(true);
}

void NormalModeKeyAction::keyF6Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    alarmStateMachine.alarmReset(true);
}

void NormalModeKeyAction::keyF1Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
        return;
    }

    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - publicMenuManager.width()) / 2;
    int y = r.y() + (r.height() - publicMenuManager.height());
    publicMenuManager.popup(x, y);
}

void NormalModeKeyAction::keyF2Released(bool multiBtnPress)
{

    if (multiBtnPress)
    {
        return;
    }

    //TODO: freeze
}

void NormalModeKeyAction::keyF3Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }
    //print button release
}

void NormalModeKeyAction::keyF4Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        nibpParam.keyReleased();
    }
}

void NormalModeKeyAction::keyF5Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    alarmStateMachine.alarmPause(false);
}

void NormalModeKeyAction::keyF6Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    alarmStateMachine.alarmReset(false);
}
