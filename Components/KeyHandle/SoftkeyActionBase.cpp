/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#include "SoftkeyActionBase.h"
#include "SoftKeyManager.h"
#include <QApplication>
#include "SoftKeyWidget.h"
#include "ECGParam.h"
#include "ECGMenu.h"
#include "PublicMenuManager.h"
#include "AlarmLimitMenu.h"
#include "CodeMarkerWidget.h"
#include "SystemBoardProvider.h"
#include "WindowManager.h"
#include "WindowLayout.h"
#include "PatientInfoWindow.h"
#include "MessageBox.h"
#include "CO2Param.h"
#include "SystemManager.h"
#include "DataStorageDirManager.h"
#include "DoseCalculationWidget.h"
#include "DoseCalculationManager.h"
#include "HemodynamicWidget.h"
#include "Window.h"
#include "MenuSidebar.h"
#include "MenuWindow.h"
#include "Frame.h"
#include "FrameItem.h"
#include "ComboBox.h"
#include "PatientInfoWindow.h"
#include "MainMenuWindow.h"
#include "CodeMarkerWindow.h"
#include "ScreenLayoutWindow.h"

/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _baseKeys[] =
{
//    KeyActionDesc("", "LeadSelection.png", SoftkeyActionBase::ecgLeadChange),
//    KeyActionDesc("", "CodeMarker.png",    SoftkeyActionBase::codeMarker),
//    KeyActionDesc("", "Bell.png",          SoftkeyActionBase::limitMenu),
//    KeyActionDesc("", "Data.png",          SoftkeyActionBase::rescueData),
//    KeyActionDesc("", "PatientInfo.png",   SoftkeyActionBase::patientInfo),
//    KeyActionDesc("", "CO2Disable.png",    SoftkeyActionBase::co2OnOff),
//    KeyActionDesc("", "SoftkeyArrow.png",  SoftkeyActionBase::nextPage),
    KeyActionDesc("", trs("SoftKeyPatientInfo"), "PatientInfo.png",   SoftkeyActionBase::patientInfo,
    SOFT_BASE_KEY_NR, true, QColor(143, 31, 132)),
    KeyActionDesc("", trs("PatientNew"), "PatientNew.png"
                    , SoftkeyActionBase::patientNew, SOFT_BASE_KEY_NR, true, QColor(143, 31, 132)),
    KeyActionDesc("", "", ICON_FILE_LEFT,  SoftkeyActionBase::previousPage),
    KeyActionDesc("", trs("AlarmSettingMenu"), "limitSet.png", SoftkeyActionBase::limitMenu),
    KeyActionDesc("", trs("CodeMarker"), "CodeMarker.png", SoftkeyActionBase::codeMarker),
    KeyActionDesc("", trs("RescueData"), "Data.png",       SoftkeyActionBase::rescueData),
    KeyActionDesc("", trs("Interface"), "interface.png",   SoftkeyActionBase::WindowLayout),
    KeyActionDesc("", trs("Calculation"), "dosecalculation.png", SoftkeyActionBase::calculation),
    KeyActionDesc("", trs("LockScreen"), "LockScreen.png", SoftkeyActionBase::lockScreen),
    KeyActionDesc("", trs("UserFaceStandard"), "standard.png", SoftkeyActionBase::switchSystemMode),
    KeyActionDesc("", trs("UserFace12Lead"), "12_Lead.png", SoftkeyActionBase::switchSystemMode),
    KeyActionDesc("", trs("UserFaceOxyCRG"), "OxyCRG.png", SoftkeyActionBase::switchSystemMode),
    KeyActionDesc("", trs("UserFaceTrend"), "trend.png", SoftkeyActionBase::switchSystemMode),
    KeyActionDesc("", trs("UserFaceBigFont"), "bigFont.png", SoftkeyActionBase::switchSystemMode),
    KeyActionDesc("", trs("UserFaceCustom"), "custom.png", SoftkeyActionBase::switchSystemMode),
    KeyActionDesc("", trs("UserFaceUnknow"), "unknow.png", SoftkeyActionBase::switchSystemMode),
    KeyActionDesc("", "", ICON_FILE_RIGHT,  SoftkeyActionBase::nextPage),
    KeyActionDesc("", "", "main.png",  SoftkeyActionBase::mainsetup
                    , SOFT_BASE_KEY_NR, true, QColor(255, 200, 0)),
};

static int _currentSystemMode = 0;

/***************************************************************************************************
 * 导联改变回调。
 **************************************************************************************************/
void SoftkeyActionBase::ecgLeadChange(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

// co2Param.setConnected(false);
// return;
    ecgParam.autoSetCalcLead();
}

/***************************************************************************************************
 * CodeMarker回调。
 **************************************************************************************************/
