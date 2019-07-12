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
#include "LayoutManager.h"
#include "TrendDataStorageManager.h"
#include "TimeManager.h"
#include <QTimer>
#include "TrendCache.h"

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
    // nibp 维护模式和demo模式、开机较零模式不响应nibp测量按钮
    if (systemManager.isSupport(CONFIG_NIBP) && systemManager.getCurWorkMode() != WORK_MODE_DEMO
            && !nibpParam.isMaintain() && !nibpParam.isZeroSelfTestState())
    {
        nibpParam.keyPressed();
    }
}

void NormalModeKeyAction::keyF2Pressed(bool multiBtnPress)
{

    static QPointer<FreezeWindow> currentFreezeWindow;
    if (multiBtnPress)
    {
        return;
    }

    if (layoutManager.getUFaceType() == UFACE_MONITOR_BIGFONT)
    {
        // should not freeze when in big font interface
        return;
    }

    if (currentFreezeWindow)
    {
        currentFreezeWindow->done(0);
        return;
    }

    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->close();
    }

    windowManager.closeAllWidows();
    FreezeWindow freezeWindow;
    currentFreezeWindow = &freezeWindow;
    freezeWindow.exec();
}

void NormalModeKeyAction::keyF3Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    // print
    if (recorderManager.isPrinting())
    {
        recorderManager.stopPrint();
    }
    else if (!recorderManager.getPrintStatus())
    {
        unsigned t = timeManager.getCurTime();
        recorderManager.addPageGenerator(new ContinuousPageGenerator());
        trendCache.collectTrendData(t);
        trendCache.collectTrendAlarmStatus(t);
        trendDataStorageManager.storeData(t, TrendDataStorageManager::CollectStatusPrint);
    }
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

    //  维护模式和demo模式不响应nibp测量按钮
    if (systemManager.isSupport(CONFIG_NIBP) && systemManager.getCurWorkMode() != WORK_MODE_DEMO
            && !nibpParam.isMaintain() && !nibpParam.isZeroSelfTestState())
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
        // 每次打开主界面时，强制聚焦在首个item
        // 需要放在showWindow下面
        w->focusFirstMenuItem();
    }
    else
    {
        while (QApplication::activeModalWidget())
        {
            QApplication::activeModalWidget()->close();
        }
        QTimer::singleShot(0, &windowManager, SLOT(closeAllWidows()));
        softkeyManager.refreshPage();
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
