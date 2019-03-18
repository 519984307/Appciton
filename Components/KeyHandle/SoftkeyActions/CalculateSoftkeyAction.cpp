/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/1
 **/



#include <QApplication>
#include "CalculateSoftkeyAction.h"
#include "WindowManager.h"
#include "SoftKeyManager.h"
#include "MenuGroup.h"
#include "MenuManager.h"
#include "DataStorageDirManager.h"
#include "DoseCalculationWindow.h"
#include "DoseCalculationManager.h"
#include "HemodynamicWindow.h"
#include "LanguageManager.h"
#include "LayoutManager.h"

/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _calculationDataKeys[] =
{
//    KeyActionDesc("", "Trend.png",   CalculateSoftkeyAction::trendReview),
//    KeyActionDesc("", "Summary.png", CalculateSoftkeyAction::summaryReview),
//    KeyActionDesc("", "Export.png",  CalculateSoftkeyAction::exportData),
//    KeyActionDesc("", "Delete.png",  CalculateSoftkeyAction::clearData),
//    KeyActionDesc("", "SoftkeyArrow.png", CalculateSoftkeyAction::exit),

    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "Dose", "dosecalculation.png", CalculateSoftkeyAction::doseCalculation),
    KeyActionDesc("", "Blood", "blood.png", CalculateSoftkeyAction::Hemodynamic),
    KeyActionDesc("", "", "SoftkeyArrow.png", CalculateSoftkeyAction::exit),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_MAIN_SETUP)
};

/***************************************************************************************************
 * 药物计算回调。
 **************************************************************************************************/
void CalculateSoftkeyAction::doseCalculation(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    bool isVisible = DoseCalculationWindow::getInstance()->isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
    }

    if (isVisible)
    {
        return;
    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - patientMenu.width()) / 2;
//    int y = r.y() + (r.height() - patientMenu.height());
//    patientMenu.autoShow(x, y);
//    //    patientMenu.autoShow();
//    doseCalculationWidget.autoShow();
    windowManager.showWindow(DoseCalculationWindow::getInstance(),
                             WindowManager::ShowBehaviorModal);
}

/***************************************************************************************************
 * 血液动力学回调。
 **************************************************************************************************/
void CalculateSoftkeyAction::Hemodynamic(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

//    bool isVisible = hemodynamicWidget.isVisible();
    bool isVisible = HemodynamicWindow::getInstance()->isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
    }

    if (isVisible)
    {
        return;
    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - patientMenu.width()) / 2;
//    int y = r.y() + (r.height() - patientMenu.height());
//    patientMenu.autoShow(x, y);
//    //    patientMenu.autoShow();
    windowManager.showWindow(HemodynamicWindow::getInstance(),
                             WindowManager::ShowBehaviorModal);
}

void CalculateSoftkeyAction::exit(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    SoftKeyActionType type = softkeyManager.uFaceTypeToSoftKeyType(layoutManager.getUFaceType());
    softkeyManager.setContent(type);
}

int CalculateSoftkeyAction::getActionDescNR(void)
{
    return sizeof(_calculationDataKeys) / sizeof(KeyActionDesc);
}

/***************************************************************************************************
 * 获取动作描述句柄。
 **************************************************************************************************/
KeyActionDesc *CalculateSoftkeyAction::getActionDesc(int index)
{
    if (index >= static_cast<int>(sizeof(_calculationDataKeys) / sizeof(KeyActionDesc)))
    {
        return NULL;
    }

    if (_calculationDataKeys[index].type < SOFT_BASE_KEY_NR)
    {
        return SoftkeyActionBase::getActionDesc(_calculationDataKeys[index].type);
    }

    return &_calculationDataKeys[index];
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
CalculateSoftkeyAction::CalculateSoftkeyAction() : SoftkeyActionBase(SOFTKEY_ACTION_CALCULATE)
{
}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
CalculateSoftkeyAction::~CalculateSoftkeyAction()
{
}
