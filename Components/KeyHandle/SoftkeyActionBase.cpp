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
#include "PatientInfoWindow.h"
#include "MessageBox.h"
#include "CO2Param.h"
#include "SystemManager.h"
#include "DataStorageDirManager.h"
#include "DoseCalculationManager.h"
#include "HemodynamicWidget.h"
#include "Window.h"
#include "MenuSidebar.h"
#include "MenuWindow.h"
#include "Frame.h"
#include "FrameItem.h"
#include "ComboBox.h"
#include "PatientInfoWindow.h"
#include "CodeMarkerWindow.h"
#include "ScreenLayoutWindow.h"
#include "MainMenuWindow.h"
#include "TrendGraphWindow.h"
#include "EventWindow.h"
#include "IBPParam.h"
#include "IConfig.h"
#include "NightModeManager.h"
#include "MenuSidebarItem.h"

/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _baseKeys[] =
{
    KeyActionDesc("", "", "main.png",  SoftkeyActionBase::mainsetup
                    , SOFT_BASE_KEY_NR, true, QColor(255, 200, 0)),
    KeyActionDesc("", trs("Patient"), "PatientInfo.png", SoftkeyActionBase::patientInfo),
    KeyActionDesc("", trs("PatientNew"), "PatientNew.png", SoftkeyActionBase::patientNew),
    KeyActionDesc("", trs("ECGLeadMode"), "LeadSelection.png", SoftkeyActionBase::ecgLeadChange),
    KeyActionDesc("", trs("AlarmSettingMenu"), "limitSet.png", SoftkeyActionBase::limitMenu),
    KeyActionDesc("", trs("CodeMarker"), "CodeMarker.png", SoftkeyActionBase::codeMarker),
    KeyActionDesc("", trs("TrendGraph"), "Summary.png", SoftkeyActionBase::summaryReview),
    KeyActionDesc("", trs("eventReview"), "Summary.png", SoftkeyActionBase::eventReview),
    KeyActionDesc("", trs("ChooseScreen"), "screenSwitch.png", SoftkeyActionBase::switchSystemMode),
    KeyActionDesc("", trs("ScreenSetup"), "interface.png",   SoftkeyActionBase::WindowLayout),
    KeyActionDesc("", trs("ParameterSwitch"), "paraSwitch.png"),
    KeyActionDesc("", trs("LockScreen"), "lockScreen.png", SoftkeyActionBase::lockScreen),
    KeyActionDesc("", trs("Standby"), "standby.png", SoftkeyActionBase::standby),
    KeyActionDesc("", trs("CO2ZeroCalib"), "calib.png", SoftkeyActionBase::CO2Zero),
    KeyActionDesc("", trs("CO2Standby"), "standby.png", SoftkeyActionBase::CO2Standby),
    KeyActionDesc("", trs("CO2Measure"), "measure.png", SoftkeyActionBase::CO2Measure),
    KeyActionDesc("", trs("IBPZeroCalib"), "calib.png", SoftkeyActionBase::IBPZero),
    KeyActionDesc("", trs("Calculation"), "dosecalculation.png", SoftkeyActionBase::calculation),
    KeyActionDesc("", trs("KeyBoardVolumn"), "keyBoard.png", SoftkeyActionBase::sysSetup),
    KeyActionDesc("", trs("SystemBrightness"), "Brightness.png", SoftkeyActionBase::sysSetup),
    KeyActionDesc("", trs("NightMode"), "nightMode.png", SoftkeyActionBase::nightMode),
    KeyActionDesc("", trs("PrintSetup"), "printSetup.png", SoftkeyActionBase::sysSetup),
};

/***************************************************************************************************
 * 导联改变回调。
 **************************************************************************************************/
void SoftkeyActionBase::ecgLeadChange(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
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
    MainMenuWindow *p = MainMenuWindow::getInstance();
    p->popup(trs("DataReviewMenu"));
}

void SoftkeyActionBase::calculation(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    MainMenuWindow *p = MainMenuWindow::getInstance();
    p->popup(trs("CalculateMenu"));
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

    // 每次打开主界面时，强制聚焦在首个item
    // 需要放在showWindow下面
    w->setFirstMenuSidebarItemFocus();
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
    MainMenuWindow *w = MainMenuWindow::getInstance();
    w->popup(trs("ScreenConfig"));
}

/***************************************************************************************************
 * summary回顾回调。
 **************************************************************************************************/
void SoftkeyActionBase::summaryReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    TrendGraphWindow::getInstance()->setHistoryData(false);
    windowManager.showWindow(TrendGraphWindow::getInstance(),
                             WindowManager::ShowBehaviorCloseIfVisiable | WindowManager::ShowBehaviorCloseOthers);
}

void SoftkeyActionBase::eventReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    EventWindow::getInstance()->setHistoryData(false);
    windowManager.showWindow(EventWindow::getInstance(), WindowManager::ShowBehaviorCloseIfVisiable
                             | WindowManager::ShowBehaviorCloseOthers);
}

void SoftkeyActionBase::standby(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    // TODO: 待机功能实现
}

void SoftkeyActionBase::CO2Zero(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    co2Param.zeroCalibration();
}

void SoftkeyActionBase::CO2Standby(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    // TODO: CO2待机
}

void SoftkeyActionBase::CO2Measure(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    // TODO: CO2测量
}

void SoftkeyActionBase::IBPZero(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    ibpParam.zeroCalibration(IBP_INPUT_1);
}

void SoftkeyActionBase::sysSetup(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    MainMenuWindow *w = MainMenuWindow::getInstance();
    w->popup(trs("SystemMenu"));
}

void SoftkeyActionBase::nightMode(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    nightModeManager.setNightMode();
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

    return &_baseKeys[index];
}

KeyActionDesc *SoftkeyActionBase::getBaseActionDesc(SoftBaseKeyType baseType)
{
    return &_baseKeys[baseType];
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