void SoftkeyActionBase::codeMarker(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

// co 2Param.setConnected(true);
// return;

    bool isVisible = codeMarkerWindow.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
    }

    if (isVisible)
    {
        return;
    }
    windowManager.showWindow(&codeMarkerWindow , WindowManager::ShowBehaviorCloseIfVisiable |
                             WindowManager::ShowBehaviorCloseOthers);
}

void SoftkeyActionBase::previousPage(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.previousPage();
}

/***************************************************************************************************
 * 下一页面回调。
 **************************************************************************************************/
void SoftkeyActionBase::nextPage(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.nextPage();
}

/***************************************************************************************************
 * 报警限设置回调。
 **************************************************************************************************/
void SoftkeyActionBase::limitMenu(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    MainMenuWindow *p = MainMenuWindow::getInstance();
    p->popup(trs("AlarmLimitMenu"));
}

void SoftkeyActionBase::WindowLayout(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    bool isVisible = windowLayout.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
    }

    if (isVisible)
    {
        return;
    }

    // windowLayout.autoShow();
    windowManager.showWindow(ScreenLayoutWindow::getInstance(),
                             WindowManager::ShowBehaviorCloseIfVisiable | WindowManager::ShowBehaviorCloseOthers);
}

/***************************************************************************************************
 * 数据查看回调。
 **************************************************************************************************/
void SoftkeyActionBase::rescueData(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.setContent(SOFTKEY_ACTION_RESCUE_DATA);
}

void SoftkeyActionBase::calculation(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.setContent(SOFTKEY_ACTION_CALCULATE);
}

/***************************************************************************************************
 * 病人信息。
 **************************************************************************************************/
void SoftkeyActionBase::patientInfo(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    patientInfoWindow.newPatientStatus(false);
    patientInfoWindow.widgetChange();
    windowManager.showWindow(&patientInfoWindow , WindowManager::ShowBehaviorCloseOthers
                             | WindowManager::ShowBehaviorCloseIfVisiable);
}

void SoftkeyActionBase::patientNew(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    bool isVisible = patientInfoWindow.isVisible();

    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
    }

    if (isVisible)
    {
        return;
    }

    // 创建新病人
    // patientMenu.createPatient();
    QStringList slist;
    slist << trs("No") << trs("EnglishYESChineseSURE");
    MessageBox messageBox(trs("Warn"), trs("RemoveAndRecePatient"), slist);
    if (messageBox.exec() == 1)
    {
        dataStorageDirManager.createDir(true);
        patientInfoWindow.newPatientStatus(true);
        patientInfoWindow.widgetChange();
        windowManager.showWindow(&patientInfoWindow , WindowManager::ShowBehaviorCloseOthers);
    }
}

void SoftkeyActionBase::mainsetup(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    MainMenuWindow *w = MainMenuWindow::getInstance();
    windowManager.showWindow(w, WindowManager::ShowBehaviorCloseOthers);
}

void SoftkeyActionBase::lockScreen(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    // TODO : lockScreen
}

void SoftkeyActionBase::switchSystemMode(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    UserFaceType type = static_cast<UserFaceType>(_currentSystemMode);
    windowManager.setUFaceType(type);
}

/***************************************************************************************************
 * 获取动作描述总个数。
 **************************************************************************************************/
int SoftkeyActionBase::getActionDescNR(void)
{
    return 0;
}

/***************************************************************************************************
 * 获取动作描述句柄。
 **************************************************************************************************/
KeyActionDesc *SoftkeyActionBase::getActionDesc(int index)
{
    if (index > SOFT_BASE_KEY_NR)
    {
        return NULL;
    }

    if (index == SOFT_BASE_KEY_PREVIOUS_PAGE)
    {
        if (softkeyManager.hasPreviousPage())
        {
            _baseKeys[index].iconPath = ICON_FILE_LEFT;
            _baseKeys[index].focus = true;
        }
        else
        {
            _baseKeys[index].iconPath = "";
            _baseKeys[index].focus = false;
        }
    }
    else if (index == SOFT_BASE_KEY_NEXT_PAGE)
    {
        if (softkeyManager.hasNextPage())
        {
            _baseKeys[index].iconPath = ICON_FILE_RIGHT;
            _baseKeys[index].focus = true;
        }
        else
        {
            _baseKeys[index].iconPath = "";
            _baseKeys[index].focus = false;
        }
    }

    else if (index >= SOFT_BASE_KEY_SYS_MODE_STANDARD
             && index <= SOFT_BASE_KEY_SYS_MODE_UNKNOW)
    {
        _currentSystemMode = index - SOFT_BASE_KEY_SYS_MODE_STANDARD;
    }
    return &_baseKeys[index];
}

/***************************************************************************************************
 * 获取类型。
 **************************************************************************************************/
SoftKeyActionType SoftkeyActionBase::getType(void)
{
    return _type;
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
SoftkeyActionBase::SoftkeyActionBase(SoftKeyActionType t)
    :_type(t)
{
}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
SoftkeyActionBase::~SoftkeyActionBase()
{
}

