/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/30
 **/



#include <QApplication>
#include "NormalModeKeyAction.h"
#include "Debug.h"
#include "Alarm.h"
#include "PopupWidget.h"
#include "NIBPParam.h"
#include "CO2Param.h"
#include "SoundManager.h"
#include "SoftKeyManager.h"
#include "ECGParam.h"
#include "WindowManager.h"
#include "ComboListPopup.h"
#include "SystemManager.h"
#include "SoundManager.h"
#include "SystemAlarm.h"
#include "MenuManager.h"
#include <QPointer>
#include "RecorderManager.h"
#include "ContinuousPageGenerator.h"
#include "FreezeWindow.h"
#include "AlarmStateMachine.h"
#include "MainMenuWindow.h"


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
    // nibp
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        nibpParam.keyPressed();
    }
}

void NormalModeKeyAction::keyF2Pressed(bool multiBtnPress)
{

//    static QPointer<FreezeWidget> currentFreezeWidget;
    static QPointer<FreezeWindow> currentFreezeWidget;
    if (multiBtnPress)
    {
        return;
    }

    if (currentFreezeWidget)
    {
        currentFreezeWidget->done(0);
        return;
    }

    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->close();
    }

//    FreezeWidget freezeWidget;
    FreezeWindow freezeWidget;
    currentFreezeWidget = &freezeWidget;
    freezeWidget.exec();
}

void NormalModeKeyAction::keyF3Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    // print
    recorderManager.addPageGenerator(new ContinuousPageGenerator());
}

void NormalModeKeyAction::keyF4Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    // menu
    // handle event when release
}

void NormalModeKeyAction::keyF5Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    alertor.updateMuteKeyStatus(true);
}

void NormalModeKeyAction::keyF6Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    alertor.updateResetKeyStatus(true);
}

void NormalModeKeyAction::keyF1Released(bool multiBtnPress)
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

void NormalModeKeyAction::keyF2Released(bool multiBtnPress)
{

    if (multiBtnPress)
    {
        return;
    }

    // TODO: freeze
}

void NormalModeKeyAction::keyF3Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }
    // print button release
}

void NormalModeKeyAction::keyF4Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    bool closePupup = false;
    while (QApplication::activePopupWidget())
    {
        QApplication::activePopupWidget()->close();
        closePupup = true;
    }

    if (closePupup)
    {
        return;
    }


    QWidget *wm = &windowManager;
    QWidget *activeWindow = QApplication::activeWindow();
    if (activeWindow == wm || activeWindow == NULL)
    {
        MainMenuWindow *w = MainMenuWindow::getInstance();
        windowManager.showWindow(w, WindowManager::ShowBehaviorNone);
    }
    else
    {
        while (QApplication::activeModalWidget())
        {
            QApplication::activeModalWidget()->close();
        }
        QTimer::singleShot(0, &windowManager, SLOT(closeAllWidows()));
        softkeyManager.resetPage();
    }
}

void NormalModeKeyAction::keyF5Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    alertor.updateMuteKeyStatus(false);
}

void NormalModeKeyAction::keyF6Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    alertor.updateResetKeyStatus(false);
}
